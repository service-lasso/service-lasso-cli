"""UNRUN: exact Node release key armor -> full binary packet keyring.

Structural fingerprinting is not self-signature, subkey or revocation validation.
Every original packet remains in the candidate keyring. Original native owner
retains command/capture/failure custody beyond this script's local work limit.
"""
import base64
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
            raise ValueError("duplicate plan field")
        result[key] = value
    return result


def pinned(path, size, digest):
    if not Path(path).is_absolute() or type(size) is not int or not 0 < size < sys.maxsize:
        raise ValueError("exact bounded original")
    with open(path, "rb") as source:
        raw = source.read(size + 1)
    if len(raw) != size or hashlib.sha256(raw).hexdigest() != digest:
        raise ValueError("original byte pin differs")
    return raw


def main():
    if len(sys.argv) != 4 or not sys.argv[2].isdigit():
        raise ValueError("usage: prepare-node-keyring26.py PLAN BYTES SHA256")
    original = pinned(sys.argv[1], int(sys.argv[2]), sys.argv[3])
    plan = json.loads(original.decode("utf-8", "strict"), object_pairs_hook=unique)
    fields = {"schema", "key", "keyBytes", "keySHA256", "outputParent", "outputLeaf",
              "maxPackets", "maxPacketBytes", "maxReportBytes", "milliseconds"}
    if set(plan) != fields or plan["schema"] != "cli39.node-keyring-preparation.v1":
        raise ValueError("exact plan schema")
    for key in ("keyBytes", "maxPackets", "maxPacketBytes", "maxReportBytes", "milliseconds"):
        if type(plan[key]) is not int or not 0 < plan[key] <= sys.maxsize:
            raise ValueError("finite original geometry")
    until = time.monotonic_ns() + plan["milliseconds"] * 1000000

    def tick():
        if time.monotonic_ns() >= until:
            raise TimeoutError("local work limit; original native owner retains custody")

    raw = pinned(plan["key"], plan["keyBytes"], plan["keySHA256"])
    tick()
    # Accept only the selected complete armor grammar. No ignored preamble,
    # appended key, unknown armor metadata, whitespace or permissive base64.
    text = raw.decode("ascii", "strict").replace("\r\n", "\n")
    if "\r" in text:
        raise ValueError("ambiguous armor line endings")
    lines = text.split("\n")
    if lines[-1] == "":
        lines.pop()
    if (len(lines) < 5 or lines[0] != "-----BEGIN PGP PUBLIC KEY BLOCK-----" or
            lines[-1] != "-----END PGP PUBLIC KEY BLOCK-----" or
            len(lines[-2]) != 5 or not lines[-2].startswith("=")):
        raise ValueError("complete selected armor framing")
    boundary = 1
    while boundary < len(lines) and lines[boundary]:
        tick()
        if not lines[boundary].startswith(("Comment: ", "Version: ")):
            raise ValueError("unknown armor metadata")
        boundary += 1
    if boundary >= len(lines) - 3:
        raise ValueError("missing armor metadata boundary")
    armor_headers = lines[1:boundary]
    encoded = lines[boundary + 1:-2]
    if any(not line or len(line) > 76 for line in encoded):
        raise ValueError("finite armor line grammar")
    binary = base64.b64decode("".join(encoded), validate=True)
    check = base64.b64decode(lines[-2][1:], validate=True)
    if len(check) != 3 or not binary:
        raise ValueError("armor checksum geometry")
    crc = 0xB704CE
    for value in binary:
        tick()
        crc ^= value << 16
        for unused in range(8):
            crc <<= 1
            if crc & 0x1000000:
                crc ^= 0x1864CFB
    if (crc & 0xFFFFFF).to_bytes(3, "big") != check:
        raise ValueError("armor CRC24")
    packets = []
    keys = []
    cursor = 0
    while cursor < len(binary):
        tick()
        start = cursor
        header = binary[cursor]
        cursor += 1
        if not header & 128:
            raise ValueError("packet tag marker")
        if header & 64:
            tag = header & 63
            if cursor >= len(binary):
                raise ValueError("packet length missing")
            first = binary[cursor]
            cursor += 1
            if first < 192:
                length = first
            elif first < 224:
                if cursor >= len(binary):
                    raise ValueError("packet length truncated")
                length = ((first - 192) << 8) + binary[cursor] + 192
                cursor += 1
            elif first == 255:
                if cursor + 4 > len(binary):
                    raise ValueError("packet length truncated")
                length = int.from_bytes(binary[cursor:cursor + 4], "big")
                cursor += 4
            else:
                raise ValueError("partial key packet unsupported; whole input refused")
        else:
            tag = (header >> 2) & 15
            form = header & 3
            if form == 3:
                raise ValueError("indeterminate key packet unsupported")
            width = (1, 2, 4)[form]
            if cursor + width > len(binary):
                raise ValueError("packet length truncated")
            length = int.from_bytes(binary[cursor:cursor + width], "big")
            cursor += width
        if tag not in (2, 6, 13, 14, 17) or not 0 < length <= plan["maxPacketBytes"] or cursor + length > len(binary):
            raise ValueError("unsupported or truncated complete public key packet")
        body = binary[cursor:cursor + length]
        cursor += length
        packets.append({"offset": start, "tag": tag, "bodyBytes": length,
                        "sha256": hashlib.sha256(binary[start:cursor]).hexdigest()})
        if len(packets) > plan["maxPackets"]:
            raise ValueError("packet count bound")
        if tag in (6, 14):
            if body[0] != 4 or length > 65535:
                raise ValueError("unsupported complete key fingerprint version")
            fingerprint = hashlib.sha1(b"\x99" + length.to_bytes(2, "big") + body).hexdigest().upper()
            keys.append({"tag": tag, "fingerprint": fingerprint})
    primary = [key for key in keys if key["tag"] == 6]
    if (packets[0]["tag"] != 6 or len(primary) != 1 or
            primary[0]["fingerprint"] != "CC68F5A3106FF448322E48ED27F5E38D5B0A215F"):
        raise ValueError("independently selected Node primary fingerprint mismatch")
    parent = Path(plan["outputParent"])
    leaf = plan["outputLeaf"]
    if (not isinstance(leaf, str) or not leaf or leaf in (".", "..") or
            any(c in leaf for c in "/\\:\x00") or not parent.is_absolute() or parent.resolve(strict=True) != parent):
        raise ValueError("canonical fresh output")
    report = json.dumps({"state": "CANDIDATE_KEYRING_STRUCTURAL_ONLY", "armorHeaders": armor_headers, "keys": keys,
                         "packets": packets, "selfSignaturesVerified": False,
                         "subkeyPolicyVerified": False, "revocationVerified": False},
                        ensure_ascii=True, separators=(",", ":")).encode("ascii")
    if len(report) + len(original) > plan["maxReportBytes"]:
        raise ValueError("retained report bound")
    tick()
    output = parent / leaf
    os.mkdir(output)
    for name, contents in (("plan-original.json", original), ("candidate-keyring.gpg", binary),
                           ("packets.json", report)):
        tick()
        with open(output / name, "xb") as target:
            target.write(contents)
        tick()


if __name__ == "__main__":
    main()
