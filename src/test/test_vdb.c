#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../r_vdb.h"

// Test: Basic VDB functionality
void test_vdb_basic(void **state) {
    (void) state;

    // Test creating a new VDB
    RVdb *db = r_vdb_new(16);
    assert_non_null(db);
    assert_int_equal(db->dimension, 16);
    assert_int_equal(db->size, 0);
    assert_non_null(db->tokens);
    assert_null(db->root); // Should be empty initially

    // Test with different dimensions
    RVdb *db32 = r_vdb_new(32);
    assert_non_null(db32);
    assert_int_equal(db32->dimension, 32);

    // Test with invalid dimension (edge case)
    RVdb *db_invalid = r_vdb_new(0);
    assert_non_null(db_invalid);
    assert_int_equal(db_invalid->dimension, 0);

    // Cleanup
    r_vdb_free(db);
    r_vdb_free(db32);
    r_vdb_free(db_invalid);

    // Test freeing NULL
    r_vdb_free(NULL); // Should not crash
}

// Test: VDB insertion
void test_vdb_insertion(void **state) {
    (void) state;

    RVdb *db = r_vdb_new(16);
    assert_non_null(db);

    // Test inserting text
    r_vdb_insert(db, "This is a test document");
    assert_int_equal(db->size, 1);
    assert_non_null(db->root);

    // Insert more documents
    r_vdb_insert(db, "Another test document");
    assert_int_equal(db->size, 2);

    r_vdb_insert(db, "Machine learning algorithms");
    assert_int_equal(db->size, 3);

    r_vdb_insert(db, "Natural language processing");
    assert_int_equal(db->size, 4);

    r_vdb_insert(db, "Vector databases and embeddings");
    assert_int_equal(db->size, 5);

    // Test inserting NULL text (should be handled gracefully)
    int size_before = db->size;
    r_vdb_insert(db, NULL);
    assert_int_equal(db->size, size_before); // Size should not change

    // Test inserting empty string
    r_vdb_insert(db, "");
    // Note: Depending on implementation, this might or might not increase size
    // The test verifies the function doesn't crash

    // Test inserting very long text
    char long_text[1000];
    memset(long_text, 'a', sizeof(long_text) - 1);
    long_text[sizeof(long_text) - 1] = '\0';
    r_vdb_insert(db, long_text);
    // Should handle long text without crashing

    r_vdb_free(db);

    // Test inserting into NULL database
    r_vdb_insert(NULL, "test"); // Should not crash
}

// Test: VDB query functionality
void test_vdb_query(void **state) {
    (void) state;

    RVdb *db = r_vdb_new(16);
    assert_non_null(db);

    // Insert test documents
    r_vdb_insert(db, "machine learning algorithms");
    r_vdb_insert(db, "natural language processing");
    r_vdb_insert(db, "computer vision techniques");
    r_vdb_insert(db, "deep neural networks");
    r_vdb_insert(db, "artificial intelligence research");

    // Test basic query
    RVdbResultSet *results = r_vdb_query(db, "machine learning", 3);
    assert_non_null(results);
    assert_true(results->size <= 3);
    assert_true(results->size > 0);

    // Verify results contain KDNodes
    for (int i = 0; i < results->size; i++) {
        assert_non_null(results->results[i].node);
        assert_non_null(results->results[i].node->text);
        assert_true(results->results[i].dist_sq >= 0.0f);
    }

    // Results should be sorted by distance (ascending)
    for (int i = 1; i < results->size; i++) {
        assert_true(results->results[i].dist_sq >= results->results[i-1].dist_sq);
    }

    r_vdb_result_free(results);

    // Test query with k=1
    results = r_vdb_query(db, "neural networks", 1);
    assert_non_null(results);
    assert_int_equal(results->size, 1);
    r_vdb_result_free(results);

    // Test query with k larger than database size
    results = r_vdb_query(db, "artificial intelligence", 10);
    assert_non_null(results);
    assert_true(results->size <= 5); // Should not exceed database size
    r_vdb_result_free(results);

    // Test query on empty database
    RVdb *empty_db = r_vdb_new(16);
    results = r_vdb_query(empty_db, "test", 5);
    assert_null(results); // Should return NULL for empty database

    r_vdb_free(empty_db);

    // Test invalid parameters
    results = r_vdb_query(NULL, "test", 5);
    assert_null(results);

    results = r_vdb_query(db, NULL, 5);
    assert_null(results);

    results = r_vdb_query(db, "test", 0);
    assert_null(results);

    results = r_vdb_query(db, "test", -1);
    assert_null(results);

    r_vdb_free(db);
}

