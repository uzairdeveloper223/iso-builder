/**
 * This code is responsible for providing the main entry point and command-line
 * interface for the LimeOS ISO builder.
 */

#include "all.h"

/** The directory where LimeOS component binaries are downloaded. */
#define COMPONENTS_DIRECTORY "/tmp/limeos-components"

/** The maximum length for log message strings. */
#define MAX_MESSAGE_LENGTH 256

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

int main(int argc, char *argv[])
{
    const char *version = NULL;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Parse command-line arguments.
    int option;
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
        log_error("Missing required argument: version");
        print_usage(argv[0]);
        return 1;
    }

    // Extract the version from positional arguments.
    version = argv[optind];

    // Print the banner.
    printf("LimeOS ISO Builder v%s\n\n", version);

    // Initialize the fetch module.
    if (fetch_init() != 0)
    {
        log_error("Failed to initialize fetch module");
        return 1;
    }

    // Log the build operation.
    char message[MAX_MESSAGE_LENGTH];
    snprintf(message, sizeof(message), "Building ISO for version %s", version);
    log_info(message);

    // Fetch all required components.
    if (fetch_all_components(version, COMPONENTS_DIRECTORY) != 0)
    {
        log_error("Failed to fetch components");
        fetch_cleanup();
        return 1;
    }

    // Log successful completion of phase 1.
    log_success("Phase 1 complete: Components fetched");

    // Clean up the fetch module.
    fetch_cleanup();

    return 0;
}
