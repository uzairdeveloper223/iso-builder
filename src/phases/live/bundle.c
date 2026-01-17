/**
 * This code is responsible for downloading and bundling bootloader .deb
 * packages (with dependencies) into the live rootfs APT cache for the
 * installer to use.
 */

#include "all.h"

/**
 * Copies .deb files from source directory to destination directory.
 */
static int copy_debs(const char *src_dir, const char *dst_dir)
{
    // Quote the source path for shell safety.
    char quoted_src[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(src_dir, quoted_src, sizeof(quoted_src)) != 0)
    {
        return -1;
    }

    // Quote the destination path for shell safety.
    char quoted_dst[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(dst_dir, quoted_dst, sizeof(quoted_dst)) != 0)
    {
        return -1;
    }

    // Copy .deb files.
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command), "cp %s/*.deb %s/", quoted_src, quoted_dst);
    return run_command(command);
}

/**
 * Checks if bootloader packages exist in the build cache.
 */
static int has_bundle_cache(void)
{
    // Get build cache directory.
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        return 0;
    }

    // Construct bundle directory path.
    char bundle_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(bundle_dir, sizeof(bundle_dir), "%s/packages/bootloader", cache_dir);

    // Check if directory exists and has .deb files.
    char pattern[COMMAND_PATH_MAX_LENGTH];
    snprintf(pattern, sizeof(pattern), "%s/*.deb", bundle_dir);

    // Look for the first matching .deb file.
    char first_match[COMMAND_PATH_MAX_LENGTH];
    return find_first_glob(pattern, first_match, sizeof(first_match)) == 0;
}

/**
 * Restores bundled packages from the build cache to the APT cache directory.
 */
static int restore_bundles(const char *apt_cache_dir)
{
    // Get build cache directory.
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        return -1;
    }

    // Construct bundle directory path.
    char bundle_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(bundle_dir, sizeof(bundle_dir), "%s/packages/bootloader", cache_dir);

    // Copy .deb files from build cache to APT cache.
    LOG_INFO("Restoring bootloader packages from build cache...");
    return copy_debs(bundle_dir, apt_cache_dir);
}

/**
 * Saves bundled packages from the APT cache to the build cache.
 */
static int save_bundles(const char *apt_cache_dir)
{
    // Get build cache directory.
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        LOG_WARNING("Failed to save to build cache: cannot determine cache directory");
        return -1;
    }

    // Construct bundle directory path.
    char bundle_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(bundle_dir, sizeof(bundle_dir), "%s/packages/bootloader", cache_dir);

    // Create the bundle directory.
    if (mkdir_p(bundle_dir) != 0)
    {
        LOG_WARNING("Failed to save to build cache: cannot create directory");
        return -1;
    }

    // Copy all .deb files to build cache.
    if (copy_debs(apt_cache_dir, bundle_dir) != 0)
    {
        LOG_WARNING("Failed to save to build cache: copy failed");
        return -1;
    }

    return 0;
}

/**
 * Downloads packages using apt-get download.
 * Packages are downloaded to the current directory, so we cd to the APT cache first.
 */
static int download_packages(const char *rootfs, const char *packages)
{
    char command[COMMAND_MAX_LENGTH];
    snprintf(command, sizeof(command),
        "cd " CONFIG_APT_CACHE_DIR " && apt-get download %s", packages);
    return run_chroot(rootfs, command);
}

int bundle_live_packages(const char *live_rootfs_path, int use_cache)
{
    LOG_INFO("Bundling bootloader packages into live APT cache...");

    // Construct the APT cache directory path in the live rootfs.
    char apt_cache_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(apt_cache_dir, sizeof(apt_cache_dir),
        "%s" CONFIG_APT_CACHE_DIR, live_rootfs_path);

    // Ensure the APT cache directory exists.
    if (mkdir_p(apt_cache_dir) != 0)
    {
        LOG_ERROR("Failed to create APT cache directory");
        return -1;
    }

    // Try to restore from build cache first.
    int cached = use_cache && has_bundle_cache();
    if (cached)
    {
        if (restore_bundles(apt_cache_dir) != 0)
        {
            LOG_WARNING("Failed to restore from build cache, downloading...");
            cached = 0;
        }
    }

    // Download packages if not in build cache.
    if (!cached)
    {
        // Update package lists (needed after cleanup_apt_directories removes them).
        LOG_INFO("Updating package lists...");
        if (run_chroot(live_rootfs_path, "apt-get update") != 0)
        {
            LOG_ERROR("Failed to update package lists");
            return -2;
        }

        // Download BIOS bootloader packages.
        LOG_INFO("Downloading BIOS bootloader packages...");
        if (download_packages(live_rootfs_path, CONFIG_BIOS_PACKAGES) != 0)
        {
            LOG_ERROR("Failed to download BIOS bootloader packages");
            return -2;
        }

        // Download EFI bootloader packages.
        LOG_INFO("Downloading EFI bootloader packages...");
        if (download_packages(live_rootfs_path, CONFIG_EFI_PACKAGES) != 0)
        {
            LOG_ERROR("Failed to download EFI bootloader packages");
            return -2;
        }

        // Save to build cache for future builds.
        if (use_cache)
        {
            save_bundles(apt_cache_dir);
        }

        // Clean up apt lists and cache files to reduce image size.
        // Keep only the downloaded .deb files in /var/cache/apt/archives/.
        run_chroot(live_rootfs_path, "rm -rf /var/lib/apt/lists/*");
        run_chroot(live_rootfs_path, "rm -f /var/cache/apt/*.bin");
    }

    LOG_INFO("Bootloader packages bundled successfully");

    return 0;
}
