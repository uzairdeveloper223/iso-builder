#pragma once

/**
 * Creates a Debian root filesystem for the payload system installation.
 *
 * The payload rootfs is the full system that gets installed to disk. It
 * includes bootloaders, networking, and other packages needed for a
 * functional system.
 *
 * @param path The directory where the rootfs will be created.
 *
 * @return - `0` - Indicates successful creation.
 * @return - `-1` - Indicates debootstrap failure.
 * @return - `-2` - Indicates package list update failure.
 * @return - `-3` - Indicates package installation failure.
 */
int create_payload_rootfs(const char *path);
