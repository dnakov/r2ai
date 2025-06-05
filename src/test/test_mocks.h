#ifndef TEST_MOCKS_H
#define TEST_MOCKS_H
#include <stdbool.h>
extern bool mock_http_enabled;
char *mock_r2ai_http_post(const char *url, const char *headers[], const char *data, int *code, int *rlen);
void setup_http_mock(const char *response, int response_code, int response_length);
void cleanup_mocks(void);
void enable_mocks(void);
void disable_mocks(void);
#endif
