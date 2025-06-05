#ifndef TEST_MOCKS_H
#define TEST_MOCKS_H

#include <stdbool.h>

// Mock control flags
extern bool mock_http_enabled;
extern char *mock_http_response;
extern int mock_http_code;
extern int mock_http_length;

// Mock functions
char *mock_r2ai_http_post(const char *url, const char *headers[], const char *data, int *code, int *rlen);

// Mock setup functions
void setup_http_mock(const char *response, int response_code, int response_length);
void cleanup_mocks(void);
void enable_mocks(void);
void disable_mocks(void);

#endif