/**
 * This code is responsible for executing shell commands.
 */

#include "all.h"

int run_command(const char *command)
{
    return system(command);
}

int run_command_output(
    const char *command,
    char *out_buffer,
    size_t buffer_length
)
{
    // Initialize the output buffer to empty.
    out_buffer[0] = '\0';

    // Open a pipe to execute the command.
    FILE *pipe = popen(command, "r");
    if (!pipe)
    {
        return -1;
    }

    // Read the command output into the buffer.
    size_t total_read = 0;
    while (total_read < buffer_length - 1 &&
           fgets(out_buffer + total_read, buffer_length - total_read, pipe))
    {
        total_read = strlen(out_buffer);
    }

    // Close the pipe and return its exit status.
    return pclose(pipe);
}
