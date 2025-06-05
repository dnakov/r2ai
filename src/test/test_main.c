#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>

// Test function declarations
extern void test_messages_api(void **state);
extern void test_messages_lifecycle(void **state);
extern void test_messages_tool_calls(void **state);
extern void test_messages_json_parsing(void **state);
extern void test_messages_conversation(void **state);

extern void test_tools_basic(void **state);
extern void test_tools_parsing(void **state);
extern void test_tools_execution(void **state);
extern void test_tools_json_format(void **state);

extern void test_vdb_basic(void **state);
extern void test_vdb_insertion(void **state);
extern void test_vdb_query(void **state);
extern void test_vdb_knn_search(void **state);

extern void test_markdown_basic(void **state);
extern void test_markdown_formatting(void **state);

extern void test_http_basic(void **state);
extern void test_http_post(void **state);

int main(void) {
    printf("Starting r2ai Unit Tests...\n");
    printf("========================================\n");

    const struct CMUnitTest tests[] = {
        // Messages API Tests (Critical - High Priority)
        cmocka_unit_test(test_messages_api),
        cmocka_unit_test(test_messages_lifecycle),
        cmocka_unit_test(test_messages_tool_calls),
        cmocka_unit_test(test_messages_json_parsing),
        cmocka_unit_test(test_messages_conversation),

        // Tools Framework Tests
        cmocka_unit_test(test_tools_basic),
        cmocka_unit_test(test_tools_parsing),
        cmocka_unit_test(test_tools_execution),
        cmocka_unit_test(test_tools_json_format),

        // Vector Database Tests
        cmocka_unit_test(test_vdb_basic),
        cmocka_unit_test(test_vdb_insertion),
        cmocka_unit_test(test_vdb_query),
        cmocka_unit_test(test_vdb_knn_search),

        // Markdown Tests
        cmocka_unit_test(test_markdown_basic),
        cmocka_unit_test(test_markdown_formatting),

        // HTTP Client Tests
        cmocka_unit_test(test_http_basic),
        cmocka_unit_test(test_http_post),
    };

    int result = cmocka_run_group_tests(tests, NULL, NULL);
    
    printf("\n========================================\n");
    if (result == 0) {
        printf("All tests PASSED!\n");
    } else {
        printf("Some tests FAILED!\n");
    }
    printf("========================================\n");

    return result;
}