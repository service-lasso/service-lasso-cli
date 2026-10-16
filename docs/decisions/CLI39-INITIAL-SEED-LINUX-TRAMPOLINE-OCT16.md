# CLI39 Linux A native trampoline — source implementation selected, UNRUN

Fresh different intent review of exact2c5f03b710e62bb2e678a2c2323d6915153c89a7
selects this private initial seed-loading mechanism for source implementation
only. It is not implemented, ABI-qualified, source GO
or execution admission. Its purpose is the complete Linux A controller → actual
compiler/addon output → actual Node/addon load → child-local B/full runtime build
route, not another constructor prefix. Windows A's separate emitter approval does
not approve this mechanism. Independent initial admission is external; the fixed
native program/controller/profile/source graph and Linux recipe are OUR work.

## Initial engine and architecture

Select an independently admitted Linux-x64 PowerShell/CLR initial controller with
its exact ELF/interpreter/libc/CLR/JIT/loaded/source/principal/stack/TLS/FD/case/
physical profile before its first statement. Fixed managed P/Invoke can own parent
observations and backing using those already admitted runtime primitives. It may
not call clone/fork and then resume CLR/interop in the child. There is no Add-Type,
csc, gcc/as compilation, CLI addon or nonexistent Core factory before first birth.

The source-owned small native program instead contains the actual clone and both
native return branches. Its parent branch returns to the admitted controller;
its child branch executes direct Linux syscalls only and never returns into CLR.
Its bytes come only from reviewed repository instruction source and fixed encoding
templates, never downloaded machine code, a caller byte buffer or arbitrary IL/
syscall callback. The complete native block, all fixups and child helpers are one
closed source unit. No input can add instructions, call targets or operation IDs.

This selection is Linux x86_64/SysV only, matching the selected Linux-x64 product
lane. Actual admitted engine/native image ELF machine, process architecture and
raw uname/auxiliary-vector observations must agree before code becomes executable.
No environment switch, host-name guess, ARM fallback or cross-build result selects
the ABI. Existing aarch64 source remains retained; this proposal does not qualify
it. Exact original Linux-x64 SDK headers supply syscall and kernel structure ABI;
number/layout correspondence is checked before emission, never silently defaulted.

## Source, entry ABI and concrete instruction map

Planned source sites are native/build-owner/seed_linux_x64.S (normative fixed
instruction source), a closed fixed encoder/label map owned by the initial
controller, and scripts/build-initial-seed-linux.ps1 (complete A owner/recipe).
None exists yet; this proposal does not count a future path as implementation.
No target assembler/parser/compiler/generator or emitted code has run.

The single callable entry has SysV signature int64 entry(native_frame *), RDI
pointing to the SAME preowned private native frame, 16-byte call-stack alignment
and full preservation of RBX/RBP/R12–R15. That frame and every referenced argv,
envp, source span, clone_args and native output row are native memory, not movable
managed objects. Pointer operands do not transport source/ROOT/finance authority.
The initial genuine owner precedes the entry. Parent/child raw return records,
clocks, failures and counters occupy disjoint preowned locations; neither branch
writes the other's clock/cursor. Immutable origin/deadline is common, not renewed.

The internal clone_core block has this fixed x64 encoding/layout. It is called
from the native entry after the preclock and signal blocking; only its parent
return goes back to the native parent-finish block, not straight into CLR:

| Offset | Instruction/source purpose | Encoding/next offset |
| --- | --- | --- |
| 0 | endbr64, valid fixed indirect-entry marker | f3 0f 1e fa / 4 |
| 4 | push r12, preserve parent's nonvolatile register | 41 54 / 6 |
| 6 | mov r12,rdi, retain native frame in both branches | 49 89 fc / 9 |
| 9 | mov rdi,[r12], exact clone_args pointer slot0 | 49 8b 3c 24 / 13 |
| 13 | mov esi,88, selected complete clone_args extent | be 58 00 00 00 / 18 |
| 18 | mov eax,__NR_clone3, selected x64 ABI435 | b8 b3 01 00 00 / 23 |
| 23 | syscall, clobbers RAX/RCX/R11, not source frame | 0f 05 / 25 |
| 25 | test rax,rax, zero is the actual child return | 48 85 c0 / 28 |
| 28 | jz child_start at42 | 0f 84 08 00 00 00 / 34 |
| 34 | mov [r12+8],rax, parent raw result before postcut | 49 89 44 24 08 / 39 |
| 39 | pop r12, parent-only restore | 41 5c / 41 |
| 41 | ret, parent-only to native finish/mask restore | c3 / 42 |
| 42 | child_start, native-only fixed continuation | remaining closed child body |

