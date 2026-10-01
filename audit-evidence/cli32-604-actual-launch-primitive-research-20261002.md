# CLI PR #32 — actual helper-launch primitive assessment

**Scope:** Issue #8 Development source review at
`604161edb970e6b0ba23963ecce73e7bfbd0df5e`. This is an architecture research
record. It is not a replacement for the existing independent review, and it
does not claim a release, a candidate, three-host execution, or acceptance.

## Question

Can the existing SEA (`src/scaffold.ts`) prove that the helper bytes it hashes
are the image the kernel starts when a same-identity process can replace files
in the staging area?

## Result

No. The current sequence writes verified bytes to `heldHelper`, re-reads and
hashes the pathname, returns that pathname, then gives the pathname to
`child_process.spawn`. The final operation re-resolves the name. A random
directory, mode `0700`, an advisory lock, a before/after hash, a helper receipt,
or a protected owner DACL do not bind the image opened by that final operation
against an actor with the same effective identity.

The Windows project-object DACL is a separate control. Its protected
`OWNER RIGHTS` DACL is appropriate for newly materialized project objects, but
the owner can still change discretionary ACLs. It cannot authenticate an
executable image across a later path open by a same-owner process.

## Platform primitives evaluated

| Host | Kernel-bound primitive | Why the current SEA cannot use it |
| --- | --- | --- |
| Linux | `fexecve`/`execveat(AT_EMPTY_PATH)` on an already verified descriptor | Node's child-process API accepts a command pathname; it cannot request descriptor execution. |
| macOS | a descriptor-preserving `/dev/fd/N` launch can be experimentally assessed, but supported `posix_spawn` is path-based | Node does not provide an explicit descriptor-exec contract or a portable way to assert the kernel used the verified vnode. |
| Windows | a staged file handle opened with no write/delete sharing can prevent later replacement while a process creator opens the same image; a section-backed native process path is the stronger identity binding | Node's filesystem and child-process APIs expose neither the required share mode nor handle/section-backed process creation. A DACL alone does not protect against the owner. |

Linux `fexecve(3)` documents this exact checksum-then-execute use case and
explicitly distinguishes descriptor execution from reopening a pathname. The
macOS `posix_spawn(2)` interface accepts a path. The supported Win32
`CreateProcess` interface also receives an image path; a safe staging design
needs native control of the preceding held file handle and its sharing policy.

## Required next architecture

The source must introduce a compiled, candidate-baked native launch gate as a
first-class part of the native artifact, described in the active specification
before implementation. It must be the trusted parent of the SEA and retain the
verified helper identity until child creation:

1. Embed or otherwise bind the helper bytes and digest to the gate's own
   immutable candidate image; mutable provenance remains evidence only.
2. On Linux, execute the verified anonymous/held image by descriptor.
3. On macOS, use a host-specific native mechanism with a test that proves the
   executed object remains the verified object; do not claim `/dev/fd` behavior
   without a target-host result.
4. On Windows, create and hold a private staging file through image creation
   with no write/delete sharing, verify the protected staging DACL, and fail
   closed if the native process creation contract cannot retain that binding.
5. Keep the native gate alive while the SEA asks it to launch the helper, or
   move the confined-writer protocol into the gate. A child SEA alone cannot
   reacquire this authority safely by path.

The current archive's three standalone executable targets, baked source and
candidate digests, source-only empty admissions, no-fallback rule, POSIX
writer semantics, and Windows final project-object DACL requirements remain
constraints on that design. An owner-published template tuple is still an
external end-to-end materialization blocker; it is not permission to add a
test-only normal admission or to weaken production selection.

## Conclusion

Do not merge the current head for the claimed actual held-executable property.
No source edit in this research record resolves P1. The next implementation
must be reviewed as a governed architecture change with all three native host
tests, including staged leaf/parent/sidecar mutation attempts and hostile
Windows parent-DACL coverage.
