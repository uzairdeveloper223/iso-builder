#pragma once

/**
 * Aggressively strips unnecessary files from the carrier rootfs.
 *
 * The carrier environment requires minimal size since it runs entirely from
 * RAM. This function removes documentation, non-English locales, and
 * package caches.
 *
 * @param path The path to the carrier rootfs directory.
 *
 * @return - `0` - Indicates successful stripping.
 * @return - `-1` - Indicates documentation removal failure.
 * @return - `-2` - Indicates locale removal failure.
 * @return - `-3` - Indicates apt cache removal failure.
 * @return - `-4` - Indicates apt lists removal failure.
 */
int strip_carrier_rootfs(const char *path);
