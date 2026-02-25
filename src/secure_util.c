#include "secure_util.h"

#include "secure_util.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <bcrypt.h>
    #pragma comment(lib, "bcrypt.lib")
#else
    #define _GNU_SOURCE
    #include <unistd.h>
    #include <sys/mman.h>
    #if defined(__linux__)
        #include <sys/random.h>
    #endif
#endif

/* --- Feature Detection --- */

#if defined(__STDC_LIB_EXT1__) && (__STDC_LIB_EXT1__ >= 201112L)
    #define HAVE_MEMSET_S
#endif

/* Define explicit_bzero for BSDs/glibc 2.25+ */
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || \
    defined(__APPLE__) || (defined(__GLIBC__) && (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25)))
    #define HAVE_EXPLICIT_BZERO
#endif

#define GETENTROPY_MAX_CHUNK 256

/* -------------------------------------------------- */
/* Random Number Generation                           */
/* -------------------------------------------------- */

int sec_rand_bytes(uint8_t *buf, size_t len) {
    if (!buf || len == 0) return SEC_ERR_INVALID_ARGS;

#if defined(_WIN32)
    if (len > ULONG_MAX) return SEC_ERR_LIMIT_EXCEEDED;
    return BCRYPT_SUCCESS(BCryptGenRandom(NULL, buf, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG)) 
           ? SEC_SUCCESS : SEC_ERR_SYS;

#elif defined(__linux__)
    size_t off = 0;
    while (off < len) {
        ssize_t r = getrandom(buf + off, len - off, 0);
        if (r < 0) {
            if (errno == EINTR) continue;
            return SEC_ERR_SYS; 
        }
        off += (size_t)r;
    }
    return SEC_SUCCESS;

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
    size_t off = 0;
    while (off < len) {
        size_t chunk = (len - off > GETENTROPY_MAX_CHUNK) ? GETENTROPY_MAX_CHUNK : (len - off);
        if (getentropy(buf + off, chunk) != 0) return SEC_ERR_SYS;
        off += chunk;
    }
    return SEC_SUCCESS;

#else
    FILE *f = fopen("/dev/urandom", "rb");
    if (!f) return SEC_ERR_SYS;
    int res = (fread(buf, 1, len, f) == len) ? SEC_SUCCESS : SEC_ERR_SYS;
    fclose(f);
    return res;
#endif
}

/* -------------------------------------------------- */
/* Memory Security Operations                         */
/* -------------------------------------------------- */

void sec_mem_wipe(void *ptr, size_t len) {
    if (!ptr || len == 0) return;

#if defined(_WIN32)
    SecureZeroMemory(ptr, len);
#elif defined(HAVE_MEMSET_S)
    memset_s(ptr, len, 0, len);
#elif defined(HAVE_EXPLICIT_BZERO)
    explicit_bzero(ptr, len);
#else
    volatile unsigned char *p = (volatile unsigned char *)ptr;
    while (len--) *p++ = 0;
    __asm__ __volatile__("" ::: "memory");
#endif
}

int sec_mem_equal(const void *a, const void *b, size_t len) {
    if (!a || !b) return 0;
    
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    uint8_t diff = 0;

    for (size_t i = 0; i < len; i++) {
        diff |= (pa[i] ^ pb[i]);
    }

    /* Returns 1 if identical, 0 if different */
    return (diff == 0); 
}

int sec_mem_lock(void *ptr, size_t len) {
    if (!ptr || len == 0) return SEC_ERR_INVALID_ARGS;
#if defined(_WIN32)
    return VirtualLock(ptr, len) ? SEC_SUCCESS : SEC_ERR_SYS;
#else
    return (mlock(ptr, len) == 0) ? SEC_SUCCESS : SEC_ERR_SYS;
#endif
}

int sec_mem_unlock(void *ptr, size_t len) {
    if (!ptr || len == 0) return SEC_ERR_INVALID_ARGS;
    
    sec_mem_wipe(ptr, len);
    
#if defined(_WIN32)
    return VirtualUnlock(ptr, len) ? SEC_SUCCESS : SEC_ERR_SYS;
#else
    return (munlock(ptr, len) == 0) ? SEC_SUCCESS : SEC_ERR_SYS;
#endif
}
