/**
 * This code is responsible for configuring the init system in the rootfs.
 */

#include "all.h"

static int write_installer_service(const char *rootfs_path)
{
    char path[MAX_PATH_LENGTH];
    FILE *file;

    // Create the systemd service directory.
    snprintf(path, sizeof(path), "%s/etc/systemd/system", rootfs_path);
    if (mkdir_p(path) != 0)
    {
        return -1;
    }

    // Create the installer service unit file.
    snprintf(path, sizeof(path), "%s/etc/systemd/system/limeos-installer.service", rootfs_path);
    file = fopen(path, "w");
    if (!file)
    {
        LOG_ERROR("Failed to create file %s: %s", path, strerror(errno));
        return -1;
    }
    fprintf(file,
        "[Unit]\n"
        "Description=LimeOS Installation Wizard\n"
        "After=display-manager.service\n"
        "\n"
        "[Service]\n"
        "Type=simple\n"
        "ExecStart=/usr/local/bin/installation-wizard\n"
        "Restart=on-failure\n"
        "\n"
        "[Install]\n"
        "WantedBy=graphical.target\n"
    );
    fclose(file);

    return 0;
}

static int enable_installer_service(const char *rootfs_path)
{
    char command[MAX_COMMAND_LENGTH];
    char path[MAX_PATH_LENGTH];

    // Create the target wants directory.
    snprintf(path, sizeof(path), "%s/etc/systemd/system/graphical.target.wants", rootfs_path);
    if (mkdir_p(path) != 0)
    {
        return -1;
    }

    // Create symlink to enable the service.
    snprintf(
        command, sizeof(command),
        "ln -sf ../limeos-installer.service %s/etc/systemd/system/graphical.target.wants/limeos-installer.service",
        rootfs_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    return 0;
}

static int set_default_target(const char *rootfs_path)
{
    char command[MAX_COMMAND_LENGTH];

    // Set graphical target as default.
    snprintf(
        command, sizeof(command),
        "ln -sf /lib/systemd/system/graphical.target %s/etc/systemd/system/default.target",
        rootfs_path
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    return 0;
}

static int disable_getty(const char *rootfs_path)
{
    char command[MAX_COMMAND_LENGTH];

    // Remove getty services to prevent login prompts.
    snprintf(command, sizeof(command), "rm -f %s/etc/systemd/system/getty.target.wants/*", rootfs_path);
    if (run_command(command) != 0)
    {
        LOG_ERROR("Command failed: %s", command);
        return -1;
    }

    return 0;
}

int configure_init(const char *rootfs_path)
{
    LOG_INFO("Configuring init system...");

    if (write_installer_service(rootfs_path) != 0)
    {
        return -1;
    }
    if (enable_installer_service(rootfs_path) != 0)
    {
        return -1;
    }
    if (set_default_target(rootfs_path) != 0)
    {
        return -1;
    }
    if (disable_getty(rootfs_path) != 0)
    {
        return -1;
    }

    LOG_INFO("Init system configured successfully");

    return 0;
}
