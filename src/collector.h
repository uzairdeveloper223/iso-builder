#pragma once

/**
 * Initializes the collector module.
 *
 * Must be called before any other collector functions. Initializes libcurl globally.
 *
 * @return - `0` - Indicates successful initialization.
 * @return - `-1` - Indicates initialization failure.
 */
int init_collector(void);

/**
 * Cleans up the collector module.
 *
 * Should be called when the collector module is no longer needed. Cleans up libcurl.
 */
void cleanup_collector(void);

/**
 * Fetches a component binary from local cache or GitHub releases.
 *
 * @param name The component name (e.g., "window-manager").
 * @param version The release version tag to download.
 * @param output_directory The directory to save the binary.
 *
 * @return - `0` - Indicates successful fetch.
 * @return - `-1` - Indicates a failure.
 */
int fetch_component(
    const char *name,
    const char *version,
    const char *output_directory
);

/**
 * Fetches all LimeOS components from local cache or GitHub releases.
 *
 * Fetches window-manager, display-manager, and installation-wizard binaries.
 *
 * @param version The release version tag to download.
 * @param output_directory The directory to save the binaries.
 *
 * @return - `0` - Indicates all components fetched successfully.
 * @return - `-1` - Indicates one or more components failed.
 */
int fetch_all_components(const char *version, const char *output_directory);
