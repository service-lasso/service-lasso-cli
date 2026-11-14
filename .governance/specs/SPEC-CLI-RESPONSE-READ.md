# Safe response-body transport failures — CLI #49

Active CLI-CONNECTION/CLI-AUTOMATION corrective slice, child of #1.
CoreClient must classify failure while reading the response body exactly like
fetch failure: core_unreachable, existing stable message/hint, no raw transport
error detail, and no retry. A fetched Response is not a complete successful
transport until its body is read. Keep HTTP status and JSON parsing classification
unchanged outside the transport catch.

Move only the response.text await into the existing fetch catch. Regression uses
an actual Response backed by a rejecting ReadableStream containing a secret
sentinel; assert safe code/message/hint and exactly one fetch call. Preserve all
existing valid JSON, non-JSON and status behavior. No timing/retry/API expansion.

Proof: distinct exact-source review, then explicit copied core-client/config/
errors compile with Node22.23.2/TypeScript5.9.3 and copied config/CoreClient tests.
Exact historical source plus new regression provides RED; exact candidate GREEN.
No native authoring/SEA/package/compiler admission or release authority changes.
