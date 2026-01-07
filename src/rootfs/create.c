/**
 * This code is responsible for creating and stripping the root filesystem.
 */

#include "all.h"

/** The Debian release to use for the base rootfs. */
#define CREATE_DEBIAN_RELEASE "bookworm"

int create_rootfs(const char *path)
{
    char command[MAX_COMMAND_LENGTH];

    // Log the start of rootfs creation.
    LOG_INFO("Creating rootfs at %s", path);

    // Run debootstrap to create a minimal Debian rootfs.
    snprintf(
        command, sizeof(command),
        "debootstrap --variant=minbase %s %s",
        CREATE_DEBIAN_RELEASE, path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    LOG_INFO("Rootfs created successfully");

    return 0;
}

int strip_rootfs(const char *path)
{
    char command[MAX_COMMAND_LENGTH];

    // Log the start of rootfs stripping.
    LOG_INFO("Stripping rootfs at %s", path);

    // Remove documentation files.
    snprintf(
        command, sizeof(command),
        "rm -rf %s/usr/share/doc %s/usr/share/man %s/usr/share/info",
        path, path, path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    // Remove non-English locales.
    snprintf(
        command, sizeof(command),
        "find %s/usr/share/locale -mindepth 1 -maxdepth 1 ! -name 'en*' -exec rm -rf {} +",
        path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    // Remove apt cache.
    snprintf(command, sizeof(command), "rm -rf %s/var/cache/apt/*", path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    // Remove apt lists.
    snprintf(command, sizeof(command), "rm -rf %s/var/lib/apt/lists/*", path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    LOG_INFO("Rootfs stripped successfully");

    return 0;
}