// Test: K-NN search functionality
void test_vdb_knn_search(void **state) {
    (void) state;

    RVdb *db = r_vdb_new(8); // Use smaller dimension for testing
    assert_non_null(db);

    // Insert a variety of documents to test similarity
    const char *documents[] = {
        "cat animal pet domestic",
        "dog animal pet friendly",
        "car vehicle automobile transportation",
        "bike bicycle transportation sport",
        "apple fruit food red",
        "banana fruit food yellow",
        "computer technology digital electronic",
        "phone mobile device communication"
    };

    int num_docs = sizeof(documents) / sizeof(documents[0]);
    for (int i = 0; i < num_docs; i++) {
        r_vdb_insert(db, documents[i]);
    }

    assert_int_equal(db->size, num_docs);

    // Test similarity search for animal-related query
    RVdbResultSet *results = r_vdb_query(db, "pet animal", 3);
    assert_non_null(results);
    assert_true(results->size > 0);
    assert_true(results->size <= 3);

    // The first results should be animal-related documents
    // (exact matching depends on embedding algorithm, but structure should be correct)
    for (int i = 0; i < results->size; i++) {
        assert_non_null(results->results[i].node);
        assert_non_null(results->results[i].node->text);
        printf("Result %d: %s (dist_sq: %.4f)\n", 
               i, results->results[i].node->text, results->results[i].dist_sq);
    }

    r_vdb_result_free(results);

    // Test with different query
    results = r_vdb_query(db, "fruit food", 2);
    assert_non_null(results);
    assert_true(results->size > 0);
    assert_true(results->size <= 2);
    r_vdb_result_free(results);

    // Test exact match (should return the document itself with lowest distance)
    results = r_vdb_query(db, "computer technology digital electronic", 1);
    assert_non_null(results);
    assert_int_equal(results->size, 1);
    
    // The exact match should have very low distance (potentially 0.0)
    float exact_distance = results->results[0].dist_sq;
    assert_true(exact_distance < 0.1f); // Should be very close

    r_vdb_result_free(results);

    // Test cosine similarity properties
    // For normalized vectors: dist_sq = 2 - 2 * cos_sim
    // So cos_sim = 1 - (dist_sq / 2)
    results = r_vdb_query(db, "transportation vehicle", 4);
    assert_non_null(results);
    
    for (int i = 0; i < results->size; i++) {
        float dist_sq = results->results[i].dist_sq;
        float cos_sim = 1.0f - (dist_sq * 0.5f);
        
        // Cosine similarity should be between -1 and 1
        assert_true(cos_sim >= -1.1f && cos_sim <= 1.1f); // Small tolerance for floating point
        
        printf("Distance: %.4f, Cosine similarity: %.4f, Text: %s\n", 
               dist_sq, cos_sim, results->results[i].node->text);
    }

    r_vdb_result_free(results);

    // Test edge case: query with k equal to database size
    results = r_vdb_query(db, "test query", num_docs);
    assert_non_null(results);
    assert_int_equal(results->size, num_docs);
    
    // All documents should be returned, sorted by distance
    for (int i = 1; i < results->size; i++) {
        assert_true(results->results[i].dist_sq >= results->results[i-1].dist_sq);
    }
    
    r_vdb_result_free(results);

    r_vdb_free(db);
}

// Test result set management
void test_vdb_result_management(void **state) {
    (void) state;

    // Test result set creation and freeing
    r_vdb_result_free(NULL); // Should not crash

    RVdb *db = r_vdb_new(16);
    r_vdb_insert(db, "test document");
    
    RVdbResultSet *results = r_vdb_query(db, "test", 1);
    assert_non_null(results);
    assert_int_equal(results->capacity, 1);
    assert_int_equal(results->size, 1);
    assert_non_null(results->results);
    
    r_vdb_result_free(results);
    r_vdb_free(db);
}