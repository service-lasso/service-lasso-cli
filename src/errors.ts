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
  // Native errors can include paths, URLs, environment values, and even
  // request metadata. They are not part of the CLI's public diagnostic API.
  if (error instanceof Error) return new CliError("unexpected_error", "The command could not be completed.");
  return new CliError("unexpected_error", "An unexpected error occurred.");
}
