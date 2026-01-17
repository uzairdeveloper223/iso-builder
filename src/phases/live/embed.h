#pragma once

/**
 * Embeds the target rootfs tarball into the live rootfs.
 *
 * Copies the target rootfs tarball to the configured location within the
 * live rootfs so the installer can access it during installation.
 *
 * @param live_rootfs_path The path to the live rootfs directory.
 * @param tarball_path The path to the target rootfs tarball.
 *
 * @return - `0` - Indicates successful embedding.
 * @return - `-1` - Indicates directory creation failure.
 * @return - `-2` - Indicates copy failure.
 */
int embed_target_rootfs(const char *live_rootfs_path, const char *tarball_path);
