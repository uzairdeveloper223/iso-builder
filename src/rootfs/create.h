#pragma once

/**
 * Creates a minimal Debian root filesystem using debootstrap.
 *
 * @param path The directory where the rootfs will be created.
 *
 * @return - `0` - Indicates successful creation.
 * @return - `-1` - Indicates creation failure.
 */
int create_rootfs(const char *path);

/**
 * Strips unnecessary packages, docs, and locales from the rootfs.
 *
 * @param path The path to the rootfs directory.
 *
 * @return - `0` - Indicates successful stripping.
 * @return - `-1` - Indicates stripping failure.
 */
int strip_rootfs(const char *path);
