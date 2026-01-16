/**
 * This code configures GRUB for silent boot on the installed system.
 */

#include "all.h"

int brand_grub(const char *rootfs_path)
{
    LOG_INFO("Configuring GRUB for silent boot...");

    // Use drop-in directory to override GRUB settings without replacing
    // the package-managed /etc/default/grub file.
    char dir_path[COMMAND_PATH_MAX_LENGTH];
    snprintf(dir_path, sizeof(dir_path), "%s/etc/default/grub.d", rootfs_path);
    if (mkdir_p(dir_path) != 0)
    {
        LOG_ERROR("Failed to create /etc/default/grub.d");
        return -1;
    }

    // Write the GRUB config file.
    char file_path[COMMAND_PATH_MAX_LENGTH];
    const char *content =
        "GRUB_DISTRIBUTOR=\"" CONFIG_OS_NAME "\"    # OS name shown in GRUB menu\n"
        "GRUB_TIMEOUT=0                             # No delay before booting default entry\n"
        "GRUB_TIMEOUT_STYLE=hidden                  # Hide the menu completely\n"
        "GRUB_RECORDFAIL_TIMEOUT=0                  # Don't wait on previous boot failure\n"
        "GRUB_GFXMODE=auto                          # Auto-detect display's native resolution\n"
        "GRUB_GFXPAYLOAD_LINUX=keep                 # Preserve video mode during kernel handoff\n"
        "GRUB_CMDLINE_LINUX_DEFAULT=\"" 
        "quiet "                                    // Minimize boot messages.
        "splash "                                   // Show splash screen.
        "loglevel=0 "                               // Suppress kernel log messages.
        "vt.global_cursor_default=0 "               // Hide the blinking cursor.
        "\"\n";
    snprintf(file_path, sizeof(file_path), "%s/distributor.cfg", dir_path);
    if (write_file(file_path, content) != 0)
    {
        LOG_ERROR("Failed to write GRUB config");
        return -2;
    }

    LOG_INFO("GRUB configured successfully");

    return 0;
}
