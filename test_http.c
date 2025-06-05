#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <r_core.h>
#include "../r2ai.h"
#include "test_mocks.h"

// Mock the actual HTTP function during testing
#ifdef TEST_MODE
#define r2ai_http_post mock_r2ai_http_post
#endif

// Test: Basic HTTP functionality with mocks
void test_http_basic(void **state) {
    (void) state;

    enable_mocks();
    
    int code = 0;
    int rlen = 0;
    
    // Setup a mock response
    setup_http_mock("{\"status\": \"ok\"}", 200, 16);
    
    // Test with valid parameters
    char *result = r2ai_http_post("http://example.com", NULL, "test data", &code, &rlen);
    assert_non_null(result);
    assert_int_equal(code, 200);
    assert_int_equal(rlen, 16);
    assert_string_equal(result, "{\"status\": \"ok\"}");
    free(result);

    // Test with NULL parameters (should handle gracefully)
    result = r2ai_http_post(NULL, NULL, NULL, &code, &rlen);
    assert_null(result);

    cleanup_mocks();
    disable_mocks();
}

// Test: HTTP POST with different scenarios
void test_http_post(void **state) {
    (void) state;

    enable_mocks();
    
    int code = 0;
    int rlen = 0;
    
    // Test error response
    setup_http_mock("{\"error\": \"Not found\"}", 404, 20);
    
    char *result = r2ai_http_post("http://example.com/notfound", NULL, "test", &code, &rlen);
    assert_non_null(result);
    assert_int_equal(code, 404);
    assert_string_equal(result, "{\"error\": \"Not found\"}");
    free(result);

    // Test empty response
    setup_http_mock("", 200, 0);
    
    result = r2ai_http_post("http://example.com/empty", NULL, "", &code, &rlen);
    assert_non_null(result);
    assert_int_equal(code, 200);
    assert_int_equal(rlen, 0);
    free(result);

    // Test large response
    const char *large_response = "{\"data\": \"This is a longer response with more content\"}";
    setup_http_mock(large_response, 200, strlen(large_response));
    
    result = r2ai_http_post("http://example.com/large", NULL, "test", &code, &rlen);
    assert_non_null(result);
    assert_int_equal(code, 200);
    assert_int_equal(rlen, strlen(large_response));
    assert_string_equal(result, large_response);
    free(result);

    cleanup_mocks();
    disable_mocks();
}

// ... existing code ...