#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clenzdat.h"

void test_create_dataframe() {
    printf("Testing create_dataframe()...\n");
    DataFrame *df = create_dataframe();
    if (df != NULL) {
        printf("DataFrame created successfully.\n");
        free_dataframe(df);
    } else {
        printf("Failed to create DataFrame.\n");
    }
}

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

    for (int i = 0; i < 50; i++) {
        int_data[i] = i + 1;
        float_data[i] = (float)(i + 1) / 2.0f;
        char *str = malloc(20 * sizeof(char));
        snprintf(str, 20, "Row_%d", i + 1);
        string_data[i] = str;
    }

    // Add columns to the DataFrame
    add_column(df, "Int Column", TYPE_INT, int_data, 50);
    add_column(df, "Float Column", TYPE_FLOAT, float_data, 50);
    add_column(df, "String Column", TYPE_STRING, string_data, 50);

    // Print the DataFrame
    printf("\nLarge DataFrame (50 rows):\n");
    print_dataframe(df);

    // Print shape information
    int *shape = shape_df(df);
    printf("\nDataFrame shape: (%d, %d)\n", shape[0], shape[1]);

    // Clean up
    free_dataframe(df);
    free(int_data);
    free(float_data);
    for (int i = 0; i < 50; i++) {
        free(string_data[i]);
    }
    free(string_data);
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
    DataFrame *df = read_csv("sample_data.csv");
    if (df == NULL) {
        printf("Failed to read CSV file.\n");
        return;
    }
    printf("\nDescription of DataFrame:\n");
    print_dataframe(df);
    printf("read successfully\n");
    printf("DataFrame shape: (%d, %d)\n", shape_df(df)[0], shape_df(df)[1]);
    free_dataframe(df);
}
int main() {
    test_create_dataframe();
    test_add_column();
    test_get_column_as_array();
    test_append_dataframe();
    test_large_dataframe();
    test_get_dataframe_range();  
    test_sort_dataframe();
    test_read_describe_dataframe();
    test();

    return 0;
}
