#pragma once

/**
 * Validates that a version string is in semantic versioning format.
 *
 * Accepts formats: "X.Y.Z" or "vX.Y.Z" where X, Y, Z are non-negative integers.
 *
 * @param version The version string to validate.
 *
 * @return - `0` - Indicates a valid version format.
 * @return - `-1` - Indicates an invalid version format.
 */
int validate_version(const char *version);
