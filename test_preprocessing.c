#include "clenzdat.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void print_float_column(DataFrame *df, int column_index) {
    if (df->columns[column_index].type != TYPE_FLOAT) {
        printf("Column is not of type float\n");
        return;
    }
    float *data = (float *)df->columns[column_index].data;
    for (int i = 0; i < df->num_rows; i++) {
        printf("%.2f ", data[i]);
    }
    printf("\n");
}

int main() {
    // Create a sample DataFrame
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame\n");
        return 1;
    }

    // Add a float column with some missing values (represented by NaN)
    float data[] = {1.0, 2.0, NAN, 4.0, 5.0, NAN, 7.0, 8.0, 9.0, 10.0};
    float data1[] = {1.0, 2.0, 7.2, 4.0, 5.0, 8.0, 7.0, 8.0, 9.0, 10.0};
    char *name[] = {"a", "b", "c", "b", "e", "a", "a", "h", "a", "j"};
    int num_rows = sizeof(data) / sizeof(data[0]);

    if (!add_column(df, "test_column", TYPE_FLOAT, data, num_rows)) {
        printf("Failed to add column1\n");
        free_dataframe(df);
        return 1;
    }
    if (!add_column(df, "test_column1", TYPE_FLOAT, data1, num_rows)) {
        printf("Failed to add column2\n");
        free_dataframe(df);
        return 1;
    }
    if (!add_column(df, "test_column2", TYPE_STRING, name, num_rows)) {
        printf("Failed to add column3\n");
        free_dataframe(df);
        return 1;
    }

    // Print original data
    printf("Original data:\n");
    print_dataframe(df);

    if (!write_csv(df, "null_test.csv")) {
        printf("Failed to write CSV: %s\n", error_message);
        free_dataframe(df);
        return 1;
    }
    printf("\nWritten to CSV\n");

    DataFrame *df_read = read_csv("null_test.csv");
    if (df_read == NULL) {
        printf("Failed to read CSV: %s\n", error_message);
        free_dataframe(df);
        return 1;
    }
    printf("\nRead from CSV\n");
    print_dataframe(df_read);

    // Test handle_missing_values
    printf("\nTesting handle_missing_values (mean strategy):\n");
    DataFrame *df_handled = handle_missing_values(df, 0, "mean");
    if (df_handled != NULL) {
        print_dataframe(df_handled);
    } else {
        printf("handle_missing_values failed: %s\n", error_message);
    }

    // Test normalize_column
    printf("\nTesting normalize_column:\n");
    DataFrame *df_normalized = normalize_column(df, 0);
    if (df_normalized != NULL) {
        print_float_column(df_normalized, 0);
        print_dataframe(df_normalized);
    } else {
        printf("normalize_column failed: %s\n", error_message);
    }

    // Test scale_column
    printf("\nTesting scale_column (scale to range [-1, 1]):\n");
    DataFrame *df_scaled = scale_column(df, 0, -1.0, 1.0);
    if (df_scaled != NULL) {
        print_float_column(df_scaled, 0);
        print_dataframe(df_scaled);
    } else {
        printf("scale_column failed: %s\n", error_message);
    }

    // Clean up
    free_dataframe(df);
    if (df_handled != NULL && df_handled != df) free_dataframe(df_handled);
    if (df_normalized != NULL) free_dataframe(df_normalized);
    if (df_scaled != NULL) free_dataframe(df_scaled);

    return 0;
}