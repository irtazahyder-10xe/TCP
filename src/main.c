#include <stdint.h>

#include "mmio.h"
#include "uart.h"

int main() {

    init_uart();
    uint8_t exit_status = 0;
    /* Testing output monitor */
    for (int i = -1; i < 2; i++) {
        int *p = &i;
        printf("String: %s\nDecimal: %d\nHex: 0x%x\nUnsigned: %u\nPointer: 0x%p (has value %d)\n",
               "Hello World",
               i,
               i,
               i,
               p, *p);
        printf("---------------\n");
    }

    printf("\n========= Memory Read/Write Test =========\n");
    uint64_t value = 0xdeadcafebeefface;
    uint64_t *addr = &value;
    const uint8_t tests = 4;
    uint8_t tests_passed = 0;

    printf("========= Byte Read/Write Test =========\n");
    mmio_write_byte((uintptr_t) addr, 0, 0x07);
    if (mmio_read_byte((uintptr_t) addr, 0) != 0x07) {
        printf("FAILED: 0x%x != 0x%x\n",
               value & 0xFF,
               0x07);
        exit_status = 1;
    } else {
        printf("PASSED\n");
        tests_passed++;
    }

    printf("========= Short Read/Write Test =========\n");
    mmio_write_short((uintptr_t) addr, 2, 0xdead);
    if (mmio_read_short((uintptr_t) addr, 2) != 0xdead) {
        printf("FAILED: 0x%x != 0x%x\n",
               (value & 0xFFFF0000U) >> 16,
               0xdead);
        exit_status = 1;
    } else {
        printf("PASSED\n");
        tests_passed++;
    }

    printf("========= Word read/Write Test =========\n");
    mmio_write_word((uintptr_t) addr, 4, 0xcafebeef);
    if (mmio_read_word((uintptr_t) addr, 4) != 0xcafebeefUL) {
        printf("FAILED: 0x%x != 0x%x\n",
               (value & 0xFFFFFFFF00000000U) >> 32,
               0xcafebeefUL);
        exit_status = 1;
    } else {
        printf("PASSED\n");
        tests_passed++;
    }


    printf("========= Double Read/Write Test =========\n");
    mmio_write_double((uintptr_t) addr, 0, 0xfedcba9876543210);
    if (mmio_read_double((uintptr_t) addr, 0) != 0xfedcba9876543210) {
        printf("FAILED: 0x%x != 0x%x\n",
               value,
               0xfedcba9876543210);
        exit_status = 1;
    } else {
        printf("PASSED\n");
        tests_passed++;
    }

    printf("Tests %d/%d passed", tests_passed, tests);

    return exit_status;
}
