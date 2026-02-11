/**
 * This code is responsible for aggressively stripping noncritical files
 * from the base rootfs to minimize size for both target and live.
 */

#include "all.h"

int strip_base_rootfs(const char *path)
{
    char dir_path[COMMON_MAX_PATH_LENGTH];

    LOG_INFO("Stripping base rootfs at %s", path);

    // Remove documentation files.
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/doc", path);
    if (common.rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove doc directory");
        return -1;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/man", path);
    if (common.rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove man directory");
        return -2;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/info", path);
    if (common.rm_rf(dir_path) != 0)
    {
        LOG_ERROR("Failed to remove info directory");
        return -3;
    }

    // Remove non-English locales.
    char command[COMMON_MAX_COMMAND_LENGTH];
    char quoted_locale_dir[COMMON_MAX_QUOTED_LENGTH];
    snprintf(dir_path, sizeof(dir_path), "%s/usr/share/locale", path);
    if (common.shell_escape_path(dir_path, quoted_locale_dir, sizeof(quoted_locale_dir)) != 0)
    {
        LOG_ERROR("Failed to quote locale directory");
        return -4;
    }
    snprintf(
        command, sizeof(command),
        "find %s -mindepth 1 -maxdepth 1 ! -name 'en*' -exec rm -rf {} +",
        quoted_locale_dir
    );
    if (common.run_command_indented(command) != 0)
    {
        LOG_ERROR("Failed to remove non-English locales");
        return -5;
    }

    // Clear MOTD files that display Debian messages on login.
    snprintf(dir_path, sizeof(dir_path), "%s/etc/motd", path);
    if (common.write_file(dir_path, "") != 0)
    {
        LOG_ERROR("Failed to clear /etc/motd");
        return -6;
    }
    snprintf(dir_path, sizeof(dir_path), "%s/etc/update-motd.d", path);
    common.rm_rf(dir_path);  // OK if it doesn't exist.

    LOG_INFO("Base rootfs stripped successfully");

    return 0;
}