The local clone_core frame offsets0/8 are pointer/raw slots, not the whole frame
schema or an authority type. Whole native entry also has its own preserved-register
and stack contract. Its parent finish retains clone result/pidfd, restores the
original thread signal mask, records postclock and only then returns that saved
raw result. A failed/late restore does not erase a born child or allow retry.
Actual SDK clone_args size/field geometry must match this selected88-byte v2
operand; a missing/different ABI refuses rather than reading another layout.

All other blocks are fixed source labels: entry, clock_row, block_parent_signals,
clone_core, parent_finish, child_start, child_stop, child_fd_observations,
child_stdio, child_signal_dispositions, child_restore_mask, child_exec,
child_failure. Native helper calls and loops stay inside this one bounded block.
Every conditional jump uses its fixed x64 rel32 opcode and six-byte extent;
unconditional/call fixups are E9/E8 plus signed rel32, five bytes. Each displacement
is target_offset - (instruction_offset + instruction_bytes), in signed32 range,
with exact little-endian readback. Targets must be declared instruction boundaries
in the SAME emitted source map, never data, an external address or a supplied
pointer. All complete block offsets/lengths/fixups/source associations enter the
implementation's fixed manifest and independent byte/vector review BEFORE load.
The42-byte internal map is concrete; it is not a claim the remaining program or
whole manifest is already implemented/reviewed. No partial map is loadable.

The direct syscall helper implements the existing native_syscall_linux.h x64
register relation: RAX number, RDI/RSI/RDX/R10/R8/R9 six arguments, RCX/R11 clobber,
signed raw return retained. Each call site has a source-fixed number, not a number
from controller input. Original SDK constraints include clone3=435, ptrace=101,
getpid=39, gettid=186, tgkill=234, execveat=322, exit_group=231, clock_gettime=228,
rt_sigprocmask=14 and rt_sigaction=13. These literals must correspond to the held
actual SDK before emission; listed values alone are not source/ABI authority.
Native CLOCK_MONOTONIC timespec and before/after/raw/error handling follow the
actual source-owned Linux clock semantics, without calling libc/CLR in the child.

## Backing, W→X and uncertain effects

The initial controller owns complete finite native/managed/JIT/metadata/argument/
code/frame/event/stream/FD/profile backing before first effect. Actual auxiliary
AT_PAGESZ and checked rounding select mmap size; no4096 guess is a grant. A
dedicated mapping is first RW, receives only the fixed whole program, and gets
complete independent byte/geometry/source-map readback before actual mprotect RX.
No RWX mapping or remaining writable mapping alias is selected. Retained source
bytes are a separate source object, never permission to modify executable pages.
Policy/execmem/mmap/mprotect failures deny; no privilege/sandbox/loader bypass.

Every allocation/protection/call/release attempt has one-use raw operand/result/
clock storage before its postcut. A returned mapping at address0 stays actually
owned/charged but unusable; failed/late output does not become an absent mapping.
Unknown mprotect/call completion keeps the original pages and all aliases.
No delegate finalizer, managed GC, parent helper return or successful exec of one
child retires code. Actual all-child exec/unmap/terminal and last-borrower cuts
precede explicit native release; copied child code mappings have their own lifetime.

## Clone, stack, TLS, signal and FD obligations

clone_args selects CLONE_PIDFD and exit_signal SIGCHLD, no CLONE_VM/CLONE_THREAD/
CLONE_VFORK/new stack/new TLS. Parent raw positive PID and actual pidfd are retained;
negative errno is separate. pidfd storage is a parent-side clone output, never an
inherited child descriptor. The child owns its actual zero return and fresh native
getpid/gettid/FD observations. It cannot infer its table from the parent's numeric
pidfd/FD flags. Existing slot3 distinctions and full descriptor/custody semantics
must remain explicit; no reuse of old10-row/3N+14 costs for this new larger program.

The child's initial native call stack/TLS/address space is the clone's copy of the
admitted controller, not a free new arena. Native code never dereferences managed
objects, invokes CLR/PInvoke stubs or returns to the managed call frame. Only the
fixed native stack/frame/source operands are used. Complete parent/child page-table,
copy-on-write, runtime/stack/TLS/FD and simultaneous pending costs must be derived
and physically backed, including own parent GC/JIT/other-thread mutations. A raw
mmap byte count or an apparent fork success cannot qualify that whole profile.
Missing finite whole derivation keeps invocation refused; this is implementation
work, not permission to assume those costs vanish.

Inheriting CLR signal handlers is another actual child hazard. Before clone, the
native entry retains the original issuing-thread mask and blocks all blockable
signals with actual rt_sigprocmask. The parent restores its exact mask on return.
The child keeps signals blocked through early stop and native setup. Before exec,
it actually queries each permitted signal's kernel sigaction, resets caught
handlers to SIG_DFL and preserves SIG_IGN/default, matching exec semantics; SIGKILL/
SIGSTOP are unchangeable, not failed setters. It then restores the original mask
only after no CLR handler can run. All query/set/mask rows and failure paths are
preowned/budgeted. There is no managed signal callback in the child. Unblockable
signals or native uncertainty remain actual failure/custody events, never success.

