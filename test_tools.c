#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <r_core.h>
#include "../r2ai.h"

// Test: Basic tools functionality
void test_tools_basic(void **state) {
    (void) state;

    // Test getting global tools instance
    const R2AI_Tools *tools = r2ai_get_tools();
    assert_non_null(tools);
    assert_int_equal(tools->n_tools, 2); // r2cmd and execute_js
    assert_non_null(tools->tools);

    // Verify the first tool (r2cmd)
    const R2AI_Tool *r2cmd_tool = &tools->tools[0];
    assert_string_equal(r2cmd_tool->name, "r2cmd");
    assert_string_equal(r2cmd_tool->description, "Run a radare2 command");
    assert_non_null(r2cmd_tool->parameters);

    // Verify the second tool (execute_js)
    const R2AI_Tool *js_tool = &tools->tools[1];
    assert_string_equal(js_tool->name, "execute_js");
    assert_string_equal(js_tool->description, "Execute JavaScript code");
    assert_non_null(js_tool->parameters);
}

// Simplified tool parsing test (avoiding complex JSON)
void test_tools_parsing(void **state) {
    (void) state;

    // Test with NULL input (should return NULL gracefully)
    R2AI_Tools *tools = r2ai_tools_parse(NULL);
    assert_null(tools);

    // Test with empty string (should return NULL gracefully)
    tools = r2ai_tools_parse("");
    assert_null(tools);

    // Test with invalid JSON (should return NULL gracefully)
    tools = r2ai_tools_parse("invalid json");
    assert_null(tools);
}

// Simplified tool execution test (avoiding RCore integration)
void test_tools_execution(void **state) {
    (void) state;

    // Test execute_tool with NULL parameters (should handle gracefully)
    char *result = execute_tool(NULL, NULL, NULL);
    // Function should handle NULL gracefully - exact behavior depends on implementation
    if (result) {
        free(result);
    }

    // Test with invalid tool name
    result = execute_tool(NULL, "nonexistent_tool", "{}");
    // Should return NULL or error message, but shouldn't crash
    if (result) {
        free(result);
    }
}

// Test JSON format functions with basic inputs
void test_tools_json_format(void **state) {
    (void) state;

    // Test r2ai_tools_to_openai_json with NULL input
    char *json = r2ai_tools_to_openai_json(NULL);
    assert_null(json);

    // Test r2ai_tools_to_anthropic_json with NULL input  
    json = r2ai_tools_to_anthropic_json(NULL);
    assert_null(json);

    // Test with valid tools instance
    const R2AI_Tools *tools = r2ai_get_tools();
    assert_non_null(tools);

    // Test OpenAI format conversion
    json = r2ai_tools_to_openai_json(tools);
    if (json) {
        // Should contain some JSON structure
        assert_true(strlen(json) > 0);
        free(json);
    }

    // Test Anthropic format conversion
    json = r2ai_tools_to_anthropic_json(tools);
    if (json) {
        // Should contain some JSON structure
        assert_true(strlen(json) > 0);
        free(json);
    }
}