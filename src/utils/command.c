/**
 * This code is responsible for executing shell commands.
 */

#include "all.h"

int run_command(const char *command)
{
    return system(command);
}
