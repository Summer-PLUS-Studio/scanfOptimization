
/**
 * @file test_yscanf.c
 * @brief Comprehensive test suite for YScanF optimized library
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <float.h>

#include "yscanf_optimized.h"

/* Test result tracking */
static int tests_run = 0;
static int tests_passed = 0;

/* Test macros */
#define TEST(name) \
    do { \
        printf("Testing %s... ", name); \
        fflush(stdout); \
        tests_run++; \
    } while(0)

#define PASS() \
    do { \
        printf("PASSED\n"); \
        tests_passed++; \
    } while(0)

#define FAIL(msg) \
    do { \
        printf("FAILED: %s\n", msg); \
        exit(1); \
    } while(0)

/* Helper function to create test input */
static void create_test_input(const char *input) {
    FILE *fp = fopen("test_input.txt", "w");
    if (!fp) {
        perror("Failed to create test input");
        exit(1);
    }
    fprintf(fp, "%s", input);
    fclose(fp);
    
    /* Redirect stdin to test file */
    freopen("test_input.txt", "r", stdin);
}

/* Test basic integer reading */
void test_basic_integers(void) {
    TEST("basic integers");
    
    create_test_input("42 -123 0 2147483647 -2147483648");
    
    int a, b, c, d, e;
    int ret = yscanf("%d %d %d %d %d", &a, &b, &c, &d, &e);
    
    if (ret != 5) FAIL("Failed to read 5 integers");
    if (a != 42) FAIL("First integer mismatch");
    if (b != -123) FAIL("Second integer mismatch");
    if (c != 0) FAIL("Third integer mismatch");
    if (d != INT_MAX) FAIL("Fourth integer mismatch");
    if (e != INT_MIN) FAIL("Fifth integer mismatch");
    
    PASS();
}

/* Test unsigned integers */
void test_unsigned_integers(void) {
    TEST("unsigned integers");
    
    create_test_input("123 0 4294967295 2147483648");
    
    unsigned a, b, c, d;
    int ret = yscanf("%u %u %u %u", &a, &b, &c, &d);
    
    if (ret != 4) FAIL("Failed to read 4 unsigned integers");
    if (a != 123) FAIL("First unsigned integer mismatch");
    if (b != 0) FAIL("Second unsigned integer mismatch");
    if (c != UINT32_MAX) FAIL("Third unsigned integer mismatch");
    if (d != 2147483648U) FAIL("Fourth unsigned integer mismatch");
    
    PASS();
}

/* Test long long integers */
void test_long_long(void) {
    TEST("long long integers");
    
    create_test_input("9223372036854775807 -9223372036854775808 0");
    
    long long a, b, c;
    int ret = yscanf("%lld %lld %lld", &a, &b, &c);
    
    if (ret != 3) FAIL("Failed to read 3 long long integers");
    if (a != LLONG_MAX) FAIL("First long long mismatch");
    if (b != LLONG_MIN) FAIL("Second long long mismatch");
    if (c != 0) FAIL("Third long long mismatch");
    
    PASS();
}

/* Test unsigned long long */
void test_unsigned_long_long(void) {
    TEST("unsigned long long");
    
    create_test_input("18446744073709551615 0 9223372036854775808");
    
    unsigned long long a, b, c;
    int ret = yscanf("%llu %llu %llu", &a, &b, &c);
    
    if (ret != 3) FAIL("Failed to read 3 unsigned long long");
    if (a != ULLONG_MAX) FAIL("First unsigned long long mismatch");
    if (b != 0) FAIL("Second unsigned long long mismatch");
    if (c != 9223372036854775808ULL) FAIL("Third unsigned long long mismatch");
    
    PASS();
}

/* Test floating point numbers */
void test_floating_point(void) {
    TEST("floating point");
    
    create_test_input("3.14 -2.718 0.0 1.23e4 -5.67e-8");
    
    double a, b, c, d, e;
    int ret = yscanf("%lf %lf %lf %lf %lf", &a, &b, &c, &d, &e);
    
    if (ret != 5) FAIL("Failed to read 5 floating point numbers");
    if (a < 3.139 || a > 3.141) FAIL("First float mismatch");
    if (b < -2.719 || b > -2.717) FAIL("Second float mismatch");
    if (c < -0.001 || c > 0.001) FAIL("Third float mismatch");
    if (d < 12299 || d > 12301) FAIL("Fourth float mismatch");
    if (e > -5.66e-8 || e < -5.68e-8) FAIL("Fifth float mismatch");
    
    PASS();
}

/* Test string reading */
void test_string_reading(void) {
    TEST("string reading");
    
    create_test_input("hello world test123");
    
    char a[100], b[100], c[100];
    int ret = yscanf("%s %s %s", a, b, c);
    
    if (ret != 3) FAIL("Failed to read 3 strings");
    if (strcmp(a, "hello") != 0) FAIL("First string mismatch");
    if (strcmp(b, "world") != 0) FAIL("Second string mismatch");
    if (strcmp(c, "test123") != 0) FAIL("Third string mismatch");
    
    PASS();
}

