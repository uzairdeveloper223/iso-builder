#pragma once

/**
 * Runs the carrier phase.
 *
 * Creates the carrier rootfs (the live system that boots from ISO), embeds
 * the payload tarball, installs LimeOS components, configures init, bundles
 * boot-mode-specific packages, and strips unnecessary files.
 *
 * @param rootfs_dir The directory for the carrier rootfs.
 * @param tarball_path The path to the payload tarball to embed.
 * @param components_dir The directory containing downloaded components.
 *
 * @return - `0` - Indicates success.
 * @return - `-1` - Indicates failure.
 */
int run_carrier_phase(
    const char *rootfs_dir,
    const char *tarball_path,
    const char *components_dir
);
