#include "secure_util.h"
#include "password_input.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_memory_wipe() {
    printf("[*] Testing sec_mem_wipe... ");
    char secret[] = "SensitiveData123";
    size_t len = strlen(secret);
    
    sec_mem_wipe(secret, len);
    
    int all_zero = 1;
    for (size_t i = 0; i < len; i++) {
        if (secret[i] != 0) all_zero = 0;
    }
    assert(all_zero && "Memory was not wiped!");
    printf("PASSED\n");
}

void test_constant_time_eq() {
    printf("[*] Testing sec_mem_equal... ");
    char a[] = "password";
    char b[] = "password";
    char c[] = "passward";
    
    assert(sec_mem_equal(a, b, 8) == 1 && "Equality failed");
    assert(sec_mem_equal(a, c, 8) == 0 && "Inequality failed");
    printf("PASSED\n");
}

void test_random_bytes() {
    printf("[*] Testing sec_rand_bytes... ");
    uint8_t buf1[16], buf2[16];
    
    assert(sec_rand_bytes(buf1, 16) == SEC_SUCCESS);
    assert(sec_rand_bytes(buf2, 16) == SEC_SUCCESS);
    
    assert(memcmp(buf1, buf2, 16) != 0 && "Random bytes produced identical buffers!");
    printf("PASSED\n");
}

int main() {
    printf("=== Starting Secure Utility Library Tests ===\n");
    test_memory_wipe();
    test_constant_time_eq();
    test_random_bytes();
    
    printf("\n[!] Manual Test: sec_input_password\n");
    char pass[32];
    int len = sec_input_password(pass, "Enter test password:", sizeof(pass));
    
    if (len >= 0) {
        printf("\nInput captured (%d chars). Wiping and exiting.\n", len);
        sec_mem_wipe(pass, sizeof(pass));
    } else {
        printf("\nInput cancelled or failed.\n");
    }
    
    printf("=== All Tests Completed ===\n");
    return 0;
}
