/**
 * This code is responsible for installing component binaries into the rootfs.
 */

#include "all.h"

int install_components(const char *rootfs_path, const char *components_path)
{
    char command[MAX_COMMAND_LENGTH];
    char path[MAX_PATH_LENGTH];

    LOG_INFO("Installing components into rootfs...");

    // Create the target directory for binaries.
    snprintf(path, sizeof(path), "%s/usr/local/bin", rootfs_path);
    if (mkdir_p(path) != 0)
    {
        return -1;
    }

    // Install required components.
    for (int i = 0; i < REQUIRED_COMPONENTS_COUNT; i++)
    {
        // Copy the required component binary.
        snprintf(
            command, sizeof(command),
            "cp %s/%s %s/usr/local/bin/",
            components_path, REQUIRED_COMPONENTS[i], rootfs_path
        );
        if (run_command(command) != 0)
        {
            LOG_ERROR("Failed to install required component: %s", REQUIRED_COMPONENTS[i]);
            return -1;
        }

        // Make the binary executable.
        snprintf(
            command, sizeof(command),
            "chmod +x %s/usr/local/bin/%s",
            rootfs_path, REQUIRED_COMPONENTS[i]
        );
        run_command(command);

        LOG_INFO("Installed %s", REQUIRED_COMPONENTS[i]);
    }

    // Install optional components if they exist.
    for (int i = 0; i < OPTIONAL_COMPONENTS_COUNT; i++)
    {
        // Check if the optional component binary exists.
        snprintf(path, sizeof(path), "%s/%s", components_path, OPTIONAL_COMPONENTS[i]);
        if (!file_exists(path))
        {
            LOG_INFO("Skipping optional component: %s", OPTIONAL_COMPONENTS[i]);
            continue;
        }

        // Copy the optional component binary.
        snprintf(
            command, sizeof(command),
            "cp %s/%s %s/usr/local/bin/",
            components_path, OPTIONAL_COMPONENTS[i], rootfs_path
        );
        if (run_command(command) != 0)
        {
            LOG_WARNING("Failed to install optional component: %s", OPTIONAL_COMPONENTS[i]);
            continue;
        }

        // Make the binary executable.
        snprintf(
            command, sizeof(command),
            "chmod +x %s/usr/local/bin/%s",
            rootfs_path, OPTIONAL_COMPONENTS[i]
        );
        run_command(command);

        LOG_INFO("Installed %s", OPTIONAL_COMPONENTS[i]);
    }

    LOG_INFO("All required components installed successfully");

    return 0;
}
