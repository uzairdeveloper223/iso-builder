#pragma once

/**
 * Creates a minimal Debian root filesystem for the carrier environment.
 *
 * The carrier rootfs is optimized for running the installer from the ISO.
 * It includes only the packages necessary to boot and run the installation
 * wizard.
 *
 * @param path The directory where the rootfs will be created.
 *
 * @return - `0` - Indicates successful creation.
 * @return - `-1` - Indicates debootstrap failure.
 * @return - `-2` - Indicates package list update failure.
 * @return - `-3` - Indicates package installation failure.
 * @return - `-4` - Indicates kernel copy failure.
 * @return - `-5` - Indicates initrd copy failure.
 */
int create_carrier_rootfs(const char *path);