/* Test character reading */
void test_character_reading(void) {
    TEST("character reading");
    
    create_test_input("ABC 123");
    
    char a, b, c, d, e, f, g;
    int ret = yscanf("%c%c%c %c%c%c", &a, &b, &c, &d, &e, &f);
    
    if (ret != 6) FAIL("Failed to read 6 characters");
    if (a != 'A') FAIL("First character mismatch");
    if (b != 'B') FAIL("Second character mismatch");
    if (c != 'C') FAIL("Third character mismatch");
    if (d != ' ') FAIL("Fourth character mismatch");
    if (e != '1') FAIL("Fifth character mismatch");
    if (f != '2') FAIL("Sixth character mismatch");
    
    PASS();
}

/* Test whitespace handling */
void test_whitespace_handling(void) {
    TEST("whitespace handling");
    
    create_test_input("  42  \t\n  3.14  \t  hello  ");
    
    int a;
    double b;
    char c[100];
    int ret = yscanf("%d %lf %s", &a, &b, c);
    
    if (ret != 3) FAIL("Failed to read with whitespace");
    if (a != 42) FAIL("Integer mismatch with whitespace");
    if (b < 3.139 || b > 3.141) FAIL("Float mismatch with whitespace");
    if (strcmp(c, "hello") != 0) FAIL("String mismatch with whitespace");
    
    PASS();
}

/* Test overflow handling */
void test_overflow_handling(void) {
    TEST("overflow handling");
    
    /* Test integer overflow */
    create_test_input("999999999999999999999999999 4294967296");
    
    int a;
    unsigned b;
    yscanf("%d %u", &a, &b);
    
    if (a != INT32_MAX) FAIL("Integer overflow not handled correctly");
    if (b != UINT32_MAX) FAIL("Unsigned overflow not handled correctly");
    
    PASS();
}

/* Test EOF handling */
void test_eof_handling(void) {
    TEST("EOF handling");
    
    create_test_input("42");
    
    int a, b;
    int ret = yscanf("%d %d", &a, &b);
    
    if (ret != 1) FAIL("EOF not handled correctly");
    if (a != 42) FAIL("Value not read correctly before EOF");
    
    PASS();
}

/* Test mixed types */
void test_mixed_types(void) {
    TEST("mixed types");
    
    create_test_input("42 3.14 hello 123 -45");
    
    int a, d, e;
    double b;
    char c[100];
    int ret = yscanf("%d %lf %s %d %d", &a, &b, c, &d, &e);
    
    if (ret != 5) FAIL("Failed to read mixed types");
    if (a != 42) FAIL("First integer mismatch");
    if (b < 3.139 || b > 3.141) FAIL("Float mismatch");
    if (strcmp(c, "hello") != 0) FAIL("String mismatch");
    if (d != 123) FAIL("Third integer mismatch");
    if (e != -45) FAIL("Fourth integer mismatch");
    
    PASS();
}

/* Performance test */
void test_performance(void) {
    TEST("performance");
    
    /* Create large input file */
    FILE *fp = fopen("perf_test.txt", "w");
    if (!fp) {
        FAIL("Failed to create performance test file");
    }
    
    /* Generate 100,000 integers */
    for (int i = 0; i < 100000; i++) {
        fprintf(fp, "%d ", i);
    }
    fclose(fp);
    
    /* Redirect stdin */
    freopen("perf_test.txt", "r", stdin);
    
    /* Time the parsing */
    clock_t start = clock();
    
    int sum = 0;
    int val;
    while (yscanf("%d", &val) == 1) {
        sum += val;
    }
    
    clock_t end = clock();
    double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("\n    Parsed 100,000 integers in %.3f seconds\n", cpu_time);
    printf("    Sum: %d (verification: %s)\n", sum, 
           (sum == 4999950000LL) ? "CORRECT" : "INCORRECT");
    
    if (sum != 4999950000LL) {
        FAIL("Performance test sum mismatch");
    }
    
    PASS();
}

/* Main test runner */
int main(void) {
    printf("=== YScanF Optimized Test Suite ===\n\n");
    
    /* Run all tests */
    test_basic_integers();
    test_unsigned_integers();
    test_long_long();
    test_unsigned_long_long();
    test_floating_point();
    test_string_reading();
    test_character_reading();
    test_whitespace_handling();
    test_overflow_handling();
    test_eof_handling();
    test_mixed_types();
    test_performance();
    
    /* Summary */
    printf("\n=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Success rate: %.1f%%\n", 
           tests_run > 0 ? (100.0 * tests_passed / tests_run) : 0.0);
    
    if (tests_passed == tests_run) {
        printf("\n🎉 All tests passed!\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed!\n");
        return 1;
    }
}
