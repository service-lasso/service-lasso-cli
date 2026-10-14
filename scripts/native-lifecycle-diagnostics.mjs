// Opt-in source-test observations only. Never an admission/retirement receipt.
// Labels and values are closed so no path, input, argv, identity or error text
// can cross this diagnostic stderr channel.
const stages = new Set(['writer-build', 'darwin-helper-build', 'bundle', 'sea-blob', 'signature-remove', 'postject', 'signature-add', 'primary-stage', 'primary-build', 'provenance', 'package', 'native']);
const events = new Set(['start', 'complete', 'failed', 'spawn', 'spawn-error', 'stdin-error', 'exit', 'stdout-end', 'stderr-end', 'close']);
const signals = new Set(['SIGHUP', 'SIGINT', 'SIGQUIT', 'SIGILL', 'SIGTRAP', 'SIGABRT', 'SIGBUS', 'SIGFPE', 'SIGKILL', 'SIGUSR1', 'SIGSEGV', 'SIGUSR2', 'SIGPIPE', 'SIGALRM', 'SIGTERM', 'SIGCHLD', 'SIGCONT', 'SIGSTOP', 'SIGTSTP', 'SIGTTIN', 'SIGTTOU', 'SIGBREAK']);
export function nativeLifecycleDiagnostics(enabled, sink = value => process.stderr.write(value)) {
  let sequence = 0;
  return (stage, event, code = null, signal = null) => {
    if (!enabled) return;
    if (!stages.has(stage) || !events.has(event)) throw new Error('invalid native lifecycle diagnostic label');
    const boundedCode = Number.isInteger(code) && code >= 0 && code <= 255 ? code : null;
    const boundedSignal = signals.has(signal) ? signal : null;
    sink(`${JSON.stringify({ diagnostic: 'native-lifecycle.v1', sequence: ++sequence, stage, event, code: boundedCode, signal: boundedSignal })}\n`);
  };
}