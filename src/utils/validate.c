/**
 * This code is responsible for input validation utilities.
 */

#include "all.h"

int validate_version(const char *version)
{
    const char *p = version;

    // Skip optional 'v' prefix.
    if (*p == 'v' || *p == 'V')
    {
        p++;
    }

    // Parse major version (required, at least one digit).
    if (*p < '0' || *p > '9')
    {
        return -1;
    }
    while (*p >= '0' && *p <= '9')
    {
        p++;
    }

    // Require dot separator.
    if (*p != '.')
    {
        return -1;
    }
    p++;

    // Parse minor version (required, at least one digit).
    if (*p < '0' || *p > '9')
    {
        return -1;
    }
    while (*p >= '0' && *p <= '9')
    {
        p++;
    }

    // Require dot separator.
    if (*p != '.')
    {
        return -1;
    }
    p++;

    // Parse patch version (required, at least one digit).
    if (*p < '0' || *p > '9')
    {
        return -1;
    }
    while (*p >= '0' && *p <= '9')
    {
        p++;
    }

    // Ensure no trailing characters.
    if (*p != '\0')
    {
        return -1;
    }

    return 0;
}
