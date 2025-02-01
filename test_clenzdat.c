#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "clenzdat.h"
// #include <clenzdat.h>

void test_add_column() {
    printf("\nTesting add_column()...\n");
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame.\n");
        return;
    }

    int int_data[] = {1, 2, 3, 4, 5};
    float float_data[] = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f};
    char *string_data[] = {"one", "two", "three", "four", "five"};

    if (add_column(df, "Int Column", TYPE_INT, int_data, 5) &&
        add_column(df, "Float Column", TYPE_FLOAT, float_data, 5) &&
        add_column(df, "String Column", TYPE_STRING, string_data, 5)) {
        printf("Columns added successfully.\n");
        print_dataframe(df);

        int *shape = shape_df(df);
        printf("\nDataFrame shape: (%d, %d)\n", shape[0], shape[1]);

        printf("Number of rows: %d\n", shape[0]);
        printf("Number of columns: %d\n", shape[1]);

        printf("\nColumn Information:\n");
        for (int i = 0; i < df->num_columns; i++) {
            printf("Column %d: %s (", i + 1, df->column_names[i]);
            switch (df->columns[i].type) {
                case TYPE_INT:
                    printf("Integer");
                    break;
                case TYPE_FLOAT:
                    printf("Float");
                    break;
                case TYPE_STRING:
                    printf("String");
                    break;
                default:
                    printf("Unknown Type");
            }
            printf(")\n");
        }
    } else {
        printf("Failed to add columns.\n");
    }

    free_dataframe(df);
}



