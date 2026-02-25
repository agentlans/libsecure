# libsecure

A lightweight, cross-platform C library providing essential security primitives for memory management, constant-time operations, and secure user input.

## 🚀 Features

* **Secure Memory Wiping**: Compiler-resistant zeroing of sensitive data using platform-specific primitives (`SecureZeroMemory`, `memset_s`, or `explicit_bzero`).
* **CSPRNG**: Easy access to system-preferred cryptographically secure random number generators.
* **Constant-Time Comparison**: Comparison functions designed to prevent timing attacks.
* **Memory Locking**: Prevent sensitive data from being swapped to disk (using `mlock` or `VirtualLock`).
* **Secure Password Input**: Terminal-based password entry with masking, visibility toggling (Ctrl+V), and signal handling. (POSIX only)

## 📁 Project Structure

```text
libsecure/
├── include/              # Public headers (.h)
├── src/                  # Implementation files (.c)
├── tests/                # Unit tests & examples
├── bin/                  # Compiled libraries (.a, .so)
└── Makefile              # Cross-platform build script

```

## 🛠 Building the Library

### Prerequisites

* A C99 compatible compiler (GCC, Clang, or MSVC with MinGW).
* `make` build utility.

### Compilation

To build both the static and dynamic libraries, simply run:

```bash
make

```

To build and run the test suite:

```bash
make test

```

### Cleaning Up

```bash
make clean

```

## 📖 API Reference

### Randomness

`int sec_rand_bytes(uint8_t *buf, size_t len)`

Fills a buffer with secure random bytes. Returns `SEC_SUCCESS` (0) on success.

### Memory Utilities

```c
void sec_mem_wipe(void *ptr, size_t len)
```

Securely erases memory. Use this for keys and passwords before they go out of scope.

```c
int sec_mem_equal(const void *a, const void *b, size_t len)
```

Compares two memory blocks in constant time. Returns `1` if equal, `0` if not.

```c
int sec_mem_lock(void *ptr, size_t len)
```

Locks a memory range into RAM, preventing it from being written to the swap file/disk.

### User Input

```c
int sec_input_password(char* password, const char *prompt, size_t max_len)
```

Prompts for a password. Masks input with `*`.

* **Ctrl+V**: Toggle visibility.
* **Backspace/Arrows**: Standard editing support.
* **Returns**: Length of input on success, `SEC_ERR_SYS` on failure.

## 💻 Integration Example

```c
#include "secure_util.h"
#include "password_input.h"
#include <stdio.h>

int main() {
    char password[64];
    
    // 1. Securely grab input
    if (sec_input_password(password, "Enter Key:", sizeof(password)) > 0) {
        
        // 2. Do something sensitive...
        printf("\nKey accepted.\n");

        // 3. WIPE when finished!
        sec_mem_wipe(password, sizeof(password));
    }

    return 0;
}

```

## ⚖ Licence

This project is provided under the MIT Licence. Use it at your own risk. When dealing with high-security requirements, always undergo a professional audit.

