#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../markdown.h"

// Test: Basic markdown functionality
void test_markdown_basic(void **state) {
    (void) state;

    // Test with NULL input
    char *result = r2ai_markdown(NULL);
    assert_null(result);

    // Test with empty string
    result = r2ai_markdown("");
    assert_non_null(result);
    assert_string_equal(result, "");
    free(result);

    // Test basic text (no markdown)
    result = r2ai_markdown("Hello world");
    assert_non_null(result);
    assert_string_equal(result, "Hello world");
    free(result);

    // Test with whitespace
    result = r2ai_markdown("  Hello world  ");
    assert_non_null(result);
    // The exact behavior depends on implementation, but should not crash
    free(result);
}

// Test: Markdown formatting features
void test_markdown_formatting(void **state) {
    (void) state;

    // Test code blocks
    const char *code_block = "```c\nint main() {\n    return 0;\n}\n```";
    char *result = r2ai_markdown(code_block);
    assert_non_null(result);
    // Should handle code blocks without crashing
    // Exact formatting depends on implementation
    free(result);

    // Test inline code
    const char *inline_code = "Use the `printf` function for output.";
    result = r2ai_markdown(inline_code);
    assert_non_null(result);
    free(result);

    // Test headers
    const char *headers = "# Main Title\n## Subtitle\n### Sub-subtitle";
    result = r2ai_markdown(headers);
    assert_non_null(result);
    free(result);

    // Test emphasis
    const char *emphasis = "This is *italic* and **bold** text.";
    result = r2ai_markdown(emphasis);
    assert_non_null(result);
    free(result);

    // Test lists
    const char *lists = "- Item 1\n- Item 2\n  - Nested item\n- Item 3";
    result = r2ai_markdown(lists);
    assert_non_null(result);
    free(result);

    // Test mixed content
    const char *mixed = "# Analysis Results\n\n"
                       "The function contains:\n\n"
                       "- **Variables**: `int x, y`\n"
                       "- **Operations**: arithmetic\n\n"
                       "```c\n"
                       "int add(int a, int b) {\n"
                       "    return a + b;\n"
                       "}\n"
                       "```\n\n"
                       "This implements *basic addition*.";
    
    result = r2ai_markdown(mixed);
    assert_non_null(result);
    // Should handle complex markdown without crashing
    free(result);

    // Test markdown with special characters
    const char *special = "Function signature: `int func(char *str, size_t len)`\n"
                         "Returns: `0` on success, `-1` on error";
    result = r2ai_markdown(special);
    assert_non_null(result);
    free(result);

    // Test very long markdown
    char long_markdown[2000];
    strcpy(long_markdown, "# Long Document\n\n");
    for (int i = 0; i < 50; i++) {
        strcat(long_markdown, "This is a very long line of text. ");
    }
    strcat(long_markdown, "\n\n```c\nlong code;\n```");
    
    result = r2ai_markdown(long_markdown);
    assert_non_null(result);
    free(result);

    // Test markdown with unicode characters (if supported)
    const char *unicode = "# Unicode Test 🚀\n\nThis contains émojis and speciál characters.";
    result = r2ai_markdown(unicode);
    assert_non_null(result);
    free(result);

    // Test malformed markdown (should be handled gracefully)
    const char *malformed = "```\nUnclosed code block\n# Header inside code\n*Unclosed emphasis";
    result = r2ai_markdown(malformed);
    assert_non_null(result);
    // Should not crash even with malformed input
    free(result);

    // Test markdown with backslashes and escapes
    const char *escapes = "Escaped \\*asterisk\\* and \\`backtick\\`";
    result = r2ai_markdown(escapes);
    assert_non_null(result);
    free(result);
}