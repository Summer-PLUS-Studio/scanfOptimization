
/**
 * @file benchmark.c
 * @brief Performance benchmark comparing original vs optimized YScanF
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/* Include both versions for comparison */
#include "yscanf.h"
#include "yscanf_optimized.h"

#define BENCHMARK_SIZE 1000000
#define ITERATIONS 10

/* Timing utilities */
static double get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

/* Generate test data */
static void generate_test_data(const char *filename, int size) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to create test data");
        exit(1);
    }
    
    /* Generate mixed data: integers, floats, strings */
    for (int i = 0; i < size; i++) {
        fprintf(fp, "%d %.6f string%d ", i, (double)i * 1.5, i);
        if (i % 10 == 0) fprintf(fp, "\n");
    }
    
    fclose(fp);
}

/* Benchmark original version */
static double benchmark_original(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open test data");
        exit(1);
    }
    
    /* Redirect stdin */
    stdin = fp;
    
    double start = get_time();
    
    /* Reset buffer state */
    extern char ybuf[], *yptr, *yend;
    yptr = ybuf;
    yend = ybuf;
    
    int sum = 0;
    double sum_d = 0;
    char str[100];
    
    /* Parse data */
    for (int i = 0; i < BENCHMARK_SIZE; i++) {
        int a;
        double b;
        if (yscanf("%d %lf %s", &a, &b, str) != 3) break;
        sum += a;
        sum_d += b;
    }
    
    double end = get_time();
    fclose(fp);
    
    printf("Original version: sum=%d, sum_d=%.2f\n", sum, sum_d);
    return end - start;
}

/* Benchmark optimized version */
static double benchmark_optimized(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open test data");
        exit(1);
    }
    
    /* Redirect stdin */
    stdin = fp;
    
    double start = get_time();
    
    int sum = 0;
    double sum_d = 0;
    char str[100];
    
    /* Parse data */
    for (int i = 0; i < BENCHMARK_SIZE; i++) {
        int a;
        double b;
        if (yscanf("%d %lf %s", &a, &b, str) != 3) break;
        sum += a;
        sum_d += b;
    }
    
    double end = get_time();
    fclose(fp);
    
    printf("Optimized version: sum=%d, sum_d=%.2f\n", sum, sum_d);
    return end - start;
}

/* Benchmark specific operations */
static void benchmark_operations(void) {
    printf("\n=== Operation-specific benchmarks ===\n");
    
    /* Integer parsing benchmark */
    {
        FILE *fp = fopen("int_test.txt", "w");
        for (int i = 0; i < 100000; i++) {
            fprintf(fp, "%d ", i);
        }
        fclose(fp);
        
        FILE *in = fopen("int_test.txt", "r");
        stdin = in;
        
        double start = get_time();
        int val, count = 0;
        while (yscanf("%d", &val) == 1) count++;
        double end = get_time();
        
        printf("Integer parsing: %d values in %.3f seconds (%.0f values/sec)\n", 
               count, end - start, count / (end - start));
        fclose(in);
    }
    
    /* Float parsing benchmark */
    {
        FILE *fp = fopen("float_test.txt", "w");
        for (int i = 0; i < 100000; i++) {
            fprintf(fp, "%.6f ", (double)i * 1.234567);
        }
        fclose(fp);
        
        FILE *in = fopen("float_test.txt", "r");
        stdin = in;
        
        double start = get_time();
        double val;
        int count = 0;
        while (yscanf("%lf", &val) == 1) count++;
        double end = get_time();
        
        printf("Float parsing: %d values in %.3f seconds (%.0f values/sec)\n", 
               count, end - start, count / (end - start));
        fclose(in);
    }
    
    /* String parsing benchmark */
    {
        FILE *fp = fopen("string_test.txt", "w");
        for (int i = 0; i < 100000; i++) {
            fprintf(fp, "string%d ", i);
        }
        fclose(fp);
        
        FILE *in = fopen("string_test.txt", "r");
        stdin = in;
        
        double start = get_time();
        char val[100];
        int count = 0;
        while (yscanf("%s", val) == 1) count++;
        double end = get_time();
        
        printf("String parsing: %d values in %.3f seconds (%.0f values/sec)\n", 
               count, end - start, count / (end - start));
        fclose(in);
    }
}

