#!/usr/bin/env node
import { asCliError } from "./errors.js";
import { run } from "./cli.js";

run().catch((error) => {
  const safe = asCliError(error);
  process.stderr.write(`Error [${safe.code}]: ${safe.message}\n`);
  if (safe.hint) process.stderr.write(`Hint: ${safe.hint}\n`);
  process.exitCode = 1;
});
