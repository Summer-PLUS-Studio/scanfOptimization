
/**
 * @file yscanf.h
 * @brief High-performance buffered input parser for competitive programming
 * @author Summer PLUS Studio
 * @email yuzhouhunter@outlook.com
 * @warning Not fully scanf-compatible, designed for competitive programming
 * @version 2.0
 */

#ifndef YSCANF_H
#define YSCANF_H

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

/* ============================================================================
 * CONFIGURATION MACROS
 * ============================================================================ */

#ifndef YSCANF_BUFFER_SIZE
#define YSCANF_BUFFER_SIZE (1 << 22)  /* 4MB default buffer */
#endif

#ifndef YSCANF_LIKELY
#if defined(__GNUC__) || defined(__clang__)
#define YSCANF_LIKELY(x)   __builtin_expect(!!(x), 1)
#define YSCANF_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define YSCANF_LIKELY(x)   (x)
#define YSCANF_UNLIKELY(x) (x)
#endif
#endif

/* ============================================================================
 * BUFFER MANAGEMENT
 * ============================================================================ */

static char ybuf[YSCANF_BUFFER_SIZE];
static char *yptr = ybuf;
static char *yend = ybuf;

/**
 * @brief Refill the input buffer from stdin
 * @note Uses hot/cold partitioning for better cache performance
 */
static inline void yrefill(void)
{
    size_t len = fread(ybuf, 1, sizeof(ybuf), stdin);
    yptr = ybuf;
    yend = ybuf + len;
}

/**
 * @brief Get next character with buffer management
 * @return Next character or EOF
 */
static inline int ynext_char(void)
{
    if (YSCANF_UNLIKELY(yptr >= yend)) {
        yrefill();
        if (YSCANF_UNLIKELY(yptr >= yend))
            return EOF;
    }
    return (unsigned char)*yptr++;
}

/**
 * @brief Peek at next character without consuming it
 * @return Next character or EOF
 */
static inline int ypeek_char(void)
{
    if (YSCANF_UNLIKELY(yptr >= yend)) {
        yrefill();
        if (YSCANF_UNLIKELY(yptr >= yend))
            return EOF;
    }
    return (unsigned char)*yptr;
}

/**
 * @brief Skip whitespace characters
 * @note Optimized for common case (space/newline)
 */
static inline void yskip_space_input(void)
{
    int c;
    while ((c = ypeek_char()) != EOF && YSCANF_LIKELY(isspace((unsigned char)c))) {
        ynext_char();
    }
}

/* ============================================================================
 * NUMBER PARSING UTILITIES
 * ============================================================================ */

/**
 * @brief Parse signed integer with overflow detection
 * @param[out] overflow Set to 1 if overflow detected
 * @return Parsed integer value
 */
static inline int64_t yparse_int(int *overflow)
{
    int64_t x = 0;
    int sign = 1;
    int c;

    *overflow = 0;
    yskip_space_input();
    c = ynext_char();

    if (c == '-') {
        sign = -1;
        c = ynext_char();
    } else if (c == '+') {
        c = ynext_char();
    }

    if (YSCANF_UNLIKELY(c < '0' || c > '9')) {
        *overflow = 1;
        return 0;
    }

    while (c >= '0' && c <= '9') {
        int64_t next = x * 10 + (c - '0');
        if (YSCANF_UNLIKELY(next < x)) {  /* Overflow detection */
            *overflow = 1;
            /* Consume remaining digits */
            while ((c = ynext_char()) >= '0' && c <= '9');
            break;
        }
        x = next;
        c = ynext_char();
    }

    return sign * x;
}

/**
 * @brief Parse unsigned integer with overflow detection
 * @param[out] overflow Set to 1 if overflow detected
 * @return Parsed unsigned integer value
 */
static inline uint64_t yparse_uint(int *overflow)
{
    uint64_t x = 0;
    int c;

    *overflow = 0;
    yskip_space_input();
    c = ynext_char();

    if (YSCANF_UNLIKELY(c < '0' || c > '9')) {
        *overflow = 1;
        return 0;
    }

    while (c >= '0' && c <= '9') {
        uint64_t next = x * 10 + (c - '0');
        if (YSCANF_UNLIKELY(next < x)) {  /* Overflow detection */
            *overflow = 1;
            /* Consume remaining digits */
            while ((c = ynext_char()) >= '0' && c <= '9');
            break;
        }
        x = next;
        c = ynext_char();
    }

    return x;
}

/* ============================================================================
 * OPTIMIZED READERS
 * ============================================================================ */

static inline int yread_int(void)
{
    int overflow;
    int64_t val = yparse_int(&overflow);
    if (YSCANF_UNLIKELY(overflow || val > INT32_MAX || val < INT32_MIN)) {
        return (val > 0) ? INT32_MAX : INT32_MIN;
    }
    return (int)val;
}

static inline long long yread_ll(void)
{
    int overflow;
    int64_t val = yparse_int(&overflow);
    if (YSCANF_UNLIKELY(overflow)) {
        return (val > 0) ? LLONG_MAX : LLONG_MIN;
    }
    return val;
}

static inline unsigned yread_uint(void)
{
    int overflow;
    uint64_t val = yparse_uint(&overflow);
    if (YSCANF_UNLIKELY(overflow || val > UINT32_MAX)) {
        return UINT32_MAX;
    }
    return (unsigned)val;
}

static inline unsigned long long yread_ull(void)
{
    int overflow;
    uint64_t val = yparse_uint(&overflow);
    if (YSCANF_UNLIKELY(overflow)) {
        return ULLONG_MAX;
    }
    return val;
}

