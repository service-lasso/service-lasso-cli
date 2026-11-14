# Explicit saved-connection membership — CLI #45

Active corrective slice of CLI-CONNECTION. A selected/default name must be an own member of the saved connection dictionary. JavaScript inherited names do not select a Core endpoint. Names such as `constructor` remain valid when explicitly saved. Explicit flag/environment URL precedence remains unchanged.

Acceptance: load and save reject missing default names with invalid_config; connection resolution rejects unsaved constructor through flag/environment/default with unknown_connection, including when a fallback Core URL exists. Explicit constructor profiles roundtrip and resolve normally. Preserve existing errors and precedence.

Implementation: Object.hasOwn checks at the existing three membership boundaries, with absent dictionaries rejected. Rejecting constructor by name was considered and rejected because it changes the public name grammar.

Proof: distinct source review precedes execution. Compile only exact copied src/config.ts and src/errors.ts in private scratch, then execute only copied tests/config.test.js; verify source hashes match. No full product scripts/build/native tests or CLI39 execution admission is supplied. Full native/publication qualification remains outside this issue.

## Focused proof (2026-11-14)

Distinct reviewer SOURCE_GO covered config SHA256 7c344385e83bbdb81ec893f25582eb9f9226946297d754ef7eec3cc00c7c8d98, errors dd707712fa5dafdca01f99ad4a9d0f35c05514893714b12f80c05c1789344e80 and tests 30a84f51ab298f4bd19495b6d316cda6d4e3ada4fd7751b8b8ac2f9b60d57cc2; coordinator admitted only isolated compile/config tests. Exact copies in /tmp/cli45-proof/{baseline,fixed}. Both compile PASS with strict ES2022 NodeNext and read-only Node typings; baseline tests fail the two added regressions, fixed tests PASS8/8. Logs compile-isolated.log/test.log retained per lane. Runtime Node24.21.0 and installed TypeScript7.0.2 (lib/tsc.js SHA256 cbdfbf11c26ed00dfc073155e316a42d6d6d8a387be61006b82fa9aa93ac572e); this is focused proof, not the repository Node22/full-build qualification. Compiler path /tmp/tui37-gitdir-011df21/five-actions/core-source/node_modules/typescript/bin/tsc; typeRoots its sibling @types. Initial incomplete Cursor compiler missing-lib and subsequent cwd/config diagnostic are retained in compile.log/compile-full-installed.log. Final invocation ran in isolated lane cwd. No native/full product command ran.
