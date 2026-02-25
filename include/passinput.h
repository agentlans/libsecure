#ifndef PASSINPUT_H
#define PASSINPUT_H

#include "secure_util.h" /* For sec_status_t and size_t */

/**
 * @brief Reads a password from stdin while masking characters.
 * * Supports backspace, arrow keys for navigation, and CTRL+V to toggle visibility.
 * The terminal is automatically restored to its original state on exit or interrupt.
 *
 * @param password Pointer to the buffer where the password will be stored.
 * @param prompt   The text shown to the user (e.g., "Enter Password:").
 * @param max_len  The size of the password buffer.
 * * @return The length of the password on success, or a negative sec_status_t on failure.
 */
int sec_input_password(char* password, const char *prompt, size_t max_len);

#endif /* PASSINPUT_H */