FD setup consumes actual held executable/source/pipe/custody occurrences and the
complete initial descriptor/exclusion roster before clone. The child repeats its
own observations; stdio redirection, close-on-exec flags, exact selected allowlist
and every required close/dup retain one-use raw outcomes. It never closes an
unknown foreign reference based on a guessed number. Parent stdin writer closes
once for the fixed empty schedule; stdout/stderr natural EOF waits for ALL actual
descendant writers. No parent retained writer or zero availability is called EOF.

## Native-only child path and complete controller continuation

Child start records its actual clone return and raw own identity, ptrace TRACEME
and a real SIGSTOP using its own native PID/TID. The actual parent observes that
owned stop, joins source/code/native stack/principal/FD/map/deadline/profile and
sets the fixed ptrace options before continuation. Source-selected namespace and
own-writer exclusion, not hashes/flags alone, remain necessary. After actual
child-local descriptor/signal setup it invokes only execveat on the actual held
ELF executable, empty path, exact native argv/envp and AT_EMPTY_PATH. A returned
exec is always failure: raw errno/prefix survives a failed postclock, then native
exit_group(127) is attempted once. A returned exit records failed/unknown and traps;
it proves no termination or EOF. Parent retains the real stopped/pending graph.

Parent owns finite ptrace/waitid/pidfd/proc/maps/ELF/interpreter/libc/source events,
every actual fork/vfork/clone/exec/exit descendant, separate native pipe streams,
config/source/output readback, one-use closes and physical last borrowers. No
automatic detach/kill, synthetic event, timeout-as-exit or untracked descendant
supplies compiler completion. All wait/idle/event/failure attempts use the SAME
original deadline/profile and preserve first private cause with nonreturning
pending retention as needed. No output function or script return escapes custody.

The Linux fixed native source build recipe must implement actual full Node22.23.2,
full ICU/default V8/APIs/required allocator hooks, genuine selected GCC/binutils/
Python/NASM/make, libc/startup/interpreter and complete source/config/link closure.
The Windows CRT recipe is not this recipe. Generation → actual complete native
configuration validation → compilation → independent unchanged config/output
joins remain fixed. Compiler/root exit alone cannot advance the schedule.

The initial compiler route consumes existing initial-installer Linux native source
semantics for clone/owned stops, slot3, held ELF, maps and custody as references;
those compiled C functions are not mysteriously available before A exists and
their product transport/profile is not copied into a build-plane issuer.
Source/byte/ABI mapping of the new native program must be independently reviewed.

Actual produced addon load into the exact selected Node and genuine child-local
B source/principal/physical/deadline continuation must be implemented, not replaced
with a parent pointer, JSON budget or copied native record. That actual build-plane
entry remains distinct from product installer/TUI launch. Code/source correspondence
and successful raw syscalls never mint the initial authority. A complete runnable
A/B/fixed runtime route, fresh ENTIRE review and NEW exact input/ROOT/commands are
still required. producerComplete remains false; every local target action is UNRUN.

Exact initial CLR child-reaper dependency (source audit, not execution)

The selected .NET10.0.12 [native signal implementation](https://raw.githubusercontent.com/dotnet/runtime/v10.0.12/src/native/libs/System.Native/pal_signal.c)
selects reap-all when its recorded process PID is 1 or its original SIGCHLD action
was SIG_IGN. Its signal thread may then consume waitpid(-1, WNOHANG) results even
without a managed child callback. [Process.Unix](https://raw.githubusercontent.com/dotnet/runtime/v10.0.12/src/libraries/System.Diagnostics.Process/src/System/Diagnostics/Process.Unix.cs)
passes that native flag to the managed child checker. [ProcessWaitState.Unix](https://raw.githubusercontent.com/dotnet/runtime/v10.0.12/src/libraries/System.Diagnostics.Process/src/System/Diagnostics/ProcessWaitState.Unix.cs)
normally reaps only registered managed child PIDs; reap-all additionally consumes
unknown children. Thus avoiding Process.Start/managed registration is necessary
but insufficient to establish sole native wait custody.

The independently admitted initial occurrence must join the actual namespace PID,
pre-CLR SIGCHLD disposition and engine-creation custody to this exact loaded runtime
source. A later rt_sigaction observation cannot recover the runtime's retained
original action. No input boolean, private-global patch, reaper disablement, signal
handler replacement or extra host permission supplies that history. If the selected
creation wrapper fixed and observed it before engine exec, consume that SAME
original. Actual construction and exclusion of other wait actors remain required
implementation/profile work; the current proposal has not established them.
