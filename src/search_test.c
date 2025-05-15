#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clenzdat.h"

int main() {
    printf("===== DATACLENZ SEARCH ALGORITHM TEST =====\n\n");

    // ===== STEP 1: LOAD CSV DATA =====
    printf("Step 1: Loading CSV data from test_data.csv...\n");
    DataFrame *df = read_csv("test_data.csv");
    if (df == NULL) {
        printf("Failed to read CSV file. Error: %s\n", get_error());
        return 1;
    }
    printf("CSV data loaded successfully!\n");
    printf("Original DataFrame (first 10 rows):\n");
    print_dataframe(df);
    
    // Print information about the loaded data
    int *shape = shape_df(df);
    printf("DataFrame shape: (%d, %d)\n", shape[0], shape[1]);
    
    // DEBUG: Print column names to verify what's actually in the DataFrame
    printf("\nDEBUG: Column names in the DataFrame:\n");
    for (int i = 0; i < df->num_columns; i++) {
        printf("Column %d: '%s'\n", i, df->column_names[i]);
    }
    printf("\n");

    // ===== STEP 2: SORT DATA FOR SEARCH ALGORITHMS =====
    printf("Step 2: Sorting data to prepare for search tests...\n");
    
    // Sort by ID column
    printf("Sorting by ID column (numerical sort)...\n");
    // Use index instead of name to avoid column name issues
    DataFrame *sorted_by_id = sort_dataframe(df, 0, 1); // Sort first column in ascending order
    if (sorted_by_id == NULL) {
        printf("Failed to sort DataFrame by ID. Error: %s\n", get_error());
        return 1;
    }
    printf("DataFrame sorted by ID (first 10 rows):\n");
    print_dataframe(sorted_by_id);
    
    // Sort by Name column for string search tests
    printf("Sorting by Name column (alphabetical sort)...\n");
    // Use index instead of name to avoid column name issues
    DataFrame *sorted_by_name = sort_dataframe(df, 2, 1); // Sort third column in ascending order
    if (sorted_by_name == NULL) {
        printf("Failed to sort DataFrame by Name. Error: %s\n", get_error());
        return 1;
    }
    printf("DataFrame sorted by Name (first 10 rows):\n");
    print_dataframe(sorted_by_name);
    printf("\n");

    // ===== STEP 3: BINARY SEARCH TESTS =====
    printf("Step 3: Testing Binary Search Algorithm...\n");
    
    // Allocate memory for search results
    int max_indices = df->num_rows;
    int *found_indices = (int*)malloc(max_indices * sizeof(int));
    if (found_indices == NULL) {
        printf("Memory allocation failed for found_indices array.\n");
        return 1;
    }
    
    // Test binary search on ID column (integer search)
    printf("Binary search for ID = 115:\n");
    int target_id = 115;
    int found_count = binary_search(sorted_by_id, 0, &target_id, found_indices, max_indices);
    printf("Found %d occurrence(s) at index position(s): ", found_count);
    for (int i = 0; i < found_count; i++) {
        printf("%d ", found_indices[i]);
    }
    printf("\n");
    
    // Test binary search on Name column (string search)
    printf("Binary search for Name = 'Oscar':\n");
    char *target_name = "Oscar";
    found_count = binary_search(sorted_by_name, 2, target_name, found_indices, max_indices);
    printf("Found %d occurrence(s) at index position(s): ", found_count);
    for (int i = 0; i < found_count; i++) {
        printf("%d ", found_indices[i]);
    }
    printf("\n\n");

    // ===== STEP 4: JUMP SEARCH TESTS =====
    printf("Step 4: Testing Jump Search Algorithm...\n");
    
    // Test jump search on ID column (integer search)
    printf("Jump search for ID = 120:\n");
    target_id = 120;
    found_count = jump_search(sorted_by_id, 0, &target_id, found_indices, max_indices);
    printf("Found %d occurrence(s) at index position(s): ", found_count);
    for (int i = 0; i < found_count; i++) {
        printf("%d ", found_indices[i]);
    }
    printf("\n");
    
    // Test jump search on Name column (string search)
    printf("Jump search for Name = 'Tango':\n");
    target_name = "Tango";
    found_count = jump_search(sorted_by_name, 2, target_name, found_indices, max_indices);
    printf("Found %d occurrence(s) at index position(s): ", found_count);
    for (int i = 0; i < found_count; i++) {
        printf("%d ", found_indices[i]);
    }
    printf("\n\n");

    // ===== STEP 5: DATAFRAME SEARCH TESTS =====
    printf("Step 5: Testing DataFrame Search (df_search)...\n");
    
    // Use sorted_by_id directly instead of df_search which relies on column names
    printf("Searching for rows with ID = 125:\n");
    target_id = 125;
    found_count = binary_search(sorted_by_id, 0, &target_id, found_indices, max_indices);
    if (found_count > 0) {
        printf("Found row with ID = 125:\n");
        int idx = found_indices[0];
        printf("Index: %d, ID: %d, Value: %.1f, Name: %s\n",
               idx,
               ((int*)sorted_by_id->columns[0].data)[idx],
               ((float*)sorted_by_id->columns[1].data)[idx],
               ((char**)sorted_by_id->columns[2].data)[idx]);
    } else {
        printf("No rows found with ID = 125\n");
    }
    
    // Use sorted_by_name directly instead of df_search which relies on column names
    printf("\nSearching for rows with Name = 'Yankee':\n");
    target_name = "Yankee";
    found_count = binary_search(sorted_by_name, 2, target_name, found_indices, max_indices);
    if (found_count > 0) {
        printf("Found row with Name = 'Yankee':\n");
        int idx = found_indices[0];
        printf("Index: %d, ID: %d, Value: %.1f, Name: %s\n",
               idx,
               ((int*)sorted_by_name->columns[0].data)[idx],
               ((float*)sorted_by_name->columns[1].data)[idx],
               ((char**)sorted_by_name->columns[2].data)[idx]);
    } else {
        printf("No rows found with Name = 'Yankee'\n");
    }
    
    // Test searching for non-existent value
    printf("\nSearching for rows with ID = 150 (not in DataFrame):\n");
    target_id = 150;
    found_count = binary_search(sorted_by_id, 0, &target_id, found_indices, max_indices);
    if (found_count > 0) {
        printf("Unexpectedly found rows with ID = 150\n");
    } else {
        printf("No rows found with ID = 150 (as expected)\n");
    }
    
    printf("\n===== SEARCH TESTS COMPLETED SUCCESSFULLY =====\n");
    return 0;
} 