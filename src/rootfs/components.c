/**
 * This code is responsible for installing component binaries into the rootfs.
 */

#include "all.h"

int install_components(const char *rootfs_path, const char *components_path)
{
    char command[MAX_COMMAND_LENGTH];
    char path[MAX_PATH_LENGTH];

    // Log the start of component installation.
    LOG_INFO("Installing components into rootfs...");

    // Create the target directory for binaries.
    snprintf(path, sizeof(path), "%s/usr/local/bin", rootfs_path);
    if (mkdir_p(path) != 0)
    {
        return -1;
    }

    // Copy each component binary into the rootfs.
    for (int i = 0; i < COMPONENTS_COUNT; i++)
    {
        snprintf(
            command, sizeof(command),
            "cp %s/%s %s/usr/local/bin/",
            components_path, COMPONENTS[i], rootfs_path
        );
        if (run_command(command) != 0)
        {
            LOG_ERROR("Command failed: %s", command);
            return -1;
        }

        // Make the binary executable.
        snprintf(
            command, sizeof(command),
            "chmod +x %s/usr/local/bin/%s",
            rootfs_path, COMPONENTS[i]
        );
        run_command(command);

        LOG_INFO("Installed %s", COMPONENTS[i]);
    }

    LOG_INFO("All components installed successfully");

    return 0;
}
