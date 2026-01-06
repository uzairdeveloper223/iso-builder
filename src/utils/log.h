#pragma once

/**
 * Prints an informational message to stdout.
 *
 * @param message The message to print.
 */
void log_info(const char *message);

/**
 * Prints a success message to stdout.
 *
 * @param message The message to print.
 */
void log_success(const char *message);

/**
 * Prints an error message to stderr.
 *
 * @param message The message to print.
 */
void log_error(const char *message);
