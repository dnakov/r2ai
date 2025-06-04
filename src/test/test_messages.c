#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    // Verify the message was copied correctly
    R2AI_Message *stored_msg = &msgs->messages[0];
    assert_string_equal(stored_msg->role, "user");
    assert_string_equal(stored_msg->content, "Hello, AI!");
    assert_null(stored_msg->tool_call_id);
    assert_int_equal(stored_msg->n_tool_calls, 0);

    // Test adding an assistant message
    R2AI_Message assistant_msg = {
        .role = "assistant",
        .content = "Hello! How can I help you?",
        .tool_call_id = NULL,
        .tool_calls = NULL,
        .n_tool_calls = 0,
        .content_blocks = NULL,
        .reasoning_content = NULL
    };

    result = r2ai_msgs_add(msgs, &assistant_msg);
    assert_true(result);
    assert_int_equal(msgs->n_messages, 2);

    // Cleanup
    r2ai_msgs_free(msgs);
}

// Test: Messages lifecycle management
void test_messages_lifecycle(void **state) {
    (void) state;

    // Test conversation initialization
    r2ai_conversation_init();
    R2AI_Messages *conversation = r2ai_conversation_get();
    assert_non_null(conversation);

    // Test clearing messages
    R2AI_Message msg1 = {
        .role = "user",
        .content = "Test message 1",
        .tool_call_id = NULL,
        .tool_calls = NULL,
        .n_tool_calls = 0,
        .content_blocks = NULL,
        .reasoning_content = NULL
    };

    R2AI_Message msg2 = {
        .role = "assistant", 
        .content = "Test response 1",
        .tool_call_id = NULL,
        .tool_calls = NULL,
        .n_tool_calls = 0,
        .content_blocks = NULL,
        .reasoning_content = NULL
    };

    r2ai_msgs_add(conversation, &msg1);
    r2ai_msgs_add(conversation, &msg2);
    assert_int_equal(conversation->n_messages, 2);

    // Test clearing
    r2ai_msgs_clear(conversation);
    assert_int_equal(conversation->n_messages, 0);

    // Test delete last messages functionality
    r2ai_msgs_add(conversation, &msg1);
    r2ai_msgs_add(conversation, &msg2);
    assert_int_equal(conversation->n_messages, 2);

    r2ai_delete_last_messages(conversation, 1);
    assert_int_equal(conversation->n_messages, 1);

    // Cleanup
    r2ai_conversation_free();
}

// Test: Tool calls functionality
void test_messages_tool_calls(void **state) {
    (void) state;

    R2AI_Messages *msgs = r2ai_msgs_new();
    assert_non_null(msgs);

    // Add an assistant message first
    R2AI_Message assistant_msg = {
        .role = "assistant",
        .content = "I'll run a command for you.",
        .tool_call_id = NULL,
        .tool_calls = NULL,
        .n_tool_calls = 0,
        .content_blocks = NULL,
        .reasoning_content = NULL
    };

    bool result = r2ai_msgs_add(msgs, &assistant_msg);
    assert_true(result);

    // Add a tool call to the last message
    R2AI_ToolCall tool_call = {
        .name = "r2cmd",
        .arguments = "{\"command\":\"pdf\"}",
        .id = "call_123"
    };

    result = r2ai_msgs_add_tool_call(msgs, &tool_call);
    assert_true(result);

    // Verify the tool call was added
    R2AI_Message *last_msg = &msgs->messages[msgs->n_messages - 1];
    assert_int_equal(last_msg->n_tool_calls, 1);
    assert_non_null(last_msg->tool_calls);
    
    assert_string_equal(last_msg->tool_calls[0].name, "r2cmd");
    assert_string_equal(last_msg->tool_calls[0].arguments, "{\"command\":\"pdf\"}");
    assert_string_equal(last_msg->tool_calls[0].id, "call_123");

    // Add another tool call
    R2AI_ToolCall tool_call2 = {
        .name = "execute_js",
        .arguments = "{\"script\":\"console.log('hello')\"}",
        .id = "call_456"
    };

    result = r2ai_msgs_add_tool_call(msgs, &tool_call2);
    assert_true(result);
    assert_int_equal(last_msg->n_tool_calls, 2);

    // Add a tool response message
    R2AI_Message tool_response = {
        .role = "tool",
        .content = "Command executed successfully",
        .tool_call_id = "call_123",
        .tool_calls = NULL,
        .n_tool_calls = 0,
        .content_blocks = NULL,
        .reasoning_content = NULL
    };

    result = r2ai_msgs_add(msgs, &tool_response);
    assert_true(result);
    assert_int_equal(msgs->n_messages, 2);

    R2AI_Message *tool_msg = &msgs->messages[1];
    assert_string_equal(tool_msg->role, "tool");
    assert_string_equal(tool_msg->tool_call_id, "call_123");

    r2ai_msgs_free(msgs);
}

