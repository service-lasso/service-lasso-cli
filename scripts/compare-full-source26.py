"""UNRUN prerequisite: retain complete release/Git structural differences.

Framed blob IDs are candidate associations, not authentication of Git metadata,
raw reference blobs, release transformations, tools or native source ancestry.
"""
import hashlib
import json
import os
from pathlib import Path
import sys
import time


def unique(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError("duplicate JSON field")
        result[key] = value
    return result


def pin_read(pin, retain_raw=False):
    if set(pin) != {"path", "bytes", "sha256"} or type(pin["bytes"]) is not int or not 0 < pin["bytes"] < sys.maxsize:
        raise ValueError("exact input pin")
    if not Path(pin["path"]).is_absolute():
        raise ValueError("absolute original input")
    with open(pin["path"], "rb") as source:
        raw = source.read(pin["bytes"] + 1)
    if len(raw) != pin["bytes"] or hashlib.sha256(raw).hexdigest() != pin["sha256"]:
        raise ValueError("original input differs")
    value = json.loads(raw.decode("utf-8", "strict"), object_pairs_hook=unique)
    return (value, raw) if retain_raw else value


def relative(value):
    if not isinstance(value, str) or not value or any(c in value for c in "\\\x00"):
        raise ValueError("original relative path")
    if any(part in ("", ".", "..") for part in value.split("/")):
        raise ValueError("original path geometry")
    return value


def main():
    if len(sys.argv) != 4 or not sys.argv[2].isdigit():
        raise ValueError("usage: compare-full-source26.py PLAN BYTES SHA256")
    plan, raw_plan = pin_read({"path": sys.argv[1], "bytes": int(sys.argv[2]), "sha256": sys.argv[3]}, True)
    if set(plan) != {"schema", "commit", "tree", "members", "rootName", "expectedEntries",
                     "referenceMembers", "referenceRootName", "maxReferenceEntries",
                     "outputParent", "outputLeaf", "milliseconds", "maxReportBytes", "maxReleaseEntries"}:
        raise ValueError("comparison plan keys")
    if plan["schema"] != "cli39.full-source-git-comparison.v2":
        raise ValueError("comparison plan schema")
    for key in ("expectedEntries", "milliseconds", "maxReportBytes", "maxReleaseEntries", "maxReferenceEntries"):
        if type(plan[key]) is not int or not 0 < plan[key] <= sys.maxsize:
            raise ValueError("finite original geometry")
    until = time.monotonic_ns() + plan["milliseconds"] * 1000000

    def tick():
        if time.monotonic_ns() >= until:
            raise TimeoutError("local comparison work limit; original native owner retains custody")

    root = relative(plan["rootName"])
    reference_root = relative(plan["referenceRootName"])
    if reference_root != "node-aa4c77582be995286fc6e00aaf530dc7ade102a9":
        raise ValueError("fixed commit reference root")
    if root != "node-v22.23.2" or any(c in plan["outputLeaf"] for c in "/\\:\x00") or plan["outputLeaf"] in ("", ".", ".."):
        raise ValueError("original root/output leaf")
    parent = Path(plan["outputParent"])
    if not parent.is_absolute() or parent.resolve(strict=True) != parent:
        raise ValueError("canonical original output parent")
    output = parent / plan["outputLeaf"]
    os.mkdir(output)
    tick()
    if len(raw_plan) > plan["maxReportBytes"]:
        raise ValueError("retained original plan extent exceeded")
    with open(output / "plan-original.json", "xb") as retained:
        retained.write(raw_plan)
    commit = pin_read(plan["commit"])
    tick()
    tree = pin_read(plan["tree"])
    tick()
    members = pin_read(plan["members"])
    tick()
    reference_members = pin_read(plan["referenceMembers"])
    tick()
    if (commit["sha"] != "aa4c77582be995286fc6e00aaf530dc7ade102a9" or
            commit["tree"]["sha"] != "cee5f56eb6b8b6b8e300419196dc90e8c7abd64b" or
            tree["sha"] != commit["tree"]["sha"] or tree["truncated"] is not False or
            not isinstance(tree["tree"], list) or len(tree["tree"]) != plan["expectedEntries"]):
        raise ValueError("complete original Git metadata association")
    originals = {}
    for row in tree["tree"]:
        tick()
        path = relative(row["path"])
        if path in originals or row["type"] not in ("blob", "tree", "commit"):
            raise ValueError("duplicate/invalid original tree entry")
        if ((row["type"] == "blob" and row["mode"] not in ("100644", "100755", "120000")) or
                (row["type"] == "tree" and row["mode"] != "040000") or
                (row["type"] == "commit" and row["mode"] != "160000")):
            raise ValueError("unsupported original Git mode")
        if row["type"] == "blob" and (type(row["size"]) is not int or row["size"] < 0):
            raise ValueError("original blob size")
        if len(row["sha"]) != 40 or any(c not in "0123456789abcdef" for c in row["sha"]):
            raise ValueError("original object ID")
        originals[path] = row
    def inventory(document, archive_root, maximum):
        if not isinstance(document["members"], list) or len(document["members"]) > maximum:
            raise ValueError("finite complete archive inventory geometry")
        result = {}
        by_full_path = {row["path"]: row for row in document["members"]}
        if len(by_full_path) != len(document["members"]):
            raise ValueError("duplicate archive member")
        if archive_root not in by_full_path or by_full_path[archive_root]["kind"] != b"5".hex():
            raise ValueError("missing complete archive root")
        for row in document["members"]:
            tick()
            path = relative(row["path"])
            if path == archive_root:
                continue
            if not path.startswith(archive_root + "/"):
                raise ValueError("archive member outside root")
            path = path[len(archive_root) + 1:]
            kind = bytes.fromhex(row["kind"])
            effective = row
            if kind == b"1":
                effective = by_full_path[row["target"]]
                if bytes.fromhex(effective["kind"]) not in (b"0", b"\x00"):
                    raise ValueError("unresolved original hardlink")
            if kind == b"5":
                value = {"mode": "040000", "type": "tree", "bytes": 0, "blob": None}
            elif kind == b"2":
                value = {"mode": "120000", "type": "blob", "bytes": len(row["link"].encode("utf-8")), "blob": row["gitBlobSHA1"]}
            elif kind in (b"0", b"\x00", b"1"):
                value = {"mode": "100755" if effective["mode"] & 0o111 else "100644",
                         "type": "blob", "bytes": effective["bytes"], "blob": effective["gitBlobSHA1"]}
            else:
                raise ValueError("unsupported archive member")
            result[path] = value
        return result

    release = inventory(members, root, plan["maxReleaseEntries"])
    reference = inventory(reference_members, reference_root, plan["maxReferenceEntries"])
    reference_differences = []
    reference_matched = 0
    for path in sorted(originals.keys() | reference.keys()):
        tick()
        original, actual = originals.get(path), reference.get(path)
        if original is None or actual is None:
            reference_differences.append({"path": path, "reason": "reference-or-Git-only", "original": original, "reference": actual})
        elif original["type"] != actual["type"] or original["mode"] != actual["mode"]:
            reference_differences.append({"path": path, "reason": "reference-type-or-mode", "original": original, "reference": actual})
        elif original["type"] == "commit" or (original["type"] == "blob" and
                (original["size"] != actual["bytes"] or original["sha"] != actual["blob"])):
            reference_differences.append({"path": path, "reason": "reference-blob-or-gitlink", "original": original, "reference": actual})
        else:
            reference_matched += 1
    archive_differences = []
    for path in sorted(release.keys() | reference.keys()):
        tick()
        if release.get(path) != reference.get(path):
            archive_differences.append({"path": path, "release": release.get(path), "reference": reference.get(path)})
    differences = []
    matched = 0
    for path in sorted(originals.keys() | release.keys()):
        tick()
        original, actual = originals.get(path), release.get(path)
        if original is None:
            differences.append({"path": path, "reason": "release-only", "release": actual})
        elif actual is None:
            differences.append({"path": path, "reason": "Git-only", "original": original})
        elif original["type"] == "commit":
            differences.append({"path": path, "reason": "gitlink-needs-complete-original-source", "original": original, "release": actual})
        elif original["type"] != actual["type"] or original["mode"] != actual["mode"]:
            differences.append({"path": path, "reason": "type-or-mode", "original": original, "release": actual})
        elif original["type"] == "blob" and (original["size"] != actual["bytes"] or original["sha"] != actual["blob"]):
            differences.append({"path": path, "reason": "blob-size-or-framed-ID", "original": original, "release": actual})
        else:
            matched += 1
    report = {"state": "STRUCTURAL_COMPARISON_NOT_AUTHENTICATION_OR_SOURCE_ADMISSION",
              "originalEntries": len(originals), "releaseEntries": len(release), "matched": matched,
              "differences": differences, "rawReferenceBlobsVerified": False,
              "referenceEntries": len(reference), "referenceMatched": reference_matched,
              "referenceDifferences": reference_differences, "archiveDifferences": archive_differences,
              "releaseTransformationsVerified": False, "localSignatureVerified": False}
    written = len(raw_plan)
    with open(output / "complete-comparison.json", "x", encoding="utf-8", newline="\n") as target:
        for block in json.JSONEncoder(ensure_ascii=True, separators=(",", ":")).iterencode(report):
            tick()
            written += len(block.encode("utf-8"))
            if written > plan["maxReportBytes"]:
                raise ValueError("retained report extent exceeded")
            target.write(block)
        tick()
    # No cleanup/retry, transformation allowlist or boolean proof fallback.


if __name__ == "__main__":
    main()
