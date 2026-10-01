import { resolve } from "node:path";
import { verifyCandidateDirectory } from "./protected-candidate-lib.mjs";

function option(name) { const index = process.argv.indexOf(name); if (index < 0 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`); return process.argv[index + 1]; }
const directory = resolve(option("--directory"));
const { manifest } = await verifyCandidateDirectory(directory, option("--version"), option("--source-sha"));
process.stdout.write(`${JSON.stringify({ candidateTag: manifest.candidateTag, assetCount: manifest.assets.length })}\n`);
