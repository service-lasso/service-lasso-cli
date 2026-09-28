# Service Lasso CLI instructions

## Governance

This repository uses `.governance/` as its source of truth. Read the rules in
numeric order before changing product code.

## Branch boundary

Development work uses `develop` and issue-scoped branches only. Do not use
`main` as a development input. Release promotion is a separate authorised
activity.

## Delivery

- Keep an active specification and issue trail for product changes.
- Run focused tests, type checks and the applicable CI workflow before claiming
  a change is qualified.
- Do not publish packages, create releases, or deploy without explicit
  authorisation.
