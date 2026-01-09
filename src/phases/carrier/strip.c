/**
 * This code is responsible for aggressively stripping unnecessary files
 * from the carrier rootfs to minimize ISO size.
 */

#include "all.h"

int strip_carrier_rootfs(const char *path)
{
    char dir_path[COMMAND_PATH_MAX_LENGTH];

    // Log the start of rootfs stripping.
    LOG_INFO("Stripping carrier rootfs at %s", path);

    // Remove documentation files.
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/doc", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove doc directory");
        return -1;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/man", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove man directory");
        return -1;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/info", path);
    if (rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove info directory");
        return -1;
    }

    // Remove non-English locales.
    char command[COMMAND_MAX_LENGTH];
    char quoted_locale_dir[COMMAND_QUOTED_MAX_LENGTH];
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/locale", path);
    if (shell_quote_path(dir_path, quoted_locale_dir, sizeof(quoted_locale_dir)) != 0)
    {
        LOG_ERROR("Failed to quote locale directory");
        return -2;
    }
    snprintf(
        command, sizeof(command),
        "find %s -mindepth 1 -maxdepth 1 ! -name 'en*' -exec rm -rf {} +",
        quoted_locale_dir
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to remove non-English locales");
        return -2;
    }

    // Remove apt cache and lists.
    if (cleanup_apt_directories(path) != 0)
    {
        return -3;
    }

    LOG_INFO("Carrier rootfs stripped successfully");

    return 0;
}
