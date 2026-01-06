#pragma once

/**
 * Executes a shell command and returns its exit code.
 *
 * @param command The shell command to execute.
 *
 * @return The exit code of the command.
 */
int run_command(const char *command);

/**
 * Executes a shell command and captures its stdout output.
 *
 * @param command The shell command to execute.
 * @param out_buffer The buffer to store the captured output.
 * @param buffer_length The size of the output buffer.
 *
 * @return The exit code of the command, or -1 on failure.
 */
int run_command_output(
    const char *command,
    char *out_buffer,
    size_t buffer_length
);
