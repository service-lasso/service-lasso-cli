"""UNRUN CLI39 prerequisite: bounded XZ or GZIP/tar full materialization.

Requires separately admitted plan, Python/liblzma/source/runtime and original
native command owner. No build, imports from target source or subprocesses.
Failure retains the fresh destination and every already written original.
"""
import hashlib
import json
import lzma
import os
from pathlib import Path
import sys
import time
import zlib


FIELDS = {"schema", "compression", "archive", "archiveBytes", "archiveSHA256", "rootName",
          "outputParent", "outputLeaf", "chunkBytes", "xzMemoryBytes",
          "maxTarBytes", "maxMembers", "maxMemberBytes", "maxPayloadBytes",
          "maxPathBytes", "maxArtifactBytes", "milliseconds"}
LIMITS = FIELDS - {"schema", "compression", "archive", "archiveSHA256", "rootName",
                   "outputParent", "outputLeaf"}


class Materialization:
    def __init__(self, plan):
        if set(plan) != FIELDS or plan["schema"] != "cli39.full-source-materialization.v2":
            raise ValueError("plan schema")
        if plan["compression"] not in ("xz", "gzip"):
            raise ValueError("explicit archive compression")
        for key in LIMITS:
            if type(plan[key]) is not int or not 0 < plan[key] <= sys.maxsize:
                raise ValueError("finite plan geometry")
        if (plan["chunkBytes"] > plan["maxTarBytes"] or
                plan["maxMemberBytes"] > plan["maxPayloadBytes"]):
            raise ValueError("inconsistent plan geometry")
        digest = plan["archiveSHA256"]
        if not isinstance(digest, str) or len(digest) != 64 or any(c not in "0123456789abcdef" for c in digest):
            raise ValueError("archive pin")
        for key in ("rootName", "outputLeaf"):
            value = plan[key]
            if not isinstance(value, str) or not value or value in (".", "..") or any(c in value for c in "/\\:\x00"):
                raise ValueError("fresh leaf")
        self.plan = plan
        self.until = time.monotonic_ns() + plan["milliseconds"] * 1000000
        self.parent = Path(plan["outputParent"])
        self.archive = Path(plan["archive"])
        if not self.parent.is_absolute() or not self.archive.is_absolute():
            raise ValueError("absolute originals")
        self.output = self.parent / plan["outputLeaf"]
        self.rows = []
        self.controls = []
        self.payload = 0
        self.artifact_bytes = 0
        self.tick()
        # The admitted native owner supplies actual private parent-directory
        # writer exclusion. Lexical/realpath checks alone do not prove that.
        if self.parent.resolve(strict=True) != self.parent:
            raise ValueError("canonical output parent")
        os.mkdir(self.output)
        self.tick()

    def tick(self):
        if time.monotonic_ns() >= self.until:
            raise TimeoutError("local materialization work limit; original native owner retains custody")

    def fresh_json(self, leaf, value):
        self.tick()
        with open(self.output / leaf, "x", encoding="utf-8", newline="\n") as stream:
            for block in json.JSONEncoder(ensure_ascii=True, separators=(",", ":")).iterencode(value):
                self.tick()
                self.artifact_bytes += len(block.encode("utf-8"))
                if self.artifact_bytes > self.plan["maxArtifactBytes"]:
                    raise ValueError("artifact extent exceeded")
                stream.write(block)
            self.artifact_bytes += 1
            if self.artifact_bytes > self.plan["maxArtifactBytes"]:
                raise ValueError("artifact extent exceeded")
            stream.write("\n")
        self.tick()

    def decompress(self):
        p = self.plan
        gzip = p["compression"] == "gzip"
        # The qualified zlib GZIP wrapper validates the complete RFC1952
        # header (including optional fields/FHCRC), DEFLATE stream, CRC32 and
        # ISIZE trailer. No automatic second member or raw-DEFLATE fallback.
        # Its module, allocator/window and linked native inputs are admitted
        # separately; xzMemoryBytes is not a claimed zlib allocation budget.
        decoder = (zlib.decompressobj(wbits=31) if gzip else
                   lzma.LZMADecompressor(format=lzma.FORMAT_XZ, memlimit=p["xzMemoryBytes"]))
        digest = hashlib.sha256()
        compressed = expanded = 0
        with open(self.archive, "rb") as source, open(self.output / "original.tar", "xb") as target:
            while True:
                self.tick()
                raw = source.read(p["chunkBytes"])
                if not raw:
                    break
                compressed += len(raw)
                if compressed > p["archiveBytes"] or decoder.eof:
                    raise ValueError("extra compressed bytes")
                digest.update(raw)
                while True:
                    self.tick()
                    block = decoder.decompress(raw, max_length=min(p["chunkBytes"], p["maxTarBytes"] - expanded + 1))
                    raw = decoder.unconsumed_tail if gzip else b""
                    expanded += len(block)
                    if expanded > p["maxTarBytes"]:
                        raise ValueError("tar extent exceeded")
                    target.write(block)
                    self.tick()
                    if decoder.eof:
                        if decoder.unused_data:
                            raise ValueError("concatenated/trailing compressed data")
                        break
                    if (gzip and not raw) or (not gzip and decoder.needs_input):
                        break
            if not decoder.eof or compressed != p["archiveBytes"] or digest.hexdigest() != p["archiveSHA256"]:
                raise ValueError("original archive differs or is incomplete")
        self.tick()
        self.fresh_json("decompression.json", {"compression": p["compression"], "compressedBytes": compressed, "tarBytes": expanded,
                                               "sha256": digest.hexdigest()})

    def exact(self, stream, amount):
        self.tick()
        raw = stream.read(amount)
        self.tick()
        if len(raw) != amount:
            raise ValueError("truncated tar")
        return raw

    @staticmethod
    def number(raw):
        if raw[0] & 128:
            if raw[0] & 64:
                raise ValueError("negative tar number")
            return int.from_bytes(bytes([raw[0] & 127]) + raw[1:], "big")
        raw = raw.rstrip(b"\x00 ").lstrip(b" ")
        if any(c not in b"01234567" for c in raw):
            raise ValueError("non-octal tar number")
        return int(raw or b"0", 8)

    def text(self, raw):
        value, separator, tail = raw.partition(b"\x00")
        if separator and any(tail):
            raise ValueError("nonzero tar text suffix")
        return value.decode("utf-8", "strict")

    def path(self, value, directory=False):
        if directory and value.endswith("/"):
            value = value[:-1]
        if (not value or len(value.encode("utf-8")) > self.plan["maxPathBytes"] or
                any(c in value for c in "\\:\x00") or value.startswith("/")):
            raise ValueError("unsafe tar path")
        parts = value.split("/")
        if parts[0] != self.plan["rootName"] or any(part in ("", ".", "..") for part in parts):
            raise ValueError("path outside original root")
        # Reject Windows alternate namespace/normalization and case aliases on
        # every platform, so the SAME complete tree is representable on both.
        devices = {"con", "prn", "aux", "nul", "clock$"}
        for part in parts:
            stem = part.split(".")[0].casefold()
            if (part[-1] in " ." or any(ord(c) < 32 or c in '<>"|?*' for c in part) or
                    stem in devices or (len(stem) == 4 and stem[:3] in ("com", "lpt") and stem[3] in "123456789")):
                raise ValueError("unrepresentable original path")
        return "/".join(parts)

    def pax(self, raw):
        result = {}
        while raw:
            self.tick()
            length, separator, rest = raw.partition(b" ")
            if not separator or not length.isdigit():
                raise ValueError("invalid PAX length")
            count = int(length)
            if count <= len(length) + 2 or count > len(raw):
                raise ValueError("invalid PAX extent")
            record = raw[len(length) + 1:count]
            if not record.endswith(b"\n") or b"=" not in record:
                raise ValueError("invalid PAX record")
            key, value = record[:-1].split(b"=", 1)
            key, value = key.decode("ascii"), value.decode("utf-8", "strict")
            if key in result or key not in {"path", "linkpath", "size", "mtime", "atime", "ctime", "uid", "gid", "uname", "gname"}:
                raise ValueError("duplicate/unsupported PAX field")
            result[key] = value
            raw = raw[count:]
        return result

    def inventory(self):
        p = self.plan
        seen = set()
        global_pax, local_pax = {}, {}
        long_name = long_link = None
        with open(self.output / "original.tar", "rb") as source:
            while True:
                offset = source.tell()
                header = self.exact(source, 512)
                if not any(header):
                    if any(self.exact(source, 512)) or local_pax or long_name is not None or long_link is not None:
                        raise ValueError("invalid tar terminator")
                    trailing = 1024
                    while True:
                        self.tick()
                        tail = source.read(p["chunkBytes"])
                        if not tail:
                            break
                        if any(tail):
                            raise ValueError("nonzero trailing tar bytes")
                        trailing += len(tail)
                    if trailing % 512:
                        raise ValueError("unaligned trailing tar bytes")
                    break
                if len(self.rows) + len(self.controls) >= p["maxMembers"]:
                    raise ValueError("member extent exceeded")
                if self.number(header[148:156]) != sum(header[:148]) + 256 + sum(header[156:]):
                    raise ValueError("tar header checksum")
                if header[257:263] not in (b"ustar\x00", b"ustar ", b"\x00" * 6):
                    raise ValueError("unsupported tar header")
                kind = header[156:157]
                size = self.number(header[124:136])
                if size > p["maxMemberBytes"]:
                    raise ValueError("member size exceeded")
                name = self.text(header[:100])
                prefix = self.text(header[345:500]) if header[257:263] == b"ustar\x00" else ""
                if prefix:
                    name = prefix + "/" + name
                link = self.text(header[157:257])
                if kind in (b"x", b"g", b"L", b"K"):
                    raw = self.exact(source, size)
                    self.controls.append({"headerOffset": offset, "kind": kind.decode("ascii"),
                                          "bytes": size, "sha256": hashlib.sha256(raw).hexdigest()})
                    if kind == b"x":
                        if local_pax:
                            raise ValueError("repeated local PAX")
                        local_pax = self.pax(raw)
                    elif kind == b"g":
                        for key, value in self.pax(raw).items():
                            if key in ("path", "linkpath", "size") or key in global_pax:
                                raise ValueError("ambiguous global PAX")
                            global_pax[key] = value
                    elif kind == b"L":
                        if long_name is not None:
                            raise ValueError("repeated long name")
                        long_name = self.text(raw)
                    else:
                        if long_link is not None:
                            raise ValueError("repeated long link")
                        long_link = self.text(raw)
                else:
                    attrs = dict(global_pax)
                    attrs.update(local_pax)
                    if long_name is not None and "path" in attrs or long_link is not None and "linkpath" in attrs:
                        raise ValueError("ambiguous extended names")
                    name = self.path(attrs.get("path", long_name if long_name is not None else name), kind == b"5")
                    link = attrs.get("linkpath", long_link if long_link is not None else link)
                    if "size" in attrs:
                        if not attrs["size"].isdigit() or int(attrs["size"]) != size:
                            raise ValueError("PAX size disagreement")
                    local_pax = {}
                    long_name = long_link = None
                    folded = name.casefold()
                    if folded in seen:
                        raise ValueError("duplicate/case-aliased member")
                    seen.add(folded)
                    if kind not in (b"0", b"\x00", b"5", b"1", b"2") or kind not in (b"0", b"\x00") and size:
                        raise ValueError("unsupported member or nonfile payload")
                    self.payload += size
                    if self.payload > p["maxPayloadBytes"]:
                        raise ValueError("full payload extent exceeded")
                    digest = hashlib.sha256()
                    blob = hashlib.sha1(b"blob " + str(size).encode("ascii") + b"\x00")
                    remaining = size
                    while remaining:
                        block = self.exact(source, min(remaining, p["chunkBytes"]))
                        digest.update(block)
                        blob.update(block)
                        remaining -= len(block)
                    blob_id = blob.hexdigest() if kind in (b"0", b"\x00") else None
                    if kind == b"2":
                        link_raw = link.encode("utf-8", "strict")
                        blob_id = hashlib.sha1(b"blob " + str(len(link_raw)).encode("ascii") + b"\x00" + link_raw).hexdigest()
                    self.rows.append({"headerOffset": offset, "bodyOffset": offset + 512, "path": name,
                                      "kind": kind.hex(), "bytes": size, "sha256": digest.hexdigest(),
                                      "gitBlobSHA1": blob_id,
                                      "headerSHA256": hashlib.sha256(header).hexdigest(),
                                      "link": link, "mode": self.number(header[100:108]),
                                      "uid": self.number(header[108:116]), "gid": self.number(header[116:124]),
                                      "mtime": self.number(header[136:148]), "pax": attrs})
                padding = (-size) % 512
                if padding and any(self.exact(source, padding)):
                    raise ValueError("nonzero member padding")
        self.validate_graph()
        self.fresh_json("members.json", {"members": self.rows, "controlHeaders": self.controls,
                                          "payloadBytes": self.payload})

    def validate_graph(self):
        rows = {row["path"]: row for row in self.rows}
        if self.plan["rootName"] not in rows or rows[self.plan["rootName"]]["kind"] != b"5".hex():
            raise ValueError("missing root directory")
        for row in self.rows:
            self.tick()
            parts = row["path"].split("/")
            for index in range(1, len(parts)):
                parent = rows.get("/".join(parts[:index]))
                if parent is None or parent["kind"] != b"5".hex():
                    raise ValueError("missing directory or link ancestor")
            kind = bytes.fromhex(row["kind"])
            if kind == b"1":
                destination = self.path(row["link"])
                target = rows.get(destination)
                if target is None or bytes.fromhex(target["kind"]) not in (b"0", b"\x00"):
                    raise ValueError("hardlink not to original regular file")
                row["target"] = destination
                row["gitBlobSHA1"] = target["gitBlobSHA1"]
            elif kind == b"2":
                link = row["link"]
                if not link or any(c in link for c in "\\:\x00") or link.startswith("/"):
                    raise ValueError("unsafe symlink")
                components = parts[:-1]
                for part in link.split("/"):
                    if part == "..":
                        if len(components) <= 1:
                            raise ValueError("symlink escapes root")
                        components.pop()
                    elif part not in ("", "."):
                        components.append(part)
                target = self.path("/".join(components))
                if target not in rows or bytes.fromhex(rows[target]["kind"]) not in (b"0", b"\x00", b"5"):
                    raise ValueError("dangling/cyclic/indirect symlink")
                row["target"] = target
            elif row["link"]:
                raise ValueError("unexpected link metadata")

    def materialize(self):
        tree = self.output / "tree"
        os.mkdir(tree)
        with open(self.output / "original.tar", "rb") as source:
            for row in sorted(self.rows, key=lambda item: (item["path"].count("/"), item["path"])):
                self.tick()
                destination = tree.joinpath(*row["path"].split("/"))
                kind = bytes.fromhex(row["kind"])
                if kind == b"5":
                    os.mkdir(destination)
                elif kind in (b"0", b"\x00"):
                    source.seek(row["bodyOffset"])
                    digest = hashlib.sha256()
                    remaining = row["bytes"]
                    with open(destination, "xb") as target:
                        while remaining:
                            block = self.exact(source, min(remaining, self.plan["chunkBytes"]))
                            digest.update(block)
                            target.write(block)
                            self.tick()
                            remaining -= len(block)
                    if digest.hexdigest() != row["sha256"]:
                        raise ValueError("retained tar changed")
                    os.chmod(destination, row["mode"] & 0o777)
                    self.tick()
            # Links are created only after every original regular file and
            # directory; no subsequent payload write can traverse a link.
            for row in self.rows:
                self.tick()
                destination = tree.joinpath(*row["path"].split("/"))
                kind = bytes.fromhex(row["kind"])
                if kind == b"1":
                    os.link(tree.joinpath(*row["target"].split("/")), destination)
                elif kind == b"2":
                    target = next(item for item in self.rows if item["path"] == row["target"])
                    os.symlink(row["link"], destination, target_is_directory=target["kind"] == b"5".hex())
                self.tick()
        self.fresh_json("complete.json", {"state": "MATERIALIZED_NOT_SOURCE_OR_BUILD_ADMISSION",
                                         "memberCount": len(self.rows), "payloadBytes": self.payload})


