# Matching-host ordinary CLI E2E — CLI #55

Active child of #1, extends CLI-REAL-CORE-LOCAL-READ / CLI-DISTRIBUTION ordinary
source and portable Node suites to Windows/macOS while preserving Linux.
Before importing shared setup, verify explicit exact clean Core revision. Import
only the selected Core scripts/operator-e2e-fixture-private.mjs, whose import has
no IO. createPrivateDirectory/verifyPrivateDirectory/verifyPrivateFile enforce
shared platform policy, without duplicated ACL rules. Root output must be private
before any raw evidence or credential writes; private child home/appdata/temp/cache
paths must be controlled. Full SID/DACL proof remains private; public records expose
platform/policy/verified/proofSHA256 only. On Windows forward exact npm-cli.js with
literal Node argv to the Core helper. No npm.cmd shell guessing.

Core1925 shared exports: async createPrivateDirectory(absentAbsoluteCanonicalPath),
verifyPrivateDirectory(path), verifyPrivateFile(path), writePrivateJsonFile(path,
value). The selected frozen merged revision will be bound before actual integration.
No dirty draft import or synthetic ACL result is authentic platform evidence.
Portable helper accepts shared directory creator; consumer/archive/staging inherit
verified root policy. Public machine output never copies raw SID/ACE details.

Acceptance: actual source+portable matching-host jobs terminal on Linux/Windows/
macOS; investigate failures. macOS evidence is exploratory/deferred ordinary source,
not GA/native restoration. Matching-host privacy/owned child cleanup/source/artifact
identity remains mandatory. Controlled adapter tests cover sequencing/mismatch and
redaction, never Windows ACL proof. Full native39/SEA/publication/permissions and
complete authoring/lifecycle gates remain separate and visible. Distinct review,
signed pushes, normal merge, postmerge proof and clean closure govern this issue.
