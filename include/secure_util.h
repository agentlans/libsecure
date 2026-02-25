#ifndef SECURE_UTIL_H
#define SECURE_UTIL_H

#include <stdint.h>
#include <stddef.h>

/* Return Codes */
typedef enum {
    SEC_SUCCESS            =  0,
    SEC_ERR_SYS            = -1,  /* OS-level failure */
    SEC_ERR_INVALID_ARGS   = -2,  /* NULL pointers or 0 length */
    SEC_ERR_LIMIT_EXCEEDED = -3   /* Requested length too large */
} sec_status_t;

/* Public API */
int  sec_rand_bytes(uint8_t *buf, size_t len);
void sec_mem_wipe(void *ptr, size_t len);
int  sec_mem_equal(const void *a, const void *b, size_t len);
int  sec_mem_lock(void *ptr, size_t len);
int  sec_mem_unlock(void *ptr, size_t len);

#endif