void test_get_column_as_array() {
    printf("\nTesting get_column_as_array()...\n");
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame.\n");
        return;
    }

    int int_data[] = {1, 2, 3, 4, 5};
    float float_data[] = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f};
    char *string_data[] = {"one", "two", "three", "four", "five"};

    add_column(df, "Int Column", TYPE_INT, int_data, 5);
    add_column(df, "Float Column", TYPE_FLOAT, float_data, 5);
    add_column(df, "String Column", TYPE_STRING, string_data, 5);

    ColumnType type;
    void *array;

    // Test int column
    array = get_column_as_array(df, 0, &type);
    if (array != NULL && type == TYPE_INT) {
        printf("Int column retrieved successfully:\n");
        for (int i = 0; i < 5; i++) {
            printf("%d ", ((int*)array)[i]);
        }
        printf("\n");
        free(array);
    } else {
        printf("Failed to retrieve int column.\n");
    }

    // Test float column
    array = get_column_as_array(df, 1, &type);
    if (array != NULL && type == TYPE_FLOAT) {
        printf("Float column retrieved successfully:\n");
        for (int i = 0; i < 5; i++) {
            printf("%.1f ", ((float*)array)[i]);
        }
        printf("\n");
        free(array);
    } else {
        printf("Failed to retrieve float column.\n");
    }

    // Test string column
    array = get_column_as_array(df, 2, &type);
    if (array != NULL && type == TYPE_STRING) {
        printf("String column retrieved successfully:\n");
        for (int i = 0; i < 5; i++) {
            printf("%s ", ((char**)array)[i]);
        }
        printf("\n");
        free(array);
    } else {
        printf("Failed to retrieve string column.\n");
    }

    free_dataframe(df);
}
void test_append_dataframe() {
    printf("\nTesting append_dataframe()...\n");
    DataFrame *df1 = create_dataframe();
    DataFrame *df2 = create_dataframe();
    if (df1 == NULL || df2 == NULL) {
        printf("Failed to create DataFrames.\n");
        return;
    }

    int int_data1[] = {1, 2, 3};
    float float_data1[] = {1.1f, 2.20000266666273f, 3.3f};
    char *string_data1[] = {"one", "two", "thredddddddjjjjjjjjjjjjjjjjdddddddde"};

    int int_data2[] = {4, 5};
    float float_data2[] = {4.4f, 5.5f};
    char *string_data2[] = {"four", "five"};

    add_column(df1, "Int Column", TYPE_INT, int_data1, 3);
    add_column(df1, "Float Column", TYPE_FLOAT, float_data1, 3);
    add_column(df1, "String Column", TYPE_STRING, string_data1, 3);

    add_column(df2, "Int Column", TYPE_INT, int_data2, 2);
    add_column(df2, "Float Column", TYPE_FLOAT, float_data2, 2);
    add_column(df2, "String Column", TYPE_STRING, string_data2, 2);

    printf("DataFrame 1:\n");
    print_dataframe(df1);
    printf("\nDataFrame 2:\n");
    print_dataframe(df2);

    DataFrame *result = append_dataframe(df1, df2);
    if (result != NULL) {
        printf("\nAppended DataFrame:\n");
        print_dataframe(result);
        free_dataframe(result);
    } else {
        printf("Failed to append DataFrames.\n");
    }
    int *shape = shape_df(result);
        printf("\nDataFrame shape: (%d, %d)\n", shape[0], shape[1]);
    free_dataframe(df1);
    free_dataframe(df2);
}
void test_change_value() {
    printf("\nTesting change_value()...\n");
    
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Test failed: Unable to create DataFrame\n");
        return;
    }

    // Add test columns
    int int_data[] = {1, 2, 3, 4, 5};
    float float_data[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    char *string_data[] = {"one", "two", "three", "four", "five"};

    if (!add_column(df, "Int Column", TYPE_INT, int_data, 5) ||
        !add_column(df, "Float Column", TYPE_FLOAT, float_data, 5) ||
        !add_column(df, "String Column", TYPE_STRING, string_data, 5)) {
        printf("Test failed: Unable to add columns to DataFrame\n");
        free_dataframe(df);
        return;
    }

    printf("Original DataFrame:\n");
    print_dataframe(df);

    // Test 1: Change integer value
    int new_int = 10;
    if (change_value(df, 2, 0, &new_int)) {
        printf("\nChanged integer value at row 2, column 0 to 10\n");
    } else {
        printf("Test failed: Unable to change integer value\n");
    }

    // Test 2: Change float value
    float new_float = 6.6;
    if (change_value(df, 3, 1, &new_float)) {
        printf("Changed float value at row 3, column 1 to 6.6\n");
    } else {
        printf("Test failed: Unable to change float value\n");
    }

    // Test 3: Change string value
    char *new_string = "six";
    if (change_value(df, 4, 2, new_string)) {
        printf("Changed string value at row 4, column 2 to 'six'\n");
    } else {
        printf("Test failed: Unable to change string value\n");
    }

    printf("\nUpdated DataFrame:\n");
    print_dataframe(df);

    // Test 4: Attempt to change integer with incompatible type (float)
    float incompatible_float = 7.7;
    if (!change_value(df, 0, 0, &incompatible_float)) {
        printf("\nCorrectly failed to change integer with incompatible float type\n");
    } else {
        printf("Test failed: Changed integer with incompatible float type\n");
    }

    // Test 5: Attempt to change with invalid indices
    if (!change_value(df, 10, 0, &new_int) && !change_value(df, 0, 10, &new_int)) {
        printf("Correctly failed to change value with invalid indices\n");
    } else {
        printf("Test failed: Changed value with invalid indices\n");
    }
    

    free_dataframe(df);
}
void test_large_dataframe() {
    printf("Testing large DataFrame (>30 rows)...\n");
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame.\n");
        return;
    }

    // Create large arrays for each column type
    int *int_data = malloc(50 * sizeof(int));
    float *float_data = malloc(50 * sizeof(float));
    char **string_data = malloc(50 * sizeof(char*));

    if (int_data == NULL || float_data == NULL || string_data == NULL) {
        printf("Memory allocation failed for data arrays.\n");
        free(int_data);
        free(float_data);
        free(string_data);
        free_dataframe(df);
        return;
    }

    for (int i = 0; i < 50; i++) {
        int_data[i] = i + 1;
        float_data[i] = (float)(i + 1) / 2.0f;
        char *str = malloc(20 * sizeof(char));
        if (str == NULL) {
            printf("Memory allocation failed for string at index %d.\n", i);
            // Clean up previously allocated memory
            for (int j = 0; j < i; j++) {
                free(string_data[j]);
            }
            free(int_data);
            free(float_data);
            free(string_data);
            free_dataframe(df);
            return;
        }
        snprintf(str, 20, "Row_%d", i + 1);
        string_data[i] = str;
    }

    // Add columns to the DataFrame
    if (!add_column(df, "Int Column", TYPE_INT, int_data, 50) ||
        !add_column(df, "Float Column", TYPE_FLOAT, float_data, 50) ||
        !add_column(df, "String Column", TYPE_STRING, string_data, 50)) {
        printf("Failed to add columns to DataFrame.\n");
        // Clean up
        
        free(int_data);
        free(float_data);
        free_dataframe(df);
        return;
    }

    // Print the DataFrame
    printf("\nLarge DataFrame (50 rows):\n");
    print_dataframe(df);

    // Print shape information
    int *shape = shape_df(df);
    if (shape != NULL) {
        printf("\nDataFrame shape: (%d, %d)\n", shape[0], shape[1]);
        free(shape);
    } else {
        printf("Failed to get DataFrame shape.\n");
    }


    // Clean up
    free_dataframe(df);
    printf("Freed DataFrame\n");

    free(int_data);
    printf("Freed int_data\n");

    free(float_data);
    printf("Freed float_data\n");

    

    printf("Test large DataFrame completed successfully\n");
}

