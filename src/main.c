/**
 * This code is responsible for providing the main entry point and command-line
 * interface for the LimeOS ISO builder.
 */

#include "all.h"

static void print_usage(const char *program_name)
{
    printf("Usage: %s <version> [options]\n", program_name);
    printf("\n");
    printf("Arguments:\n");
    printf("  <version>       Version tag to build (e.g., 1.0.0)\n");
    printf("\n");
    printf("Options:\n");
    printf("  --help          Show this help message\n");
}

// Phase 1: Prepare components from GitHub.
static int run_phase_1_preparation(
    const char *version, const char *components_dir
)
{
    if (init_fetch() != 0)
    {
        LOG_ERROR("Failed to initialize fetch module");
        return -1;
    }

    if (fetch_all_components(version, components_dir) != 0)
    {
        LOG_ERROR("Failed to fetch components");
        cleanup_fetch();
        return -1;
    }

    cleanup_fetch();
    LOG_INFO("Phase 1 complete: Preparation finished");
    return 0;
}

// Phase 2A: Create and package payload rootfs.
static int run_phase_2a_payload(
    const char *payload_rootfs_dir, const char *payload_tarball_path
)
{
    if (create_payload_rootfs(payload_rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create payload rootfs");
        return -1;
    }

    if (strip_payload_rootfs(payload_rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to strip payload rootfs");
        return -1;
    }

    if (package_payload_rootfs(payload_rootfs_dir, payload_tarball_path) != 0)
    {
        LOG_ERROR("Failed to package payload rootfs");
        return -1;
    }

    rm_rf(payload_rootfs_dir);
    LOG_INFO("Phase 2A complete: Payload rootfs packaged");
    return 0;
}

// Phase 2B: Create carrier rootfs with embedded payload.
static int run_phase_2b_carrier(
    const char *carrier_rootfs_dir, const char *payload_tarball_path,
    const char *components_dir
)
{
    if (create_carrier_rootfs(carrier_rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to create carrier rootfs");
        return -1;
    }

    if (embed_payload_rootfs(carrier_rootfs_dir, payload_tarball_path) != 0)
    {
        LOG_ERROR("Failed to embed payload rootfs");
        return -1;
    }

    if (install_carrier_components(carrier_rootfs_dir, components_dir) != 0)
    {
        LOG_ERROR("Failed to install components");
        return -1;
    }

    if (configure_carrier_init(carrier_rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure init");
        return -1;
    }

    if (bundle_packages(carrier_rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to bundle packages");
        return -1;
    }

    if (strip_carrier_rootfs(carrier_rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to strip carrier rootfs");
        return -1;
    }

    LOG_INFO("Phase 2B complete: Carrier rootfs created");
    return 0;
}

// Phase 3: Configure bootloaders.
static int run_phase_3_boot(const char *carrier_rootfs_dir)
{
    if (setup_grub(carrier_rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure GRUB");
        return -1;
    }

    if (setup_isolinux(carrier_rootfs_dir) != 0)
    {
        LOG_ERROR("Failed to configure isolinux");
        return -1;
    }

    if (setup_splash(carrier_rootfs_dir, CONFIG_SPLASH_LOGO_PATH) != 0)
    {
        LOG_ERROR("Failed to configure splash screen");
        return -1;
    }

    LOG_INFO("Phase 3 complete: Boot configured");
    return 0;
}

// Phase 4: Assemble final ISO image.
static int run_phase_4_iso(
    const char *carrier_rootfs_dir, const char *version
)
{
    char iso_output_path[256];
    snprintf(iso_output_path, sizeof(iso_output_path), CONFIG_ISO_FILENAME_PREFIX "-%s.iso", version);

    if (create_iso(carrier_rootfs_dir, iso_output_path) != 0)
    {
        LOG_ERROR("Failed to create ISO image");
        return -1;
    }

    LOG_INFO("Phase 4 complete: ISO assembled");
    LOG_INFO("Build complete! ISO available at: %s", iso_output_path);
    return 0;
}

int main(int argc, char *argv[])
{
    const char *version = NULL;
    char build_dir[COMMAND_PATH_MAX_LENGTH];
    char components_dir[COMMAND_PATH_MAX_LENGTH];
    char payload_rootfs_dir[COMMAND_PATH_MAX_LENGTH];
    char payload_tarball_path[COMMAND_PATH_MAX_LENGTH];
    char carrier_rootfs_dir[COMMAND_PATH_MAX_LENGTH];
    int exit_code = 0;

    // Verify the program is running as root.
    if (geteuid() != 0)
    {
        LOG_ERROR("This program must be run as root");
        return 1;
    }

    // Validate all required dependencies are available.
    if (validate_dependencies() != 0)
    {
        LOG_ERROR("Missing dependencies, cannot continue");
        return 1;
    }

    // Parse command-line arguments.
    int option;
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    while ((option = getopt_long(argc, argv, "h", long_options, NULL)) != -1)
    {
        switch (option)
        {
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Validate that a version argument was provided.
    if (optind >= argc)
    {
        LOG_ERROR("Missing required argument: version");
        print_usage(argv[0]);
        return 1;
    }

    // Extract and validate the version.
    version = argv[optind];
    if (validate_version(version) != 0)
    {
        LOG_ERROR("Invalid version format: %s (expected: X.Y.Z or vX.Y.Z)", version);
        return 1;
    }

    // Create a secure temporary build directory.
    if (create_secure_tmpdir(build_dir, sizeof(build_dir)) != 0)
    {
        LOG_ERROR("Failed to create secure build directory");
        return 1;
    }

    // Install signal handlers for graceful shutdown.
    install_signal_handlers(build_dir);

    // Construct derived paths.
    snprintf(components_dir, sizeof(components_dir), "%s/components", build_dir);
    snprintf(payload_rootfs_dir, sizeof(payload_rootfs_dir), "%s/payload-rootfs", build_dir);
    snprintf(payload_tarball_path, sizeof(payload_tarball_path), "%s/rootfs.tar.gz", build_dir);
    snprintf(carrier_rootfs_dir, sizeof(carrier_rootfs_dir), "%s/carrier-rootfs", build_dir);

    LOG_INFO("Building ISO for version %s", version);

    // Phase 1: Preparation.
    if (run_phase_1_preparation(version, components_dir) != 0)
    {
        exit_code = 1;
        goto cleanup;
    }
    if (check_interrupted()) return 130;

    // Phase 2A: Create payload rootfs.
    if (run_phase_2a_payload(payload_rootfs_dir, payload_tarball_path) != 0)
    {
        exit_code = 1;
        goto cleanup;
    }
    if (check_interrupted()) return 130;

    // Phase 2B: Create carrier rootfs.
    if (run_phase_2b_carrier(carrier_rootfs_dir, payload_tarball_path, components_dir) != 0)
    {
        exit_code = 1;
        goto cleanup;
    }
    if (check_interrupted()) return 130;

    // Phase 3: Configure bootloaders.
    if (run_phase_3_boot(carrier_rootfs_dir) != 0)
    {
        exit_code = 1;
        goto cleanup;
    }
    if (check_interrupted()) return 130;

    // Phase 4: Assemble ISO.
    if (run_phase_4_iso(carrier_rootfs_dir, version) != 0)
    {
        exit_code = 1;
        goto cleanup;
    }

cleanup:
    rm_rf(build_dir);
    clear_cleanup_dir();
    return exit_code;
}