// Test: JSON parsing functionality
void test_messages_json_parsing(void **state) {
    (void) state;

    R2AI_Messages *msgs = r2ai_msgs_new();
    assert_non_null(msgs);

    // Test parsing a simple OpenAI response
    const char *openai_response = "{\
        \"choices\": [{\
            \"message\": {\
                \"role\": \"assistant\",\
                \"content\": \"Hello! I can help you analyze code.\"\
            }\
        }]\
    }";

    bool result = r2ai_msgs_from_response(msgs, openai_response);
    assert_true(result);
    assert_int_equal(msgs->n_messages, 1);

    R2AI_Message *parsed_msg = &msgs->messages[0];
    assert_string_equal(parsed_msg->role, "assistant");
    assert_string_equal(parsed_msg->content, "Hello! I can help you analyze code.");

    // Test parsing response with tool calls
    const char *tool_call_response = "{\
        \"choices\": [{\
            \"message\": {\
                \"role\": \"assistant\",\
                \"content\": null,\
                \"tool_calls\": [{\
                    \"id\": \"call_abc123\",\
                    \"type\": \"function\",\
                    \"function\": {\
                        \"name\": \"r2cmd\",\
                        \"arguments\": \"{\\\"command\\\":\\\"pdf\\\"}\"\
                    }\
                }]\
            }\
        }]\
    }";

    r2ai_msgs_clear(msgs);
    result = r2ai_msgs_from_response(msgs, tool_call_response);
    assert_true(result);
    assert_int_equal(msgs->n_messages, 1);

    parsed_msg = &msgs->messages[0];
    assert_string_equal(parsed_msg->role, "assistant");
    assert_int_equal(parsed_msg->n_tool_calls, 1);
    assert_string_equal(parsed_msg->tool_calls[0].name, "r2cmd");
    assert_string_equal(parsed_msg->tool_calls[0].id, "call_abc123");

    // Test converting messages to JSON
    R2AI_Message user_msg = {
        .role = "user",
        .content = "Analyze this function",
        .tool_call_id = NULL,
        .tool_calls = NULL,
        .n_tool_calls = 0,
        .content_blocks = NULL,
        .reasoning_content = NULL
    };

    r2ai_msgs_clear(msgs);
    r2ai_msgs_add(msgs, &user_msg);

    char *json_output = r2ai_msgs_to_json(msgs);
    assert_non_null(json_output);
    assert_true(strstr(json_output, "\"role\":\"user\"") != NULL);
    assert_true(strstr(json_output, "\"content\":\"Analyze this function\"") != NULL);
    
    free(json_output);

    // Test Anthropic format conversion
    char *anthropic_json = r2ai_msgs_to_anthropic_json(msgs);
    assert_non_null(anthropic_json);
    free(anthropic_json);

    r2ai_msgs_free(msgs);
}

// Test: Conversation management
void test_messages_conversation(void **state) {
    (void) state;

    // Test create_conversation helper
    R2AI_Messages *conversation = create_conversation("Hello, analyze this code");
    assert_non_null(conversation);
    assert_int_equal(conversation->n_messages, 1);
    
    R2AI_Message *msg = &conversation->messages[0];
    assert_string_equal(msg->role, "user");
    assert_string_equal(msg->content, "Hello, analyze this code");

    r2ai_msgs_free(conversation);

    // Test creating conversation with NULL message
    conversation = create_conversation(NULL);
    assert_non_null(conversation);
    assert_int_equal(conversation->n_messages, 0);

    r2ai_msgs_free(conversation);

    // Test array growth (beyond initial capacity)
    R2AI_Messages *large_msgs = r2ai_msgs_new();
    
    // Add more messages than initial capacity (8)
    for (int i = 0; i < 10; i++) {
        char content_buf[64];
        snprintf(content_buf, sizeof(content_buf), "Message number %d", i);
        
        R2AI_Message msg = {
            .role = "user",
            .content = content_buf,
            .tool_call_id = NULL,
            .tool_calls = NULL,
            .n_tool_calls = 0,
            .content_blocks = NULL,
            .reasoning_content = NULL
        };
        
        bool result = r2ai_msgs_add(large_msgs, &msg);
        assert_true(result);
    }
    
    assert_int_equal(large_msgs->n_messages, 10);
    assert_true(large_msgs->cap_messages > 8); // Should have grown
    
    // Verify all messages are still accessible
    for (int i = 0; i < 10; i++) {
        R2AI_Message *stored = &large_msgs->messages[i];
        assert_non_null(stored->content);
        
        char expected_buf[64];
        snprintf(expected_buf, sizeof(expected_buf), "Message number %d", i);
        assert_string_equal(stored->content, expected_buf);
    }
    
    r2ai_msgs_free(large_msgs);
}