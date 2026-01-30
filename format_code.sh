
#!/bin/bash
# YScanF Code Formatting Script
# Automatically formats and checks code quality

set -e

# Configuration
CLANG_FORMAT_STYLE="{
    BasedOnStyle: Google,
    IndentWidth: 4,
    TabWidth: 4,
    UseTab: Never,
    ColumnLimit: 100,
    AlignAfterOpenBracket: Align,
    AlignConsecutiveAssignments: true,
    AlignConsecutiveDeclarations: true,
    AllowShortFunctionsOnASingleLine: Inline,
    AlwaysBreakAfterReturnType: None,
    BreakBeforeBraces: Attach,
    PointerAlignment: Right,
    SpaceAfterCStyleCast: false,
    SpaceBeforeParens: ControlStatements,
    SpacesInParentheses: false,
    SpacesInSquareBrackets: false,
    SpacesInAngles: false,
    KeepEmptyLinesAtTheStartOfBlocks: false,
    MaxEmptyLinesToKeep: 1,
    SortIncludes: true,
    IndentCaseLabels: true
}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== YScanF Code Formatting Tool ===${NC}"

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check for required tools
check_requirements() {
    echo -e "${YELLOW}Checking requirements...${NC}"
    
    if ! command_exists clang-format; then
        echo -e "${RED}Error: clang-format not found${NC}"
        echo "Please install clang-format:"
        echo "  Ubuntu/Debian: sudo apt-get install clang-format"
        echo "  macOS: brew install clang-format"
        echo "  Fedora: sudo dnf install clang-tools-extra"
        exit 1
    fi
    
    if ! command_exists cppcheck; then
        echo -e "${YELLOW}Warning: cppcheck not found (static analysis will be skipped)${NC}"
    fi
    
    echo -e "${GREEN}Requirements check passed${NC}"
}

# Format C/C++ files
format_files() {
    echo -e "${YELLOW}Formatting C/C++ files...${NC}"
    
    local files=("yscanf.h" "yscanf_optimized.h" "test_yscanf.c" "benchmark.c")
    
    for file in "${files[@]}"; do
        if [ -f "$file" ]; then
            echo "Formatting $file..."
            clang-format -i -style="$CLANG_FORMAT_STYLE" "$file"
            echo -e "${GREEN}✓ $file formatted${NC}"
        fi
    done
}

# Run static analysis
run_static_analysis() {
    if ! command_exists cppcheck; then
        echo -e "${YELLOW}Skipping static analysis (cppcheck not available)${NC}"
        return
    fi
    
    echo -e "${YELLOW}Running static analysis...${NC}"
    
    cppcheck --enable=all --suppress=missingIncludeSystem \
             --inline-suppr --force \
             --std=c11 \
             --platform=unix64 \
             yscanf_optimized.h test_yscanf.c \
             2>&1 | tee cppcheck_report.txt
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Static analysis completed${NC}"
    else
        echo -e "${YELLOW}Static analysis found issues (check cppcheck_report.txt)${NC}"
    fi
}

# Check code style
check_style() {
    echo -e "${YELLOW}Checking code style...${NC}"
    
    local issues=0
    
    # Check for tabs in source files
    if grep -r $'\t' *.h *.c 2>/dev/null; then
        echo -e "${RED}Found tabs in source files${NC}"
        issues=$((issues + 1))
    fi
    
    # Check for trailing whitespace
    if grep -r '[[:space:]]$' *.h *.c 2>/dev/null; then
        echo -e "${RED}Found trailing whitespace${NC}"
        issues=$((issues + 1))
    fi
    
    # Check for long lines (>100 characters)
    if awk 'length > 100 {print FILENAME ":" NR ":" $0}' *.h *.c 2>/dev/null; then
        echo -e "${YELLOW}Found lines longer than 100 characters${NC}"
    fi
    
    # Check for proper header guards
    for file in *.h; do
        if [ -f "$file" ]; then
            guard=$(echo "$file" | tr '[:lower:].' '[:upper:]_')
            if ! grep -q "#ifndef ${guard}" "$file"; then
                echo -e "${YELLOW}Missing or incorrect header guard in $file${NC}"
            fi
        fi
    done
    
    if [ $issues -eq 0 ]; then
        echo -e "${GREEN}Code style check passed${NC}"
    else
        echo -e "${RED}Code style issues found: $issues${NC}"
    fi
}

# Generate documentation
generate_docs() {
    echo -e "${YELLOW}Generating documentation...${NC}"
    
    # Create simple documentation from header comments
    if [ -f "yscanf_optimized.h" ]; then
        echo "Extracting function documentation..."
        grep -E "^/\*\*|^ \* @|^static inline" yscanf_optimized.h > function_docs.txt
        echo -e "${GREEN}Function documentation extracted to function_docs.txt${NC}"
    fi
}

# Performance benchmarking
run_benchmarks() {
    echo -e "${YELLOW}Running performance benchmarks...${NC}"
    
    if [ -f "benchmark.c" ]; then
        echo "Compiling benchmark..."
        gcc -O3 -march=native -flto benchmark.c -o benchmark -lm
        
        echo "Running benchmark..."
        ./benchmark > benchmark_results.txt
        
        echo -e "${GREEN}Benchmark results saved to benchmark_results.txt${NC}"
    else
        echo -e "${YELLOW}No benchmark.c found, skipping benchmarks${NC}"
    fi
}

# Clean up temporary files
cleanup() {
    echo -e "${YELLOW}Cleaning up temporary files...${NC}"
    
    rm -f test_input.txt perf_test.txt
    rm -f cppcheck_report.txt function_docs.txt
    rm -f benchmark benchmark_results.txt
    
    echo -e "${GREEN}Cleanup completed${NC}"
}

# Main execution
main() {
    check_requirements
    
    case "${1:-all}" in
        format)
            format_files
            ;;
        analyze)
            run_static_analysis
            ;;
        style)
            check_style
            ;;
        docs)
            generate_docs
            ;;
        benchmark)
            run_benchmarks
            ;;
        all)
            format_files
            check_style
            run_static_analysis
            generate_docs
            run_benchmarks
            ;;
        clean)
            cleanup
            ;;
        *)
            echo "Usage: $0 [format|analyze|style|docs|benchmark|all|clean]"
            echo ""
            echo "Commands:"
            echo "  format    - Format all C/C++ files"
            echo "  analyze   - Run static analysis"
            echo "  style     - Check code style"
            echo "  docs      - Generate documentation"
            echo "  benchmark - Run performance benchmarks"
            echo "  all       - Run all checks (default)"
            echo "  clean     - Clean temporary files"
            exit 1
            ;;
    esac
    
    echo -e "${GREEN}=== Code formatting completed ===${NC}"
}

# Run main function
main "$@"
