#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "clenzdat.h"  // Include your DataFrame header file

void test_read_csv() {
    // Create a temporary CSV file
    const char *filename = "test.csv";
    FILE *file = fopen(filename, "w");
    assert(file != NULL);
    
    fprintf(file, "Name,Age,Score\n");
    fprintf(file, "Alice,25,85.5\n");
    fprintf(file, "Bob,30,90.0\n");
    fprintf(file, "Charlie,22,78.0\n");
    fclose(file);
    
    // Read the CSV file into a DataFrame
    DataFrame *df = read_csv(filename);
    assert(df != NULL);
    
    // Check DataFrame shape
    int *shape = shape_df(df);
    assert(shape[0] == 3); // 3 rows
    assert(shape[1] == 3); // 3 columns
    free(shape);
    
    // Check column names
    void *col_data;
    ColumnType col_type;
    col_data = get_column_as_array(df, 0, &col_type);
    assert(col_data != NULL);
    assert(col_type == TYPE_STRING);
    
    col_data = get_column_as_array(df, 1, &col_type);
    assert(col_data != NULL);
    assert(col_type == TYPE_INT);
    
    col_data = get_column_as_array(df, 2, &col_type);
    assert(col_data != NULL);
    assert(col_type == TYPE_FLOAT);
    
    // Print the dataframe for verification
    print_dataframe(df);
    
    // Cleanup
    free_dataframe(df);
    remove(filename);
    printf("test_read_csv passed.\n");
}

int main() {
    test_read_csv();
    return 0;
}