/* Memory usage benchmark */
static void benchmark_memory_usage(void) {
    printf("\n=== Memory usage analysis ===\n");
    
    printf("Original buffer size: %d bytes (%.1f MB)\n", 
           1 << 22, (1 << 22) / (1024.0 * 1024.0));
    printf("Optimized buffer size: %d bytes (%.1f MB)\n", 
           YSCANF_BUFFER_SIZE, YSCANF_BUFFER_SIZE / (1024.0 * 1024.0));
    
    printf("Static memory usage (original): ~4 MB\n");
    printf("Static memory usage (optimized): ~%.1f MB\n", 
           YSCANF_BUFFER_SIZE / (1024.0 * 1024.0));
}

/* Cache performance test */
static void benchmark_cache_performance(void) {
    printf("\n=== Cache performance test ===\n");
    
    /* Test with different input sizes */
    int sizes[] = {1000, 10000, 100000, 1000000};
    const char *size_names[] = {"1K", "10K", "100K", "1M"};
    
    for (int s = 0; s < 4; s++) {
        char filename[100];
        snprintf(filename, sizeof(filename), "cache_test_%s.txt", size_names[s]);
        
        /* Generate test data */
        FILE *fp = fopen(filename, "w");
        for (int i = 0; i < sizes[s]; i++) {
            fprintf(fp, "%d %.3f ", i, (double)i);
        }
        fclose(fp);
        
        /* Benchmark */
        FILE *in = fopen(filename, "r");
        stdin = in;
        
        double start = get_time();
        int ival;
        double fval;
        int count = 0;
        while (yscanf("%d %lf", &ival, &fval) == 2) count++;
        double end = get_time();
        
        printf("Size %s: %d pairs in %.3f seconds (%.1f ns/pair)\n", 
               size_names[s], count, end - start, 
               ((end - start) * 1e9) / count);
        
        fclose(in);
        remove(filename);
    }
}

int main(void) {
    printf("=== YScanF Performance Benchmark ===\n\n");
    
    /* Generate test data */
    printf("Generating test data...\n");
    generate_test_data("benchmark_data.txt", BENCHMARK_SIZE);
    
    printf("Test data: %d records generated\n", BENCHMARK_SIZE);
    
    /* Run main benchmark */
    printf("\n=== Main benchmark (averaged over %d iterations) ===\n", ITERATIONS);
    
    double total_orig = 0, total_opt = 0;
    
    for (int i = 0; i < ITERATIONS; i++) {
        printf("\nIteration %d:\n", i + 1);
        
        double t_orig = benchmark_original("benchmark_data.txt");
        double t_opt = benchmark_optimized("benchmark_data.txt");
        
        total_orig += t_orig;
        total_opt += t_opt;
        
        printf("Original: %.3f seconds\n", t_orig);
        printf("Optimized: %.3f seconds\n", t_opt);
        printf("Speedup: %.2fx\n", t_orig / t_opt);
    }
    
    double avg_orig = total_orig / ITERATIONS;
    double avg_opt = total_opt / ITERATIONS;
    
    printf("\n=== Average Results ===\n");
    printf("Original average: %.3f seconds\n", avg_orig);
    printf("Optimized average: %.3f seconds\n", avg_opt);
    printf("Average speedup: %.2fx\n", avg_orig / avg_opt);
    printf("Performance improvement: %.1f%%\n", 
           ((avg_orig - avg_opt) / avg_orig) * 100);
    
    /* Run specialized benchmarks */
    benchmark_operations();
    benchmark_memory_usage();
    benchmark_cache_performance();
    
    /* Cleanup */
    remove("benchmark_data.txt");
    remove("int_test.txt");
    remove("float_test.txt");
    remove("string_test.txt");
    
    printf("\n=== Benchmark Complete ===\n");
    
    return 0;
}
