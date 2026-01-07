/**
 * This code is responsible for filesystem utility operations.
 */

#include "all.h"

int mkdir_p(const char *path)
{
    char command[MAX_COMMAND_LENGTH];

    // Construct the mkdir command with parent directory creation.
    snprintf(command, sizeof(command), "mkdir -p %s", path);

    // Execute the command and check for errors.
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    return 0;
}

int copy_file(const char *src, const char *dst)
{
    char command[MAX_COMMAND_LENGTH];

    // Construct the copy command.
    snprintf(command, sizeof(command), "cp %s %s", src, dst);

    // Execute the command and check for errors.
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    return 0;
}

int file_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}