def main():
    # Plan bytes and their parser/runtime are prerequisite review inputs, not
    # product source authority. The exact command pins the original plan hash.
    if len(sys.argv) != 4 or not sys.argv[2].isdigit():
        raise ValueError("usage: materialize-full-source26.py PLAN BYTES SHA256")
    plan_bytes = int(sys.argv[2])
    if not 0 < plan_bytes < sys.maxsize:
        raise ValueError("finite original plan geometry")
    with open(sys.argv[1], "rb") as original:
        raw = original.read(plan_bytes + 1)
    if len(raw) != plan_bytes or hashlib.sha256(raw).hexdigest() != sys.argv[3]:
        raise ValueError("plan pin differs")
    def unique(pairs):
        result = {}
        for key, value in pairs:
            if key in result:
                raise ValueError("duplicate plan field")
            result[key] = value
        return result
    plan = json.loads(raw.decode("utf-8", "strict"), object_pairs_hook=unique)
    work = Materialization(plan)
    try:
        with open(work.output / "plan-original.json", "xb") as retained:
            work.artifact_bytes += len(raw)
            if work.artifact_bytes > work.plan["maxArtifactBytes"]:
                raise ValueError("artifact extent exceeded")
            retained.write(raw)
        work.decompress()
        work.inventory()
        work.materialize()
    except BaseException:
        # No deletion, cleanup, reuse or success receipt on partial completion.
        # The independent native command owner retains actual failure/streams.
        raise


if __name__ == "__main__":
    main()
