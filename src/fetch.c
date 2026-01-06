/**
 * This code is responsible for downloading LimeOS component binaries from
 * GitHub releases using libcurl.
 */

#include "all.h"

/** The GitHub organization hosting LimeOS component repositories. */
#define FETCH_GITHUB_ORG "limeos-org"

/** The maximum length for constructed URL strings. */
#define FETCH_MAX_URL_LENGTH 512

/** The maximum length for file path strings. */
#define FETCH_MAX_PATH_LENGTH 256

/** The maximum length for log message strings. */
#define FETCH_MAX_MESSAGE_LENGTH 256

int fetch_init(void)
{
    // Initialize the curl library globally.
    CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (result != CURLE_OK)
    {
        return -1;
    }

    return 0;
}

void fetch_cleanup(void)
{
    // Clean up the curl library globally.
    curl_global_cleanup();
}

static size_t handle_write_data(
    void *data,
    size_t size,
    size_t count,
    void *stream
)
{
    return fwrite(data, size, count, (FILE *)stream);
}

int fetch_component(
    const char *name,
    const char *version,
    const char *output_directory
)
{
    CURL *curl;
    CURLcode result;
    FILE *output_file;
    char url[FETCH_MAX_URL_LENGTH];
    char output_path[FETCH_MAX_PATH_LENGTH];
    char message[FETCH_MAX_MESSAGE_LENGTH];

    // Construct the GitHub release download URL.
    snprintf(
        url, sizeof(url),
        "https://github.com/%s/%s/releases/download/%s/%s",
        FETCH_GITHUB_ORG, name, version, name
    );

    // Construct the local output file path.
    snprintf(output_path, sizeof(output_path), "%s/%s", output_directory, name);

    // Log the fetch operation.
    snprintf(message, sizeof(message), "Fetching %s %s", name, version);
    log_info(message);

    // Create the output directory if it does not exist.
    mkdir(output_directory, 0755);

    // Open the output file for writing.
    output_file = fopen(output_path, "wb");
    if (!output_file)
    {
        snprintf(
            message, sizeof(message), "Failed to create file: %s",
            output_path
        );
        log_error(message);
        return -1;
    }

    // Initialize the curl session.
    curl = curl_easy_init();
    if (!curl)
    {
        log_error("Failed to initialize curl");
        fclose(output_file);
        return -1;
    }

    // Configure curl options for the download.
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_file);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "limeos-iso-builder/1.0");

    // Perform the download.
    result = curl_easy_perform(curl);

    // Get the HTTP response code.
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    // Clean up curl resources.
    curl_easy_cleanup(curl);
    fclose(output_file);

    // Check for curl errors.
    if (result != CURLE_OK)
    {
        remove(output_path);
        snprintf(
            message, sizeof(message), "Download failed: %s",
            curl_easy_strerror(result)
        );
        log_error(message);
        return -1;
    }

    // Check for HTTP errors.
    if (http_code != 200)
    {
        remove(output_path);
        snprintf(
            message, sizeof(message), "Download failed: HTTP %ld",
            http_code
        );
        log_error(message);
        return -1;
    }

    // Log successful download.
    snprintf(message, sizeof(message), "Downloaded %s", name);
    log_success(message);

    return 0;
}

int fetch_all_components(const char *version, const char *output_directory)
{
    const char *components[] = {
        "window-manager",
        "display-manager",
        "installation-wizard"
    };
    int component_count = sizeof(components) / sizeof(components[0]);

    // Log the start of component fetching.
    log_info("Fetching LimeOS components...");

    // Fetch each component sequentially.
    for (int i = 0; i < component_count; i++)
    {
        if (fetch_component(components[i], version, output_directory) != 0)
        {
            return -1;
        }
    }

    // Log successful completion.
    log_success("All components fetched successfully");

    return 0;
}