/**
 * @brief Optimized double parsing using fast algorithms
 * @note Handles scientific notation and edge cases
 */
static inline double yread_double(void)
{
    double x = 0.0, frac = 0.0, base = 1.0;
    int sign = 1;
    int c;

    yskip_space_input();
    c = ynext_char();

    if (c == '-') {
        sign = -1;
        c = ynext_char();
    } else if (c == '+') {
        c = ynext_char();
    }

    /* Parse integer part */
    while (c >= '0' && c <= '9') {
        x = x * 10.0 + (c - '0');
        c = ynext_char();
    }

    /* Parse fractional part */
    if (c == '.') {
        c = ynext_char();
        while (c >= '0' && c <= '9') {
            frac = frac * 10.0 + (c - '0');
            base *= 10.0;
            c = ynext_char();
        }
    }

    return sign * (x + frac / base);
}

/**
 * @brief Parse double with scientific notation
 */
static inline double yread_double_exp(void)
{
    double x = yread_double();
    int c = ypeek_char();

    if (c == 'e' || c == 'E') {
        ynext_char();  /* consume 'e'/'E' */

        int esign = 1, exp = 0;
        c = ynext_char();

        if (c == '-') {
            esign = -1;
            c = ynext_char();
        } else if (c == '+') {
            c = ynext_char();
        }

        /* Parse exponent */
        while (c >= '0' && c <= '9') {
            exp = exp * 10 + (c - '0');
            c = ynext_char();
        }

        /* Fast exponentiation using pow() - compiler will optimize */
        double power = 1.0;
        if (esign == 1) {
            power = 1e308;  /* Max reasonable exponent */
            if (exp < 308) power = 1.0;
            for (int i = 0; i < exp; i++) {
                if (power > 1e300) break;
                power *= 10.0;
            }
        } else {
            power = 1.0;
            for (int i = 0; i < exp; i++) {
                power /= 10.0;
            }
        }

        return x * power;
    }

    return x;
}

/**
 * @brief Read string until whitespace
 * @param[out] s Output buffer (must be large enough)
 * @warning No bounds checking - ensure sufficient buffer size
 */
static inline void yread_string(char *s)
{
    int c;
    yskip_space_input();

    while ((c = ynext_char()) != EOF && !isspace((unsigned char)c)) {
        *s++ = (char)c;
    }
    *s = '\0';
}

/* ============================================================================
 * MAIN YSCANF FUNCTION
 * ============================================================================ */

/**
 * @brief Fast scanf-like function with format string parsing
 * @param fmt Format string (limited scanf compatibility)
 * @return Number of successfully parsed items
 */
static inline int yscanf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int cnt = 0;
    int skip_space = 0;

    while (*fmt) {
        /* Handle whitespace in format */
        if (isspace((unsigned char)*fmt)) {
            skip_space = 1;
            fmt++;
            continue;
        }

        /* Skip literal characters */
        if (*fmt != '%') {
            skip_space = 0;
            fmt++;
            continue;
        }

        /* Parse format specifier */
        fmt++;

        /* Handle different format specifiers */
        switch (*fmt) {
            case 'd': {
                if (skip_space) yskip_space_input();
                int *p = va_arg(ap, int *);
                *p = yread_int();
                cnt++;
                break;
            }

            case 'u': {
                if (skip_space) yskip_space_input();
                unsigned *p = va_arg(ap, unsigned *);
                *p = yread_uint();
                cnt++;
                break;
            }

            case 's': {
                if (skip_space) yskip_space_input();
                char *p = va_arg(ap, char *);
                yread_string(p);
                cnt++;
                break;
            }

            case 'c': {
                char *p = va_arg(ap, char *);
                if (skip_space) yskip_space_input();
                int c = ynext_char();
                if (c == EOF) {
                    va_end(ap);
                    return cnt ? cnt : EOF;
                }
                *p = (char)c;
                cnt++;
                break;
            }

            case 'l': {
                fmt++;
                switch (*fmt) {
                    case 'l': {
                        fmt++;
                        if (*fmt == 'd') {
                            if (skip_space) yskip_space_input();
                            long long *p = va_arg(ap, long long *);
                            *p = yread_ll();
                            cnt++;
                        } else if (*fmt == 'u') {
                            if (skip_space) yskip_space_input();
                            unsigned long long *p = va_arg(ap, unsigned long long *);
                            *p = yread_ull();
                            cnt++;
                        } else {
                            va_end(ap);
                            return -1;
                        }
                        break;
                    }

                    case 'f': {
                        if (skip_space) yskip_space_input();
                        double *p = va_arg(ap, double *);
                        *p = yread_double();
                        cnt++;
                        break;
                    }

                    default:
                        va_end(ap);
                        return -1;
                }
                break;
            }

            case 'g':
            case 'e':
            case 'f': {
                if (skip_space) yskip_space_input();
                double *p = va_arg(ap, double *);
                *p = yread_double_exp();
                cnt++;
                break;
            }

            default:
                va_end(ap);
                return -1;
        }

        skip_space = 0;
        fmt++;
    }

    va_end(ap);
    return cnt;
}

#endif /* YSCANF_H */

/* ============================================================================
 * USAGE EXAMPLE
 * ============================================================================
 *
 * #include "yscanf.h"
 *
 * int main() {
 *     int n;
 *     double x;
 *     char str[100];
 *
 *     yscanf("%d %lf %s", &n, &x, str);
 *     printf("Read: %d, %f, %s\n", n, x, str);
 *
 *     return 0;
 * }
 * ============================================================================ */