void test_get_dataframe_range() {
    printf("\nTesting get_dataframe_range()...\n");
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame.\n");
        return;
    }

    int int_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    float float_data[] = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f, 9.9f, 10.10f};
    char *string_data[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};

    add_column(df, "Int Column", TYPE_INT, int_data, 10);
    add_column(df, "Float Column", TYPE_FLOAT, float_data, 10);
    add_column(df, "String Column", TYPE_STRING, string_data, 10);

    printf("Original DataFrame:\n");
    print_dataframe(df);

    DataFrame *range_df = get_dataframe_range(df, 2, 6);
    if (range_df != NULL) {
        printf("\nDataFrame range (rows 2-6):\n");
        print_dataframe(range_df);

        int *shape = shape_df(range_df);
        printf("\nRange DataFrame shape: (%d, %d)\n", shape[0], shape[1]);

        free_dataframe(range_df);
    } else {
        printf("Failed to get DataFrame range.\n");
    }

    free_dataframe(df);
}
void test_sort_dataframe() {
    // Create a sample DataFrame
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame\n");
        return;
    }

    // Add columns
    int ages[] = {30, 25, 40, 35, 28};
    float salaries[] = {50000.0, 45000.0, 60000.0, 55000.0, 48000.0};
    char *names[] = {"Alice", "Bob", "Charlie", "David", "Eve"};

    add_column(df, "Age", TYPE_INT, ages, 5);
    add_column(df, "Salary", TYPE_FLOAT, salaries, 5);
    add_column(df, "Name", TYPE_STRING, names, 5);

    // Print original DataFrame
    printf("Original DataFrame:\n");
    print_dataframe(df);

    // Sort by Age (ascending)
    DataFrame *sorted_by_age = sort_dataframe(df, 0, 1);
    if (sorted_by_age != NULL) {
        printf("\nSorted by Age (ascending):\n");
        print_dataframe(sorted_by_age);
        free_dataframe(sorted_by_age);
    } else {
        printf("Failed to sort by Age\n");
    }

    // Sort by Salary (descending)
    DataFrame *sorted_by_salary = sort_dataframe(df, 1, 0);
    if (sorted_by_salary != NULL) {
        printf("\nSorted by Salary (descending):\n");
        print_dataframe(sorted_by_salary);
        free_dataframe(sorted_by_salary);
    } else {
        printf("Failed to sort by Salary\n");
    }

    // Sort by Name (ascending)
    DataFrame *sorted_by_name = sort_dataframe(df, 1, 1);
    if (sorted_by_name != NULL) {
        printf("\nSorted by Name (ascending):\n");
        print_dataframe(sorted_by_name);
        free_dataframe(sorted_by_name);
    } else {
        printf("Failed to sort by Name\n");
    }
    
    // Clean up
    free_dataframe(df);
}
void test_read_describe_dataframe() {
    printf("\nTesting read_describe_dataframe()...\n");
    DataFrame *df = read_csv("sample_data.csv");
    if (df == NULL) {
        printf("Failed to read CSV file.\n");
        return;
    }
    print_dataframe(df);
    DataFrame *description = describe_dataframe(df);
    if (description != NULL) {
        printf("\nDescription of DataFrame:\n");
        print_dataframe(description);
        write_csv(description, "description.csv");
        free_dataframe(description);
    } else {
        printf("Failed to generate description.\n");
    }
    free_dataframe(df);
}
void test(){
    clock_t start, end;
    float cpu_time_used;
    start = clock();
    DataFrame *df = read_csv("customers-100.csv");
    
    if (df == NULL) {
        printf("Failed to read CSV file.\n");
        return;
    }
    end = clock();
    cpu_time_used = ((float) (end - start)) / CLOCKS_PER_SEC;
    printf("Reading time: %f seconds\n", cpu_time_used);
    printf("\nDescription of DataFrame:\n");
    print_dataframe(df);
    printf("read successfully\n");
    printf("DataFrame shape: (%d, %d)\n", shape_df(df)[0], shape_df(df)[1]);
    
    // Save DataFrame to txt file
    if (print_dataframe_s(df, "dataframe_output.txt")) {
        printf("DataFrame saved to dataframe_output.txt\n");
    } else {
        printf("Failed to save DataFrame to txt file\n");
    }
    printf("Reading time: %f seconds\n", cpu_time_used);
    if (delete_column(df, 3)) {
        printf("Column deleted successfully.\n");
        print_dataframe(df);
        print_dataframe_s(df, "dataframe_output.txt");
    } else {
        printf("Failed to delete column:\n");
    }
    printf("\nAll tests for change_value() completed.\n");
    free_dataframe(df);
}
void test_replace_value() {
    printf("\nTesting replace_value()...\n");
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame.\n");
        return;
    }

    int int_data[] = {1, 2, 3, 2, 5};
    float float_data[] = {1.1f, 2.2f, 3.3f, 2.2f, 5.5f};
    char *string_data[] = {"one", "two", "three", "two", "five"};

    add_column(df, "Int Column", TYPE_INT, int_data, 5);
    add_column(df, "Float Column", TYPE_FLOAT, float_data, 5);
    add_column(df, "String Column", TYPE_STRING, string_data, 5);

    printf("Original DataFrame:\n");
    print_dataframe(df);
    df = read_csv("sample_data.csv");
    // Replace int value
    int old_int = 30, new_int = 60;
    int replaced_int = replace_value(df, 1, &old_int, &new_int);
    printf("\nReplaced %d occurrences of %d with %d in Int Column\n", replaced_int, old_int, new_int);

    // Replace float value
    float old_float = 5.5f, new_float = 20.2f;
    int replaced_float = replace_value(df, 4, &old_float, &new_float);
    printf("Replaced %d occurrences of %.1f with %.1f in Float Column\n", replaced_float, old_float, new_float);

    // Replace string value
    char *old_str = "Sales", *new_str = "1";
    int replaced_str = replace_value(df, 3, old_str, new_str);
    printf("Replaced %d occurrences of '%s' with '%s' in String Column\n", replaced_str, old_str, new_str);

    printf("\nUpdated DataFrame:\n");
    print_dataframe(df);

    free_dataframe(df);
}
void test_print_unique_values() {
    printf("\nTesting print_unique_values()...\n");
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame.\n");
        return;
    }

    int int_data[] = {1, 2, 3, 2, 1, 4, 5, 4, 3};
    float float_data[] = {1.1f, 2.2f, 3.3f, 2.2f, 1.1f, 4.4f, 5.5f, 4.4f, 3.3f};
    char *string_data[] = {"one", "two", "three", "two", "one", "four", "five", "four", "three"};

    add_column(df, "Int Column", TYPE_INT, int_data, 9);
    add_column(df, "Float Column", TYPE_FLOAT, float_data, 9);
    add_column(df, "String Column", TYPE_STRING, string_data, 9);

    printf("Original DataFrame:\n");
    print_dataframe(df);
    df = read_csv("sample_data.csv");
    printf("\nUnique values in each column:\n");
    int unique_int = print_unique_values(df, 0);
    int unique_float = print_unique_values(df, 1);
    int unique_string = print_unique_values(df, 2);

    printf("\nNumber of unique values:\n");
    printf("Int Column: %d\n", unique_int);
    printf("Float Column: %d\n", unique_float);
    printf("String Column: %d\n", unique_string);

    free_dataframe(df);
}
void test_split_dataframe() {
    printf("\nTesting split_dataframe()...\n");

    // Read the housing.csv file
    DataFrame* df = read_csv("housing.csv");
    if (df == NULL) {
        printf("Failed to read housing.csv file.\n");
        return;
    }

    printf("Original DataFrame:\n");
    print_dataframe(df);
    
    // Split the DataFrame
    void* y;
    DataFrame* X = split_dataframe(df, "median_house_value", &y);

    if (X == NULL || y == NULL) {
        printf("Failed to split DataFrame.\n");
        free_dataframe(df);
        return;
    }

    printf("\nFeatures DataFrame (X):\n");
    print_dataframe(X);
    print_dataframe_s(X,"outputs/X.txt");
    delete_column(X, 8);    
    printf("\n");    

    printf("\nSplit DataFrame test completed.\n");



}



