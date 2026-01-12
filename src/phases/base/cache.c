/**
 * This code is responsible for caching the base rootfs to speed up
 * subsequent builds by skipping debootstrap and stripping operations.
 */

#include "all.h"

/** The cache directory name under XDG_CACHE_HOME or ~/.cache. */
#define CACHE_DIR_NAME "limeos"

/** Length of SHA256 hash (32 bytes). */
#define SHA256_DIGEST_LEN 32

/** Length of SHA256 hash in hex format (64 chars + null). */
#define SHA256_HEX_LENGTH 65

int compute_cache_key(char *out_key, size_t buffer_length)
{
    // Validate the output buffer is large enough.
    if (buffer_length < SHA256_HEX_LENGTH)
    {
        return -1;
    }

    // Initialize the digest context.
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        return -1;
    }

    // Configure the context for SHA256.
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return -1;
    }

    // Hash the Debian release.
    if (EVP_DigestUpdate(ctx, CONFIG_DEBIAN_RELEASE, strlen(CONFIG_DEBIAN_RELEASE)) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return -1;
    }

    // Hash the cache version.
    char version_str[32];
    snprintf(version_str, sizeof(version_str), "%d", CONFIG_CACHE_VERSION);
    if (EVP_DigestUpdate(ctx, version_str, strlen(version_str)) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return -1;
    }

    // Finalize the hash computation.
    unsigned char hash[SHA256_DIGEST_LEN];
    unsigned int digest_len = 0;
    if (EVP_DigestFinal_ex(ctx, hash, &digest_len) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return -1;
    }

    // Clean up the digest context.
    EVP_MD_CTX_free(ctx);

    // Convert the hash to a hex string.
    for (unsigned int i = 0; i < digest_len; i++)
    {
        snprintf(out_key + (i * 2), 3, "%02x", hash[i]);
    }
    out_key[SHA256_HEX_LENGTH - 1] = '\0';

    return 0;
}

int get_cache_dir(char *out_path, size_t buffer_length)
{
    // Check for XDG_CACHE_HOME environment variable.
    const char *cache_home = getenv("XDG_CACHE_HOME");
    if (cache_home && cache_home[0] != '\0')
    {
        snprintf(out_path, buffer_length, "%s/%s", cache_home, CACHE_DIR_NAME);
        return 0;
    }

    // Fall back to ~/.cache if HOME is set.
    const char *home = getenv("HOME");
    if (home && home[0] != '\0')
    {
        snprintf(out_path, buffer_length, "%s/.cache/%s", home, CACHE_DIR_NAME);
        return 0;
    }

    LOG_ERROR("Cannot determine cache directory: HOME not set");
    return -1;
}

int cache_exists(char *out_cache_path, size_t buffer_length)
{
    // Get the cache directory path.
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        return 0;
    }

    // Compute the cache key for the current configuration.
    char cache_key[SHA256_HEX_LENGTH];
    if (compute_cache_key(cache_key, sizeof(cache_key)) != 0)
    {
        return 0;
    }

    // Construct the full cache file path.
    snprintf(
        out_cache_path, buffer_length, "%s/base-rootfs-%s.tar.gz",
        cache_dir, cache_key
    );

    return file_exists(out_cache_path);
}

int restore_from_cache(const char *cache_path, const char *rootfs_dir)
{
    LOG_INFO("Restoring base rootfs from cache...");

    // Quote the cache path for shell safety.
    char quoted_cache[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(cache_path, quoted_cache, sizeof(quoted_cache)) != 0)
    {
        return -1;
    }

    // Quote the rootfs directory for shell safety.
    char quoted_dir[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(rootfs_dir, quoted_dir, sizeof(quoted_dir)) != 0)
    {
        return -1;
    }

    // Create the target directory.
    if (mkdir_p(rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create rootfs directory");
        return -1;
    }

    // Extract the cached tarball.
    char command[COMMAND_MAX_LENGTH];
    snprintf(
        command, sizeof(command),
        "tar -xzf %s -C %s", quoted_cache, quoted_dir
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to extract cached rootfs");
        return -1;
    }

    LOG_INFO("Base rootfs restored from cache");
    return 0;
}

int save_to_cache(const char *rootfs_dir)
{
    // Get the cache directory path.
    char cache_dir[COMMAND_PATH_MAX_LENGTH];
    if (get_cache_dir(cache_dir, sizeof(cache_dir)) != 0)
    {
        LOG_WARNING("Cannot save to cache: unable to determine cache directory");
        return -1;
    }

    // Compute the cache key for the current configuration.
    char cache_key[SHA256_HEX_LENGTH];
    if (compute_cache_key(cache_key, sizeof(cache_key)) != 0)
    {
        LOG_WARNING("Cannot save to cache: failed to compute cache key");
        return -1;
    }

    // Create the cache directory.
    if (mkdir_p(cache_dir) != 0)
    {
        LOG_WARNING("Cannot save to cache: failed to create cache directory");
        return -1;
    }

    // Construct the full cache file path.
    char cache_path[COMMAND_PATH_MAX_LENGTH];
    snprintf(
        cache_path, sizeof(cache_path), "%s/base-rootfs-%s.tar.gz",
        cache_dir, cache_key
    );

    // Quote the cache path for shell safety.
    char quoted_cache[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(cache_path, quoted_cache, sizeof(quoted_cache)) != 0)
    {
        return -1;
    }

    // Quote the rootfs directory for shell safety.
    char quoted_dir[COMMAND_QUOTED_MAX_LENGTH];
    if (shell_quote_path(rootfs_dir, quoted_dir, sizeof(quoted_dir)) != 0)
    {
        return -1;
    }

    LOG_INFO("Saving base rootfs to cache...");

    // Create tarball from rootfs contents.
    char command[COMMAND_MAX_LENGTH];
    snprintf(
        command, sizeof(command),
        "tar -czf %s -C %s .",
        quoted_cache, quoted_dir
    );
    if (run_command(command) != 0)
    {
        LOG_ERROR("Failed to create cache tarball");
        rm_file(cache_path);
        return -1;
    }

    LOG_INFO("Base rootfs cached at %s", cache_path);
    return 0;
}
