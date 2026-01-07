#pragma once

/**
 * Configures GRUB for UEFI boot.
 *
 * Sets up GRUB bootloader configuration with quiet splash kernel parameters.
 *
 * @param rootfs_path The path to the root filesystem directory.
 *
 * @return - `0` - Indicates successful configuration.
 * @return - `-1` - Indicates configuration failure.
 */
int setup_grub(const char *rootfs_path);

/**
 * Configures isolinux for legacy BIOS boot.
 *
 * Sets up isolinux bootloader configuration with quiet splash kernel 
 * parameters.
 *
 * @param rootfs_path The path to the root filesystem directory.
 *
 * @return - `0` - Indicates successful configuration.
 * @return - `-1` - Indicates configuration failure.
 */
int setup_isolinux(const char *rootfs_path);

/**
 * Configures Plymouth with the LimeOS boot splash.
 *
 * Installs the splash image and configures Plymouth to display it during boot.
 *
 * @param rootfs_path The path to the root filesystem directory.
 * @param logo_path The path to the splash logo image.
 *
 * @return - `0` - Indicates successful configuration.
 * @return - `-1` - Indicates configuration failure.
 */
int setup_splash(const char *rootfs_path, const char *logo_path);
