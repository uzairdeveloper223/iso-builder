/**
 * This code orchestrates live-specific branding.
 *
 * Applies OS identity and splash branding for the live environment.
 */

#include "all.h"

int brand_live_rootfs(const char *path, const char *version)
{
    LOG_INFO("Applying live branding...");

    // Apply OS identity branding.
    if (brand_os_identity(path, version) != 0)
    {
        return -1;
    }

    // Apply Plymouth splash branding.
    if (brand_splash(path, CONFIG_SPLASH_LOGO_PATH) != 0)
    {
        return -2;
    }

    LOG_INFO("Live branding applied successfully");

    return 0;
}
