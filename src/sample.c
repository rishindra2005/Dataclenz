#include <stdio.h>
#include <clenzdat.h>

int main() {
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame.\n");
        return 1;
    }

    int int_data[] = {1, 2, 3, 4, 5};
    float float_data[] = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f};
    char *string_data[] = {"one", "two", "three", "four", "five"};

    add_column(df, "Int Column", TYPE_INT, int_data, 5);
    add_column(df, "Float Column", TYPE_FLOAT, float_data, 5);
    add_column(df, "String Column", TYPE_STRING, string_data, 5);

    printf("Sample DataFrame:\n");
    print_dataframe(df);

    free_dataframe(df);
    return 0;
}