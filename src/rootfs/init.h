#pragma once

/**
 * Configures init to launch the installer directly.
 *
 * Sets up the rootfs so it boots directly into the installation wizard
 * without a login prompt or shell.
 *
 * @param rootfs_path The path to the rootfs directory.
 *
 * @return - `0` - Indicates successful configuration.
 * @return - `-1` - Indicates configuration failure.
 */
int configure_init(const char *rootfs_path);