// void test_linear_regression() {
//     printf("\nTesting linear regression...\n");

//     // Read the housing.csv file
//     DataFrame* df = read_csv("large_sample.csv");
//     if (df == NULL) {
//         printf("Failed to read housing.csv file.\n");
//         return;
//     }
    

   
//     float* y;
//     DataFrame* X = split_dataframe(df, "result", (void**)&y);

//     if (X == NULL || y == NULL) {
//         printf("Failed to split DataFrame.\n");
//         free_dataframe(df);
//         return;
//     }

//     printf("X shape after split: (%d, %d)\n", X->num_rows, X->num_columns);

//     // Check for NaN or inf values in X and y
//     for (int i = 0; i < X->num_rows; i++) {
//         for (int j = 0; j < X->num_columns; j++) {
//             float value = ((float*)X->columns[j].data)[i];
//             if (isnan(value) || isinf(value)) {
//                 printf("Warning: NaN or inf value found in X at row %d, column %d\n", i, j);
//             }
//         }
//         if (isnan(y[i]) || isinf(y[i])) {
//             printf("Warning: NaN or inf value found in y at index %d\n", i);
//         }
//     }

//     // Create and fit the linear regression model
//     LinearRegressionModel* model = create_linear_regression_model(X->num_columns);
//     if (model == NULL) {
//         printf("Failed to create linear regression model.\n");
//         free_dataframe(df);
//         free_dataframe(X);
//         free(y);
//         return;
//     }

