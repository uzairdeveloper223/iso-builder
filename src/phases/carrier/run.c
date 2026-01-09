/**
 * This code is responsible for orchestrating the carrier phase.
 */

#include "all.h"

int run_carrier_phase(
    const char *rootfs_dir,
    const char *tarball_path,
    const char *components_dir
)
{
    if (create_carrier_rootfs(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create carrier rootfs");
        return -1;
    }

    if (embed_payload_rootfs(rootfs_dir, tarball_path) != 0)
    {
        LOG_ERROR("Failed to embed payload rootfs");
        return -1;
    }

    if (install_carrier_components(rootfs_dir, components_dir) != 0)
    {
        LOG_ERROR("Failed to install components");
        return -1;
    }

    if (configure_carrier_init(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure init");
        return -1;
    }

    if (bundle_packages(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to bundle packages");
        return -1;
    }

    if (strip_carrier_rootfs(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to strip carrier rootfs");
        return -1;
    }

    LOG_INFO("Phase 2B complete: Carrier rootfs created");
    return 0;
}
