export class CliError extends Error {
  public constructor(
    public readonly code: string,
    message: string,
    public readonly hint?: string,
  ) {
    super(message);
    this.name = "CliError";
  }
}

export function asCliError(error: unknown): CliError {
  if (error instanceof CliError) return error;
  if (error instanceof Error) return new CliError("unexpected_error", error.message);
  return new CliError("unexpected_error", "An unexpected error occurred.");
}
