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

int main() {
    test_create_dataframe();
    test_add_column();
    test_get_column_as_array();
    test_append_dataframe();
    return 0;
}


