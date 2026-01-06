/**
 * This code is responsible for logging messages.
 */

#include "all.h"

void log_info(const char *message)
{
    printf("[INFO] %s\n", message);
}

void log_success(const char *message)
{
    printf("[OK] %s\n", message);
}

void log_error(const char *message)
{
    fprintf(stderr, "[ERROR] %s\n", message);
}