//     printf("Fitting linear regression model...\n");
//     clock_t start = clock();
//     if (!fit_linear_regression(model, X, y)) {
//         printf("Failed to fit linear regression model.\n");
//         free_linear_regression_model(model);
//         free_dataframe(df);
//         free_dataframe(X);
//         free(y);
//         return;
//     }
//     clock_t end  = clock();
//     double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

//     printf("Model fitted successfully.\n");
//     printf("Time taken to fit the model: %.4f seconds\n", cpu_time_used);

//     // Make predictions
//     float* y_pred = predict_linear_regression(model, X);
//     if (y_pred == NULL) {
//         printf("Failed to make predictions.\n");
//         free_linear_regression_model(model);
//         free_dataframe(df);
//         free_dataframe(X);
//         free(y);
//         return;
//     }

//     printf("Predictions made successfully.\n");

//     // Calculate and print accuracy metrics
//     float r_squared = calculate_r_squared(y, y_pred, X->num_rows);
//     float mse = calculate_mse(y, y_pred, X->num_rows);

//     printf("Linear Regression Results:\n");
//     printf("R-squared: %.4f\n", r_squared);
//     printf("Mean Squared Error: %.4f\n", mse);

//     // Print coefficients and intercept
//     // printf("Coefficients:\n");
//     // for (int i = 0; i < X->num_columns; i++) {
//     //     printf("%s: %.4f\n", X->column_names[i], model->coefficients[i]);
//     // }
//     // printf("Intercept: %.4f\n", model->intercept);    

//     // Clean up
//     free_linear_regression_model(model);
//     free_dataframe(df);
//     free_dataframe(X);
//     free(y);
//     free(y_pred);
//     printf("Linear regression test completed.\n");
//     scanf("%d");

// }
int main() {
    test_add_column();
    test_get_column_as_array();
    test_append_dataframe();
    test_large_dataframe();
    test_change_value();
    test_get_dataframe_range();  
    test_sort_dataframe();
    test_read_describe_dataframe();
    test_replace_value();  
    test_print_unique_values();
    test_split_dataframe();
    test_linear_regression();
    test();


    return 0;
}
