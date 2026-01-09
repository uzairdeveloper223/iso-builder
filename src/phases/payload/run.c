/**
 * This code is responsible for orchestrating the payload phase.
 */

#include "all.h"

int run_payload_phase(const char *rootfs_dir, const char *tarball_path)
{
    if (create_payload_rootfs(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create payload rootfs");
        return -1;
    }

    if (strip_payload_rootfs(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to strip payload rootfs");
        return -1;
    }

    if (package_payload_rootfs(rootfs_dir, tarball_path) != 0)
    {
        LOG_ERROR("Failed to package payload rootfs");
        return -1;
    }

    rm_rf(rootfs_dir);
    LOG_INFO("Phase 2A complete: Payload rootfs packaged");
    return 0;
}
