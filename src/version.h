#pragma once

/** The maximum length for version strings (e.g., "1.2.345"). */
#define VERSION_MAX_LENGTH 32

/**
 * Resolves the latest release version within a major version for a component.
 *
 * Queries the GitHub API for the specified component repository and finds the
 * latest release that shares the same major version as the provided version.
 *
 * @param component The component name (repository name, e.g., "window-manager").
 * @param version The user-provided version (e.g., "1.0.0").
 * @param out_resolved The buffer to store the resolved version string.
 * @param buffer_length The size of the output buffer.
 *
 * @return - `0` - Indicates successful resolution.
 * @return - `-1` - Indicates a network or API failure.
 * @return - `-2` - Indicates no matching version was found.
 * @return - `-3` - Indicates JSON parsing failure.
 */
int resolve_version(
    const char *component,
    const char *version,
    char *out_resolved,
    size_t buffer_length
);

/**
 * Extracts the major version number from a semantic version string.
 *
 * @param version The version string (e.g., "1.2.3" or "v1.2.3").
 *
 * @return The major version number, or -1 if parsing fails.
 */
int extract_major_version(const char *version);
