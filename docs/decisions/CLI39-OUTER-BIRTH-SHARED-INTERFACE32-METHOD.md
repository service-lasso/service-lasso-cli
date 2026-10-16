# Fixed CLI pending birth interface proposal32

For parent/Core owner source disposition under selected outer coordinator
method C7D00580. No implementation or native/source/execution approval follows.

Core119 inspected existing shared producer originals: preparation.nativeState,
actual process/thread HANDLE and PID from CreateProcess output, retained native
call, assigned/resumed flags and calls, control SafePipeHandle, actual server
PID and live process waits before/after peer lookup. The source has no actual
process creation-time/token/image observation at this pending point. It must
not infer those facts from PID equality or a token challenge.

Propose a closed source branch only for cli39.source-input-host.v1, shared by
outer -> retained-caller and retained-caller -> operation-host. It calls literal
CLI functions, never a supplied scriptblock, callback or action name:

1. `Prepare-Cli39OwnedHostBirth32(PreparationOriginal, NativeOwnerOriginal)`
   before process/native acquisition. It enrolls a fixed pending observation
   book on SAME native owner before allocating contexts, results, worker stacks,
   pins and any partial constructor. Process handle/PID cells are initially
   empty; they are populated only from SAME actual successful CreateProcess
   output, never root JSON or caller input. All worker recipes/rosters are
   preowned and charged to SAME complete host-operation reservation.
2. `Advance-Cli39OwnedHostBirth32(PreparationOriginal, NativeOwnerOriginal)`
   after actual control-peer PID plus live-before/live-after checks, before
   setting peerChecked and allowing NPH. It advances at most the fixed current
   worker/Join0 observation, returning pending while the shared owner continues
   servicing pipes/control/deadline. It never blocks the shared capture loop,
   renews the deadline, retries an uncertain call or starts a generic effect.
3. `Test-Cli39OwnedHostBirthClosure32(PreparationOriginal, NativeOwnerOriginal)`
   at process-handle close eligibility and shared receiver closure. An issued
   worker may borrow the existing process handle; no cleanup closes it beneath
   unknown/start-pending/native-pending work. No-start terminal construction
   failure retains managed partials but need not invent a native borrower.

The actual fixed observation graph uses the SAME resident CLI own13 kernel32
query declarations for GetProcessTimes and QueryFullProcessImageNameW, with
the native owner's actual process HANDLE. Expected image bytes/length derive
from its fixed admitted executable original, not a guessed path maximum. It
retains native-return markers before independent diagnostics and all raw
FILETIMEs/image codeunits/count/status/error/pin/join originals.

Actual child principal observation needs a real queried child token, rather
than current-process pseudo token -4. Extend the existing fixed held Advapi32
principal module with literal OpenProcessToken(HANDLE,DWORD,HANDLE*) BOOL,
TOKEN_QUERY=8; keep GetTokenInformation literal. Preown token-open, TokenUser,
TokenStatistics and independent token-close workers before birth. Query
buffers retain the same SDK-derived user/statistics geometry and raw aliases.
Only the newly acquired actual token is eligible for independent once-close;
the borrowed process handle stays with its existing native owner. Known raw
token acquisition survives a later diagnostic failure; unknown acquisition or
borrower prevents fictional close. This is an explicit two-export module
extension, not kernel32 own13 mutation or new loader/owner authority.

The query graph does not by itself make admission true. Actual peer/native
process/birth/image/principal observations bind the existing owner's child
and admitted fixed command and are retained through root/tree/EOF/terminal
control. Child separately observes its own birth, and source-owned derived
root contains actual parent facts and static foundation lineage. Expected
publisher/OS/store/tool/source/loaded/namespace/profile facts remain genuine
inputs/local joins. No future child PID/birth is serialized before creation.

Core owns the shared closed branch/close/receiver cuts; CLI owns the fixed
functions, source import availability, principal module extension and complete
precharge/borrow/disposal predicates. Both must review exact final source and
profile geometry together. No shared edits should precede this bounded
interface selection.
