#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../r2ai.h"

// Mock RCore for testing
typedef struct {
    int dummy;
} MockRCore;

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
    assert_string_equal(js_tool->description, "Execute a JavaScript script in a quickjs environment. Only what you console.log will be returned.");
    assert_non_null(js_tool->parameters);
}

// Test: Tools parsing from JSON
void test_tools_parsing(void **state) {
    (void) state;

    // Test parsing valid OpenAI tools JSON
    const char *openai_tools_json = "[\
        {\
            \"type\": \"function\",\
            \"function\": {\
                \"name\": \"test_tool\",\
                \"description\": \"A test tool\",\
                \"parameters\": {\
                    \"type\": \"object\",\
                    \"properties\": {\
                        \"input\": {\
                            \"type\": \"string\",\
                            \"description\": \"Test input\"\
                        }\
                    },\
                    \"required\": [\"input\"]\
                }\
            }\
        },\
        {\
            \"type\": \"function\",\
            \"function\": {\
                \"name\": \"another_tool\",\
                \"description\": \"Another test tool\"\
            }\
        }\
    ]";

    R2AI_Tools *parsed_tools = r2ai_tools_parse(openai_tools_json);
    assert_non_null(parsed_tools);
    assert_int_equal(parsed_tools->n_tools, 2);

    // Verify first tool
    const R2AI_Tool *tool1 = &parsed_tools->tools[0];
    assert_string_equal(tool1->name, "test_tool");
    assert_string_equal(tool1->description, "A test tool");
    assert_non_null(tool1->parameters);

    // Verify second tool
    const R2AI_Tool *tool2 = &parsed_tools->tools[1];
    assert_string_equal(tool2->name, "another_tool");
    assert_string_equal(tool2->description, "Another test tool");

    r2ai_tools_free(parsed_tools);

    // Test parsing invalid JSON
    R2AI_Tools *invalid_tools = r2ai_tools_parse("invalid json");
    assert_null(invalid_tools);

    // Test parsing NULL
    R2AI_Tools *null_tools = r2ai_tools_parse(NULL);
    assert_null(null_tools);

    // Test parsing empty array
    R2AI_Tools *empty_tools = r2ai_tools_parse("[]");
    assert_non_null(empty_tools);
    assert_int_equal(empty_tools->n_tools, 0);
    r2ai_tools_free(empty_tools);
}

// Test: Tool execution (mocked)
void test_tools_execution(void **state) {
    (void) state;

    // Note: Since execute_tool requires an actual RCore instance,
    // we'll test the argument validation and basic structure
    
    // Test with NULL parameters
    char *result = execute_tool(NULL, NULL, NULL);
    assert_non_null(result);
    assert_true(strstr(result, "Tool name or arguments are NULL") != NULL);
    free(result);

    // Test with invalid JSON arguments
    result = execute_tool(NULL, "test_tool", "invalid json");
    assert_non_null(result);
    assert_true(strstr(result, "Invalid JSON arguments") != NULL);
    free(result);

    // Test with valid JSON but unknown tool
    result = execute_tool(NULL, "unknown_tool", "{\"param\":\"value\"}");
    assert_non_null(result);
    assert_true(strstr(result, "Unknown tool") != NULL);
    free(result);

    // Test r2cmd arguments validation
    const char *valid_r2cmd_args = "{\"command\":\"pdf\"}";
    const char *invalid_r2cmd_args = "{\"invalid\":\"param\"}";
    
    // These would require a real RCore to test fully, but we can at least
    // verify the JSON parsing works
    RJson *valid_json = r_json_parse((char*)valid_r2cmd_args);
    assert_non_null(valid_json);
    
    const RJson *command_json = r_json_get(valid_json, "command");
    assert_non_null(command_json);
    assert_string_equal(command_json->str_value, "pdf");
    
    r_json_free(valid_json);

    // Test JavaScript execution arguments
    const char *valid_js_args = "{\"script\":\"console.log('hello');\"}";
    RJson *js_json = r_json_parse((char*)valid_js_args);
    assert_non_null(js_json);
    
    const RJson *script_json = r_json_get(js_json, "script");
    assert_non_null(script_json);
    assert_string_equal(script_json->str_value, "console.log('hello');");
    
    r_json_free(js_json);
}

// Test: JSON format conversion
void test_tools_json_format(void **state) {
    (void) state;

    // Get the global tools and test conversion to OpenAI format
    const R2AI_Tools *tools = r2ai_get_tools();
    assert_non_null(tools);

    char *openai_json = r2ai_tools_to_openai_json(tools);
    assert_non_null(openai_json);
    
    // Verify the JSON contains expected elements
    assert_true(strstr(openai_json, "\"type\":\"function\"") != NULL);
    assert_true(strstr(openai_json, "\"name\":\"r2cmd\"") != NULL);
    assert_true(strstr(openai_json, "\"name\":\"execute_js\"") != NULL);
    assert_true(strstr(openai_json, "Run a radare2 command") != NULL);
    
    free(openai_json);

    // Test conversion to Anthropic format
    char *anthropic_json = r2ai_tools_to_anthropic_json(tools);
    assert_non_null(anthropic_json);
    
    // Verify Anthropic format structure
    assert_true(strstr(anthropic_json, "\"name\":\"r2cmd\"") != NULL);
    assert_true(strstr(anthropic_json, "\"name\":\"execute_js\"") != NULL);
    assert_true(strstr(anthropic_json, "\"input_schema\"") != NULL);
    
    free(anthropic_json);

    // Test with NULL tools
    char *null_openai = r2ai_tools_to_openai_json(NULL);
    assert_null(null_openai);

    char *null_anthropic = r2ai_tools_to_anthropic_json(NULL);
    assert_null(null_anthropic);

    // Test with empty tools structure
    R2AI_Tools empty_tools = { .tools = NULL, .n_tools = 0 };
    char *empty_openai = r2ai_tools_to_openai_json(&empty_tools);
    assert_null(empty_openai);

    char *empty_anthropic = r2ai_tools_to_anthropic_json(&empty_tools);
    assert_null(empty_anthropic);

    // Test roundtrip: OpenAI JSON -> parse -> OpenAI JSON
    const char *original_json = "[\
        {\
            \"type\": \"function\",\
            \"function\": {\
                \"name\": \"test_roundtrip\",\
                \"description\": \"Test roundtrip conversion\",\
                \"parameters\": {\
                    \"type\": \"object\",\
                    \"properties\": {\
                        \"param1\": {\
                            \"type\": \"string\"\
                        }\
                    }\
                }\
            }\
        }\
    ]";

    R2AI_Tools *parsed = r2ai_tools_parse(original_json);
    assert_non_null(parsed);
    assert_int_equal(parsed->n_tools, 1);

    char *regenerated_json = r2ai_tools_to_openai_json(parsed);
    assert_non_null(regenerated_json);
    
    // Verify key elements are preserved
    assert_true(strstr(regenerated_json, "test_roundtrip") != NULL);
    assert_true(strstr(regenerated_json, "Test roundtrip conversion") != NULL);
    
    free(regenerated_json);
    r2ai_tools_free(parsed);

    // Test tools_free with NULL
    r2ai_tools_free(NULL); // Should not crash
}