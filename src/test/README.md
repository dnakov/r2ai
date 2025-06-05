# R2AI Unit Testing Framework

This directory contains comprehensive unit tests for the r2ai C project, designed to achieve 90%+ code coverage.

## Overview

The testing framework uses **cmocka** as the testing library and covers the following modules:

- **Messages API** (`test_messages.c`) - Conversation and message management
- **Tools Framework** (`test_tools.c`) - Tool execution and JSON handling
- **Vector Database** (`test_vdb.c`) - Embeddings and K-NN search
- **Markdown Processing** (`test_markdown.c`) - Text formatting
- **HTTP Client** (`test_http.c`) - Network communication

## Prerequisites

### Install Dependencies

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y libcmocka-dev lcov gcov

# macOS (with Homebrew)
brew install cmocka lcov

# CentOS/RHEL
sudo yum install libcmocka-devel lcov

# Or use the provided target
make install-deps
```

### Radare2 Requirements

Ensure radare2 is installed and available:

```bash
# Install radare2 from git (recommended)
git clone https://github.com/radareorg/radare2
cd radare2
./configure --prefix=/usr/local
make -j$(nproc)
sudo make install

# Or install from package manager
sudo apt-get install radare2  # Ubuntu/Debian
brew install radare2          # macOS
```

## Building and Running Tests

### Quick Start

```bash
# Build and run all tests
make test

# Build only
make all

# Clean build artifacts
make clean
```

### Coverage Analysis

Generate code coverage reports:

```bash
# Generate coverage report
make coverage

# View HTML report
firefox coverage_html/index.html  # Linux
open coverage_html/index.html     # macOS
```

### Individual Test Categories

You can run specific test categories by modifying the test runner or using debugging tools:

```bash
# Run tests with verbose output
./run_tests

# Run tests with GDB for debugging
gdb ./run_tests
(gdb) run

# Run tests with Valgrind for memory checking
valgrind --leak-check=full ./run_tests
```

## Test Structure

### Messages API Tests (`test_messages.c`)

- **test_messages_api**: Basic message creation and management
- **test_messages_lifecycle**: Conversation initialization and cleanup
- **test_messages_tool_calls**: Tool call functionality
- **test_messages_json_parsing**: JSON parsing and serialization
- **test_messages_conversation**: Advanced conversation features

### Tools Framework Tests (`test_tools.c`)

- **test_tools_basic**: Global tools instance and basic operations
- **test_tools_parsing**: JSON tool definition parsing
- **test_tools_execution**: Tool execution validation (mocked)
- **test_tools_json_format**: OpenAI/Anthropic format conversion

### Vector Database Tests (`test_vdb.c`)

- **test_vdb_basic**: Database creation and management
- **test_vdb_insertion**: Document insertion and indexing
- **test_vdb_query**: Query functionality and parameter validation
- **test_vdb_knn_search**: K-nearest neighbor search algorithm

### Markdown Tests (`test_markdown.c`)

- **test_markdown_basic**: Basic text processing
- **test_markdown_formatting**: Markdown feature support

### HTTP Client Tests (`test_http.c`)

- **test_http_basic**: Basic HTTP functionality
- **test_http_post**: POST request handling and parameter validation

## Coverage Goals

The testing framework aims to achieve **90%+ code coverage** across all modules:

### Current Target Coverage

| Module | Target Coverage | Priority |
|--------|----------------|----------|
| Messages API | 95%+ | Critical |
| Tools Framework | 90%+ | High |
| Vector Database | 90%+ | High |
| HTTP Client | 85%+ | Medium |
| Markdown Processing | 85%+ | Medium |

### Coverage Analysis

Use the coverage report to identify untested code:

```bash
make coverage
# Check coverage_html/index.html for detailed analysis
# Look for red-highlighted lines indicating untested code
```

## Testing Philosophy

### Unit Test Principles

1. **Isolation**: Each test is independent and doesn't depend on external services
2. **Deterministic**: Tests produce consistent results across runs
3. **Fast**: Tests execute quickly to enable frequent running
4. **Comprehensive**: Cover normal cases, edge cases, and error conditions

### Test Categories

- **Positive Tests**: Verify correct behavior with valid inputs
- **Negative Tests**: Verify error handling with invalid inputs
- **Edge Cases**: Test boundary conditions and unusual scenarios
- **Integration**: Verify component interactions work correctly

### Mocking Strategy

Since some components require radare2 integration:

- **Tools execution**: Mocked with parameter validation
- **HTTP requests**: Tested for robustness, not network connectivity
- **Core integration**: Focused on API contracts rather than full integration

## Troubleshooting

### Common Issues

**Build Failures:**
```bash
# Missing radare2 development headers
sudo apt-get install radare2-dev

# Missing cmocka
make install-deps

# Compilation errors
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
```

**Test Failures:**
```bash
# Run tests with verbose output
make test VERBOSE=1

# Check for memory leaks
valgrind --leak-check=full ./run_tests

# Debug specific test
gdb --args ./run_tests --filter="test_messages_*"
```

**Coverage Issues:**
```bash
# Ensure gcov is working
gcc --version  # Should support --coverage
gcov --version

# Check compiler flags
make clean && make coverage CFLAGS="-g -O0 --coverage"
```

### Network-dependent Tests

HTTP tests may fail without internet connectivity:

```bash
# Skip network tests (if implemented)
./run_tests --skip-network

# Or run offline with mock data
export R2AI_TEST_OFFLINE=1
./run_tests
```

## Extending Tests

### Adding New Tests

1. Create test functions following the naming convention
2. Add function declarations to `test_main.c`
3. Add test cases to the `tests[]` array
4. Update this README with new test descriptions

### Test Function Template

```c
void test_new_feature(void **state) {
    (void) state; // unused parameter

    // Setup
    // ... initialize test data ...

    // Execute
    // ... call function under test ...

    // Verify
    assert_non_null(result);
    assert_int_equal(expected, actual);
    
    // Cleanup
    // ... free resources ...
}
```

### Coverage Targets

Aim for these coverage metrics:

- **Line Coverage**: 90%+
- **Function Coverage**: 95%+
- **Branch Coverage**: 85%+

### Mock Guidelines

When creating mocks:

1. Focus on testing the API contract
2. Validate input parameters thoroughly
3. Return realistic mock data
4. Handle error conditions explicitly

## Continuous Integration

This testing framework is designed for CI/CD integration:

```yaml
# Example GitHub Actions workflow
- name: Install Dependencies
  run: make install-deps

- name: Build Tests
  run: make all

- name: Run Tests
  run: make test

- name: Generate Coverage
  run: make coverage

- name: Upload Coverage
  uses: codecov/codecov-action@v1
  with:
    files: coverage.info
```

## Performance Considerations

- Tests should complete in under 30 seconds
- Memory usage should be bounded and predictable
- Network tests are designed to be timeout-resistant
- Vector database tests use small datasets for speed

## Contributing

When contributing tests:

1. Follow the existing code style
2. Add comprehensive test coverage for new features
3. Update documentation for any new test patterns
4. Ensure all tests pass before submitting
5. Include both positive and negative test cases

## References

- [cmocka Documentation](https://cmocka.org/)
- [gcov/lcov Coverage Tools](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html)
- [Radare2 Development Guide](https://book.rada.re/)