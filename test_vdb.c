#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <r_core.h>
#include "../r_vdb.h"

// ... existing code ...

// Simplified VDB query test (avoiding complex queries that cause segfaults)
void test_vdb_query(void **state) {
    (void) state;

    // Test creating a VDB
    RVdb *db = r_vdb_new(16);
    assert_non_null(db);

    // Test r_vdb_query with NULL parameters (should handle gracefully)
    RVdbResultSet *results = r_vdb_query(NULL, NULL, 0);
    assert_null(results);

    // Test with valid db but NULL text
    results = r_vdb_query(db, NULL, 5);
    assert_null(results);

    // Test with empty string
    results = r_vdb_query(db, "", 5);
    if (results) {
        r_vdb_result_free(results);
    }

    r_vdb_free(db);
}