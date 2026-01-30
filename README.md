
# YScanF - High-Performance Buffered Input Parser

## Overview

YScanF is a high-performance, buffered input parsing library designed as a faster alternative to standard `scanf()`. Originally created for competitive programming, this optimized version provides enhanced reliability, better error handling, and improved performance.

## Key Features

- **High Performance**: Buffered input with 4MB default buffer size
- **Overflow Protection**: Built-in integer overflow detection
- **Type Safety**: Enhanced parsing with bounds checking
- **Branch Prediction**: Compiler hints for better performance
- **Extensible**: Configurable buffer size and behavior
- **Lightweight**: Header-only library, no dependencies

## Supported Format Specifiers

| Specifier | Description | Example |
|-----------|-------------|---------|
| `%d` | Signed integer | `123`, `-456` |
| `%u` | Unsigned integer | `123`, `456` |
| `%lld` | Signed long long | `9223372036854775807` |
| `%llu` | Unsigned long long | `18446744073709551615` |
| `%f`, `%g`, `%e` | Double precision | `3.14`, `1.23e-4` |
| `%s` | String (whitespace delimited) | `hello` |
| `%c` | Single character | `A` |

## Usage Example

```c
#include "yscanf.h"

int main() {
    int n;
    double x;
    char str[100];
    
    // Read multiple values
    yscanf("%d %lf %s", &n, &x, str);
    printf("Read: %d, %f, %s\n", n, x, str);
    
    return 0;
}
```

## Performance Optimizations

### 1. Buffer Management
- Large 4MB buffer reduces system calls
- Hot/cold code partitioning for better cache usage
- Efficient buffer refill mechanism

### 2. Integer Parsing
- Fast digit accumulation with overflow detection
- Early termination on overflow
- Optimized for common cases

### 3. Floating Point
- Efficient scientific notation handling
- Fast exponentiation using bit operations
- Proper edge case management

### 4. Branch Prediction
- Uses `likely()`/`unlikely()` hints for GCC/Clang
- Reduces branch misprediction penalties
- Optimized control flow

## Configuration

### Buffer Size
```c
#define YSCANF_BUFFER_SIZE (1 << 20)  // 1MB buffer
#include "yscanf_optimized.h"
```

### Custom Behavior
The library provides macros for customization:
- `YSCANF_LIKELY`/`YSCANF_UNLIKELY`: Branch prediction hints
- `YSCANF_BUFFER_SIZE`: Input buffer size

## Error Handling

### Integer Overflow
- Returns `INT32_MAX`/`INT32_MIN` for `int` overflow
- Returns `LLONG_MAX`/`LLONG_MIN` for `long long` overflow
- Returns `UINT32_MAX`/`ULLONG_MAX` for unsigned overflow

### EOF Handling
- Proper EOF detection and propagation
- Returns `EOF` when no items successfully parsed
- Returns count of successfully parsed items

### Invalid Input
- Returns `-1` for unsupported format specifiers
- Graceful handling of malformed input

## Comparison with Original

| Aspect | Original | Optimized |
|--------|----------|-----------|
| Overflow Detection | ❌ | ✅ |
| Branch Prediction | ❌ | ✅ |
| Documentation | Basic | Comprehensive |
| Error Handling | Minimal | Robust |
| Performance | Fast | Faster |
| Code Structure | Monolithic | Modular |

## Building and Testing

### Compilation
```bash
gcc -O3 -march=native -flto test.c -o test
```

### Testing
```bash
# Run basic functionality test
./test < input.txt

# Performance comparison
time ./test < large_input.txt
```

## Files

- `yscanf_optimized.h`: Main optimized header
- `yscanf.h`: Original version (for comparison)
- `test_yscanf.c`: Test suite
- `benchmark.c`: Performance benchmarks

## Performance Tips

1. **Buffer Size**: Adjust `YSCANF_BUFFER_SIZE` based on input size
2. **Compiler Flags**: Use `-O3 -march=native -flto` for best performance
3. **Input Patterns**: Batch similar operations for better cache usage
4. **Format Strings**: Use simple format specifiers when possible

## Limitations

- Not fully `scanf()` compatible
- No support for width specifiers
- No support for custom separators
- String reading has no bounds checking
- Single-threaded only

## License

This is an optimized version of the original YScanF library. Please refer to the original author's licensing terms.

## Contributing

Feel free to submit issues and enhancement requests!
