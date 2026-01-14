#pragma once

/**
 * Downloads and bundles bootloader .deb packages into the carrier rootfs.
 *
 * Downloads bootloader packages (grub-pc, grub-efi) that cannot be
 * pre-installed due to conflicts and stores them for the installer to
 * selectively install based on the target system's boot mode.
 *
 * @param carrier_rootfs_path The path to the carrier rootfs directory.
 * @param use_cache Whether to use package caching.
 *
 * @return - `0` - Indicates successful bundling.
 * @return - `-1` - Indicates directory creation failure.
 * @return - `-2` - Indicates package download failure.
 */
int bundle_packages(const char *carrier_rootfs_path, int use_cache);
