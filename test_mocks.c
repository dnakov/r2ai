#include "test_mocks.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Mock state
bool mock_http_enabled = false;
char *mock_http_response = NULL;
int mock_http_code = 200;
int mock_http_length = 0;

// Mock HTTP POST function
char *mock_r2ai_http_post(const char *url, const char *headers[], const char *data, int *code, int *rlen) {
    if (!mock_http_enabled) {
        // If mocks are disabled, return NULL to avoid real calls
        if (code) *code = 0;
        if (rlen) *rlen = 0;
        return NULL;
    }

    // Return mock response
    if (code) *code = mock_http_code;
    if (rlen) *rlen = mock_http_length;
    
    if (mock_http_response) {
        return strdup(mock_http_response);
    }
    
    return NULL;
}

// Setup mock HTTP response
void setup_http_mock(const char *response, int response_code, int response_length) {
    cleanup_mocks();
    
    if (response) {
        mock_http_response = strdup(response);
        mock_http_length = response_length > 0 ? response_length : strlen(response);
    }
    mock_http_code = response_code;
}

// Clean up mock data
void cleanup_mocks(void) {
    if (mock_http_response) {
        free(mock_http_response);
        mock_http_response = NULL;
    }
    mock_http_code = 200;
    mock_http_length = 0;
}

// Enable/disable mocks
void enable_mocks(void) {
    mock_http_enabled = true;
}

void disable_mocks(void) {
    mock_http_enabled = false;
    cleanup_mocks();
}