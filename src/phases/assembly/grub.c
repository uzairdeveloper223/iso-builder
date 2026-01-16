/**
 * This code is responsible for configuring GRUB for UEFI boot.
 */

#include "all.h"

int setup_grub(const char *rootfs_path)
{
    LOG_INFO("Configuring GRUB for UEFI boot");

    // Construct the GRUB directory path.
    char grub_dir[COMMAND_PATH_MAX_LENGTH];
    snprintf(grub_dir, sizeof(grub_dir), "%s/boot/grub", rootfs_path);

    // Create the GRUB directory.
    if (mkdir_p(grub_dir) != 0)
    {
        LOG_ERROR("Failed to create GRUB directory: %s", grub_dir);
        return -1;
    }

    // Construct the grub.cfg path.
    char grub_cfg_path[COMMAND_PATH_MAX_LENGTH];
    snprintf(grub_cfg_path, sizeof(grub_cfg_path), "%s/grub.cfg", grub_dir);

    // Define the GRUB configuration content.
    const char *grub_cfg =
        "set gfxmode=auto         # Auto-detect display's native resolution.\n"
        "set gfxpayload=keep      # Preserve video mode during kernel handoff.\n"
        "set default=0            # Boot first menu entry.\n"
        "set timeout_style=hidden # Hide the menu completely.\n"
        "set timeout=0            # No delay before booting.\n"
        "\n"
        "menuentry \"" CONFIG_GRUB_MENU_ENTRY_NAME "\" {\n"
        "    linux " CONFIG_BOOT_KERNEL_PATH " " CONFIG_BOOT_KERNEL_PARAMS "\n"
        "    initrd " CONFIG_BOOT_INITRD_PATH "\n"
        "}\n";

    // Write the GRUB configuration file.
    if (write_file(grub_cfg_path, grub_cfg) != 0)
    {
        return -2;
    }

    LOG_INFO("GRUB configured successfully");

    return 0;
}
