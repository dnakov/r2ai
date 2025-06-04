#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <r_core.h>
#include "../r2ai.h"

// Test: Basic Messages API functionality
void test_messages_api(void **state) {
    (void) state; // unused parameter

    // Test r2ai_msgs_new()
    R2AI_Messages *msgs = r2ai_msgs_new();
    assert_non_null(msgs);
    assert_int_equal(msgs->n_messages, 0);
    assert_int_equal(msgs->cap_messages, 8); // INITIAL_CAPACITY
    assert_non_null(msgs->messages);

    // Test adding a simple user message
    R2AI_Message msg = {
        .role = "user",
        .content = "Hello, AI!",
        .tool_call_id = NULL,
        .tool_calls = NULL,
        .n_tool_calls = 0,
        .content_blocks = NULL,
        .reasoning_content = NULL
    };

    bool result = r2ai_msgs_add(msgs, &msg);
    assert_true(result);
    assert_int_equal(msgs->n_messages, 1);

    // Verify the message was added correctly
    assert_string_equal(msgs->messages[0].role, "user");
    assert_string_equal(msgs->messages[0].content, "Hello, AI!");

    r2ai_msgs_free(msgs);
}

// Test: Message lifecycle
void test_messages_lifecycle(void **state) {
    (void) state;

    R2AI_Messages *msgs = r2ai_msgs_new();
    assert_non_null(msgs);

    // Add multiple messages
    for (int i = 0; i < 5; i++) {
        R2AI_Message msg = {
            .role = (i % 2 == 0) ? "user" : "assistant",
            .content = "Test message",
            .tool_call_id = NULL,
            .tool_calls = NULL,
            .n_tool_calls = 0,
            .content_blocks = NULL,
            .reasoning_content = NULL
        };
        
        bool result = r2ai_msgs_add(msgs, &msg);
        assert_true(result);
    }

    assert_int_equal(msgs->n_messages, 5);

    // Test clearing messages
    r2ai_msgs_clear(msgs);
    assert_int_equal(msgs->n_messages, 0);

    r2ai_msgs_free(msgs);
}

// Test: Tool calls
void test_messages_tool_calls(void **state) {
    (void) state;

    R2AI_Messages *msgs = r2ai_msgs_new();
    assert_non_null(msgs);

    // Add a basic message first
    R2AI_Message msg = {
        .role = "assistant",
        .content = NULL,
        .tool_call_id = NULL,
        .tool_calls = NULL,
        .n_tool_calls = 0,
        .content_blocks = NULL,
        .reasoning_content = NULL
    };

    bool result = r2ai_msgs_add(msgs, &msg);
    assert_true(result);

    // Add a tool call
    R2AI_ToolCall tc = {
        .name = "r2cmd",
        .arguments = "{\"command\": \"afl\"}",
        .id = "call_123"
    };

    result = r2ai_msgs_add_tool_call(msgs, &tc);
    assert_true(result);

    // Verify tool call was added
    assert_int_equal(msgs->messages[0].n_tool_calls, 1);
    assert_string_equal(msgs->messages[0].tool_calls[0].name, "r2cmd");

    r2ai_msgs_free(msgs);
}

// Simplified JSON parsing test (avoiding complex JSON)
void test_messages_json_parsing(void **state) {
    (void) state;

    R2AI_Messages *msgs = r2ai_msgs_new();
    assert_non_null(msgs);

    // Test with NULL input (should fail gracefully)
    bool result = r2ai_msgs_from_response(msgs, NULL);
    assert_false(result);

    // Test with empty string (should fail gracefully)  
    result = r2ai_msgs_from_response(msgs, "");
    assert_false(result);

    // Test with invalid JSON (should fail gracefully)
    result = r2ai_msgs_from_response(msgs, "invalid json");
    assert_false(result);

    r2ai_msgs_free(msgs);
}

// Test: Conversation management
void test_messages_conversation(void **state) {
    (void) state;

    // Test conversation initialization
    r2ai_conversation_init();
    
    R2AI_Messages *conversation = r2ai_conversation_get();
    assert_non_null(conversation);
    
    // Test that we get the same instance
    R2AI_Messages *conversation2 = r2ai_conversation_get();
    assert_ptr_equal(conversation, conversation2);
    
    r2ai_conversation_free();
}