/**
 * This code is responsible for lightly stripping the payload rootfs.
 *
 * Unlike the carrier environment, the payload rootfs is the user's permanent
 * installation. We preserve locales and documentation but remove caches
 * to reduce the tarball size.
 */

#include "all.h"

int strip_payload_rootfs(const char *path)
{
    LOG_INFO("Stripping payload rootfs at %s", path);

    // Remove apt cache and lists (can be regenerated).
    if (cleanup_apt_directories(path) != 0)
    {
        return -1;
    }

    LOG_INFO("Payload rootfs stripped successfully");

    return 0;
}
