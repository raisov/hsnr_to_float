//
//  hsnr_to_float tests.
//  main.c
//
//  Created by bp on 14/04/2019.
//
#include <stdio.h>
#include <math.h>
#include "hsnr_to_float.h"

int test_result(int result) {
    printf("[%s]\n", result ? "PASSED" : " F A I L E D ");
    return result;
}

int main(int argc, const char * argv[]) {
    int success = 1;

    // Test entire input range for matching
    // conversion results of values with opposite signs.
    int checked = 0;
    int failures = 0;
    printf("Complementation test for subnormal values \n");
    for (int j = 0; j != 0x800; j++) {
        uint16_t candidate = j;
        uint16_t opposite = -j & 0x0fff;
        success = hsnr_to_float(candidate) == -hsnr_to_float(opposite);
        if (!success) {
            printf("Complement test for %04hx and %04hx ", candidate, opposite);
            test_result(success);
            failures += 1;
        }
        checked += 1;
    }
    printf("%d values checked, %d failures ", checked, failures);
    success = test_result(failures == 0);
    printf("\n");

    checked = 0;
    failures = 0;
    printf("Complementation test for normal values \n");
    uint16_t exponent = 0xf000;
    while (exponent != 0) {
    for (int j = 0; j != 0x800; j++) {
        uint16_t candidate = j | exponent;
        uint16_t opposite = ((-j | 0x0800) & 0x0fff) | (j == 0 ? exponent - 0x1000 : exponent);
        success = (hsnr_to_float(candidate) == -hsnr_to_float(opposite));
        if (!success) {
            printf("Complementation test for %04hx and %04hx ", candidate, opposite);
            test_result(success);
            failures += 1;
        }
        checked += 1;
    }
        exponent -= 0x1000;
    }
    printf("%d values checked, %d failures ", checked, failures);
    success &= test_result(failures == 0);
    printf("\n");

    // Some values for testing.
    const struct {
        uint16_t hsnr;
        float value;
    } test_data[] = {
        {0xf7ff, 4095.0 * pow(2.0, -10.0)},
        {0xf7fe, 4094.0 * pow(2.0, -10.0)},
        {0xf701, 3841.0 * pow(2.0, -10.0)},
        {0xf700, 3.75},
        {0xf600, 3.5},
        {0xf500, 3.25},
        {0xf491, 3.1416015625},
        {0xf400, 3.0},
        {0xf2e0, 2.71875},
        {0xf000, 2.0},
        {0xe600, 1.75},
        {0xe400, 1.5},
        {0xe001, 2049.0 * pow(2.0, -11.0)},
        {0xe000, 1.0},
        {0xd7ff, 4095.0 * pow(2.0, -12.0)},
        {0x2700, 30 * pow(2.0, -16.0)},
        {0x17ff, 4095.0 * pow(2.0, -24.0)},
        {0x17fe, 2047.0 * pow(2.0, -23.0)},
        {0x1700, 15.0 * pow(2.0, -16.0)},
        {0x1001, 2049.0 * pow(2.0, -24.0)},
        {0x1000, 1.0 * pow(2.0, -13.0)},
        {0x07ff, 2047.0 * pow(2.0, -24.0)},
        {0x0003, 3.0 * pow(2.0, -24.0)},
        {0x0002, 1.0 * pow(2.0, -23.0)},
        {0x0001, 1.0 * pow(2.0, -24.0)},
        {0x0000, 0.0},
        {0x0fff, -1.0 * pow(2.0, -24.0)},
        {0x0ffe, -1.0 * pow(2.0, -23.0)},
        {0x0ffd, -3.0 * pow(2.0, -24.0)},
        {0x0801, -2047.0 * pow(2.0, -24.0) },
        {0x0800, -1.0 * pow(2.0, -13.0) },
        {0xd801, -4095.0 * pow(2.0, -12.0)},
        {0xd800, -1.0},
        {0xefff, -2049.0 * pow(2.0, -11.0)},
        {0xeffe, -1025.0 * pow(2.0, -10.0)},
        {0xec00, -1.5},
        {0xe802, -4094.0 * pow(2.0, -11.0)},
        {0xe801, -4095.0 * pow(2.0, -11.0)},
        {0xe800, -2.0},
        {0xffff, -2049.0 * pow(2.0, -10.0)},
        {0xfffe, -1025.0 * pow(2.0, -9.0)},
        {0xfc00, -3.0},
        {0xf802, -2047.0 * pow(2.0, -9.0)},
        {0xf801, -4095.0 * pow(2.0, -10.0)},
        {0xf800, -4.0}
    };
    // Check this values.
    int test_count = sizeof(test_data) / sizeof(test_data[0]);
    while (test_count != 0) {
        test_count -= 1;
        uint16_t hsnr = test_data[test_count].hsnr;
        float value = hsnr_to_float(hsnr);
        printf("High SNR: %04hx == %.10e ", hsnr, value);
        success &= test_result(value == test_data[test_count].value);
    }
    printf("\n");

    // Check some boundary conditions.
    printf("boundary 1 ");
    success &= test_result(hsnr_to_float(0x1000) -
                           hsnr_to_float(0x07ff) ==
                           hsnr_to_float(1));
    printf("boundary 2 ");
    success &= test_result(hsnr_to_float(0x1001) -
                           hsnr_to_float(0x1000) ==
                           hsnr_to_float(1));
    printf("boundary 3 ");
    success &= test_result(hsnr_to_float(0x1400) -
                           hsnr_to_float(0x1000) ==
                           hsnr_to_float(0x0400));
    printf("boundary 4 ");
    success &= test_result(hsnr_to_float(0x17ff) -
                           hsnr_to_float(0x1000) ==
                           hsnr_to_float(0x07ff));

    return !success;
}
