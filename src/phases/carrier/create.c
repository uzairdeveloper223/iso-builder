/**
 * This code is responsible for creating a minimal Debian root filesystem
 * for the carrier environment (boots from ISO, runs the installer).
 */

#include "all.h"

int create_carrier_rootfs(const char *path)
{
    char command[COMMAND_MAX_LENGTH];
    char quoted_path[COMMAND_QUOTED_MAX_LENGTH];

    LOG_INFO("Creating carrier rootfs at %s", path);

    // Quote the path to prevent shell injection.
    if (shell_quote_path(path, quoted_path, sizeof(quoted_path)) != 0)
    {
        LOG_ERROR("Failed to quote path");
        return -1;
    }

    // Run debootstrap to create a minimal Debian rootfs.
    snprintf(
        command, sizeof(command),
        "debootstrap --variant=minbase %s %s",
        CONFIG_DEBIAN_RELEASE, quoted_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: debootstrap");
        return -1;
    }

    // Install required packages into the rootfs.
    LOG_INFO("Installing carrier environment packages...");
    if (run_chroot(path, "apt-get update") != 0)
    {
        LOG_ERROR("Failed to update package lists");
        return -2;
    }

    // Install all packages needed by the carrier installation environment.
    if (run_chroot(path,
        "apt-get install -y --no-install-recommends " CONFIG_CARRIER_PACKAGES) != 0)
    {
        LOG_ERROR("Failed to install required packages");
        return -3;
    }

    // Copy kernel and initrd to standard paths for boot loaders.
    if (copy_kernel_and_initrd(path) != 0)
    {
        return -4;
    }

    LOG_INFO("Carrier rootfs created successfully");

    return 0;
}
