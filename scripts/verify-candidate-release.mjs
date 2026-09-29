import { createHash } from "node:crypto";
import { readFile } from "node:fs/promises";
import { join, resolve } from "node:path";

function option(name) {
  const index = process.argv.indexOf(name);
  if (index === -1 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`);
  return resolve(process.argv[index + 1]);
}
function sha256(value) {
  return createHash("sha256").update(value).digest("hex");
}

const expected = option("--expected");
const actual = option("--actual");
const expectedRecord = await readFile(join(expected, "candidate.json"));
const actualRecord = await readFile(join(actual, "candidate.json"));
if (!expectedRecord.equals(actualRecord)) throw new Error("Released candidate.json does not match the tested candidate.");
const record = JSON.parse(expectedRecord.toString("utf8"));
if (!Array.isArray(record.assets) || record.assets.length !== 1) throw new Error("Candidate record must contain exactly one archive.");
const asset = record.assets[0];
if (typeof asset?.name !== "string" || !/^[a-z0-9][a-z0-9.-]*\.tgz$/i.test(asset.name) || typeof asset.sha256 !== "string") {
  throw new Error("Candidate record contains an unsafe or incomplete archive identity.");
}
const expectedArchive = await readFile(join(expected, asset.name));
const actualArchive = await readFile(join(actual, asset.name));
if (!expectedArchive.equals(actualArchive) || sha256(actualArchive) !== asset.sha256) {
  throw new Error("Released archive does not match the tested candidate checksum.");
}
const expectedSums = await readFile(join(expected, "SHA256SUMS.txt"));
const actualSums = await readFile(join(actual, "SHA256SUMS.txt"));
if (!expectedSums.equals(actualSums)) throw new Error("Released SHA256SUMS.txt does not match the tested candidate.");
if (!actualSums.toString("utf8").includes(`${asset.sha256}  ${asset.name}\n`)) throw new Error("Released checksum manifest does not bind the candidate archive.");
