#pragma once

#include <curl/curl.h>
#include <errno.h>
#include <getopt.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/** The maximum length for constructed shell command strings. */
#define MAX_COMMAND_LENGTH 512

/** The maximum length for file path strings. */
#define MAX_PATH_LENGTH 256

/** The maximum length for URL strings. */
#define MAX_URL_LENGTH 512

/** The list of LimeOS component binaries required for the ISO. */
static const char *COMPONENTS[] = {
    "window-manager",
    "display-manager",
    "installation-wizard"
};

/** The number of components in the COMPONENTS array. */
#define COMPONENTS_COUNT (int)(sizeof(COMPONENTS) / sizeof(COMPONENTS[0]))

#include "collector.h"
#include "version.h"
#include "rootfs/components.h"
#include "rootfs/create.h"
#include "rootfs/init.h"
#include "utils/command.h"
#include "utils/fs.h"
#include "utils/log.h"
#include "utils/validate.h"
