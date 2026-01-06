#pragma once

/**
 * Initializes the fetch module.
 *
 * Must be called before any other fetch functions. Initializes libcurl globally.
 *
 * @return - `0` - Indicates successful initialization.
 * @return - `-1` - Indicates initialization failure.
 */
int fetch_init(void);

/**
 * Cleans up the fetch module.
 *
 * Should be called when the fetch module is no longer needed. Cleans up libcurl.
 */
void fetch_cleanup(void);

/**
 * Downloads a component binary from GitHub releases.
 *
 * @param name The component name (e.g., "window-manager").
 * @param version The release version tag to download.
 * @param output_directory The directory to save the downloaded binary.
 *
 * @return - `0` - Indicates successful download.
 * @return - `-1` - Indicates a download failure.
 */
int fetch_component(
    const char *name,
    const char *version,
    const char *output_directory
);

/**
 * Downloads all LimeOS components from GitHub releases.
 *
 * Fetches window-manager, display-manager, and installation-wizard binaries.
 *
 * @param version The release version tag to download.
 * @param output_directory The directory to save the downloaded binaries.
 *
 * @return - `0` - Indicates all components downloaded successfully.
 * @return - `-1` - Indicates one or more downloads failed.
 */
int fetch_all_components(const char *version, const char *output_directory);
