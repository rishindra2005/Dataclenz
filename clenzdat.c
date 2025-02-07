#include "clenzdat.h"
#include <stdarg.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>


#define NULL_INT INT_MIN
#define NULL_FLOAT NAN

char error_message[256] = {0};


/**
 * @brief Logs a debug message to stderr.
 *
 * This function prints a formatted debug message to the standard error stream.
 * It prepends "[DEBUG] " to the message and adds a newline at the end.
 *
 * @param format A printf-style format string for the debug message.
 * @param ... Variable arguments to be formatted according to the format string.
 *
 * @return This function does not return a value.
 */
void debug_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "[DEBUG] ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    fflush(stderr);
}

/**
 * @brief Sets an error message using a format string and variable arguments.
 *
 * This function formats an error message using the provided format string and
 * variable arguments, then stores it in the global error_message buffer.
 *
 * @param format A printf-style format string for the error message.
 * @param ... Variable arguments to be formatted according to the format string.
 *
 * @return This function does not return a value.
 */
void set_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(error_message, sizeof(error_message) - 1, format, args);
    va_end(args);
}



/**
 * @brief Creates a new DataFrame structure.
 *
 * This function allocates memory for a new DataFrame and initializes its members.
 * It sets up space for columns and column names, and initializes the row and column counts.
 *
 * @return DataFrame* A pointer to the newly created DataFrame, or NULL if memory allocation fails.
 *                    The caller is responsible for freeing this memory using free_dataframe().
 */
DataFrame* create_dataframe() {
    DataFrame *df = (DataFrame*)malloc(sizeof(DataFrame));
    if (df == NULL) {
        set_error("Failed to allocate memory for DataFrame");
        return NULL;
    }
    df->columns = (Column*)malloc(MAX_COLUMNS * sizeof(Column));
    df->column_names = (char**)malloc(MAX_COLUMNS * sizeof(char*));
    if (df->columns == NULL || df->column_names == NULL) {
        set_error("Failed to allocate memory for columns");
        free(df);
        return NULL;
    }
    df->num_columns = 0;
    df->num_rows = 0;
    df->max_rows = 1000;  // Start with 1000 rows, we'll resize as needed
    return df;
}

/**
 * @brief Resizes a DataFrame to a new number of rows.
 *
 * This function changes the number of rows in the DataFrame, reallocating memory
 * for each column and initializing new elements if the size is increased.
 *
 * @param df Pointer to the DataFrame to be resized.
 * @param new_size The new number of rows for the DataFrame.
 *
 * @return 1 if the resize operation was successful, 0 if an error occurred.
 */
int resize_dataframe(DataFrame *df, int new_size) {
    if (df == NULL || new_size < 0) {
        set_error("Invalid DataFrame or size");
        return 0;
    }

    for (int i = 0; i < df->num_columns; i++) {
        size_t elem_size = get_column_element_size(df->columns[i].type);
        void *new_data = realloc(df->columns[i].data, new_size * elem_size);
        if (new_data == NULL) {
            set_error("Memory reallocation failed for column %d", i);
            return 0;
        }
        df->columns[i].data = new_data;

        // If we're increasing the size, initialize new elements
        if (new_size > df->num_rows) {
            switch (df->columns[i].type) {
                case TYPE_INT:
                    memset((int*)new_data + df->num_rows, 0, (new_size - df->num_rows) * sizeof(int));
                    break;
                case TYPE_FLOAT:
                    for (int j = df->num_rows; j < new_size; j++) {
                        ((float*)new_data)[j] = 0.0f;
                    }
                    break;
                case TYPE_STRING:
                    for (int j = df->num_rows; j < new_size; j++) {
                        ((char**)new_data)[j] = strdup("");
                    }
                    break;
            }
        }
    }

    df->num_rows = new_size;
    return 1;
}



/**
 * @brief Frees all memory associated with a DataFrame.
 *
 * This function deallocates all memory used by the DataFrame, including its columns,
 * column names, and the DataFrame structure itself. For string-type columns,
 * it also frees the memory for each individual string.
 *
 * @param df Pointer to the DataFrame to be freed. If NULL, the function returns
 *           without doing anything.
 *
 * @return This function does not return a value.
 */
void free_dataframe(DataFrame *df) {
    if (df == NULL) return;
    for (int i = 0; i < df->num_columns; i++) {
        if (df->columns[i].type == TYPE_STRING) {
            for (int j = 0; j < df->num_rows; j++) {
                free(((char**)df->columns[i].data)[j]);
            }
        }
        free(df->columns[i].data);
        free(df->column_names[i]);
    }
    free(df->columns);
    free(df->column_names);
    free(df);
}



/**
 * @brief Adds a new column to the DataFrame.
 *
 * This function adds a new column to the given DataFrame with the specified name,
 * type, and data. It handles memory allocation for the new column and its data.
 *
 * @param df Pointer to the DataFrame to which the column will be added.
 * @param name The name of the new column.
 * @param type The data type of the new column (TYPE_INT, TYPE_FLOAT, or TYPE_STRING).
 * @param data Pointer to the data to be added to the new column.
 * @param length The number of elements in the data array.
 *
 * @return 1 if the column was successfully added, 0 if an error occurred.
 */
int add_column(DataFrame *df, const char *name, ColumnType type, void *data, int length) {
    if (df == NULL) {
        set_error("DataFrame is NULL");
        return 0;
    }
    if (df->num_columns >= MAX_COLUMNS) {
        set_error("Maximum number of columns reached");
        return 0;
    }

    df->column_names[df->num_columns] = strdup(name);
    if (df->column_names[df->num_columns] == NULL) {
        set_error("Failed to allocate memory for column name");
        return 0;
    }
    df->columns[df->num_columns].type = type;
    df->columns[df->num_columns].length = length;

    size_t data_size;
    switch (type) {
        case TYPE_INT:
            data_size = sizeof(int) * length;
            df->columns[df->num_columns].data = malloc(data_size);
            if (df->columns[df->num_columns].data == NULL) {
                set_error("Failed to allocate memory for column data");
                free(df->column_names[df->num_columns]);
                return 0;
            }
            for (int i = 0; i < length; i++) {
                if (((int*)data)[i] == NULL_INT) {
                    ((int*)df->columns[df->num_columns].data)[i] = NULL_INT;
                } else {
                    ((int*)df->columns[df->num_columns].data)[i] = ((int*)data)[i];
                }
            }
            break;
        case TYPE_FLOAT:
            data_size = sizeof(float) * length;
            break;
        case TYPE_STRING:
            data_size = sizeof(char*) * length;
            df->columns[df->num_columns].data = malloc(data_size);
            if (df->columns[df->num_columns].data == NULL) {
                set_error("Failed to allocate memory for column data");
                free(df->column_names[df->num_columns]);
                return 0;
            }
            for (int i = 0; i < length; i++) {
                char *str = ((char**)data)[i];
                if (str == NULL) {
                    ((char**)df->columns[df->num_columns].data)[i] = NULL;
                } else {
                    ((char**)df->columns[df->num_columns].data)[i] = strdup(str);
                    if (((char**)df->columns[df->num_columns].data)[i] == NULL) {
                        set_error("Failed to allocate memory for string data");
                        // Clean up previously allocated strings
                        for (int j = 0; j < i; j++) {
                            free(((char**)df->columns[df->num_columns].data)[j]);
                        }
                        free(df->columns[df->num_columns].data);
                        free(df->column_names[df->num_columns]);
                        return 0;
                    }
                }
            }
            break;
        default:
            set_error("Invalid column type");
            free(df->column_names[df->num_columns]);
            return 0;
    }

    df->columns[df->num_columns].data = malloc(data_size);
    if (df->columns[df->num_columns].data == NULL) {
        set_error("Failed to allocate memory for column data");
        free(df->column_names[df->num_columns]);
        return 0;
    }
    memcpy(df->columns[df->num_columns].data, data, data_size);

    df->num_columns++;
    df->num_rows = (df->num_rows < length) ? length : df->num_rows;
    return 1;
}



/**
 * @brief Prints the contents of a DataFrame to the console.
 *
 * This function displays the DataFrame in a formatted table, including column names,
 * data types, and values. If the DataFrame has more than 20 rows, it shows the first
 * 10 and last 10 rows with an ellipsis in between.
 *
 * @param df Pointer to the DataFrame to be printed. If NULL, the function prints
 *           an error message and returns.
 *
 * @return This function does not return a value.
 */
void print_dataframe(DataFrame *df) {
    if (df == NULL) {
        printf("DataFrame is NULL\n");
        return;
    }

    // Calculate column widths
    int col_widths[MAX_COLUMNS + 1] = {0};  // +1 for serial number column
    col_widths[0] = snprintf(NULL, 0, "%d", df->num_rows);  // Width for serial number
    col_widths[0] = col_widths[0] < 6 ? 6 : col_widths[0];  // Limit to 6 characters
    for (int i = 0; i < df->num_columns; i++) {
        col_widths[i + 1] = strlen(df->column_names[i]);
        for (int row = 0; row < df->num_rows; row++) {
            char buffer[64];
            int len = 0;
            switch (df->columns[i].type) {
                case TYPE_INT:
                    len = snprintf(buffer, sizeof(buffer), "%d", ((int*)df->columns[i].data)[row]);
                    break;
                case TYPE_FLOAT:
                    len = snprintf(buffer, sizeof(buffer), "%.2f", ((float*)df->columns[i].data)[row]);
                    break;
                case TYPE_STRING:
                    if (((char**)df->columns[i].data)[row] != NULL) {
                        len = strlen(((char**)df->columns[i].data)[row]);
                    } else {
                        len = 3; // Length of "N/A"
                    }
                    break;
            }
            if (len > col_widths[i + 1]) col_widths[i + 1] = len;
        }
        col_widths[i + 1] = col_widths[i + 1] < 6 ? 6 : col_widths[i + 1];  // Limit to 6 characters
        // Limit column width to 25 characters
        if (col_widths[i + 1] > 25) col_widths[i + 1] = 25;
    }

    // Determine how many rows to print
    int rows_to_print = df->num_rows;
    int print_ellipsis = 0;
    if (df->num_rows > 20) {
        rows_to_print = 20;  // 10 from start, 10 from end
        print_ellipsis = 1;
    }

    // Print top border
    for (int i = 0; i <= df->num_columns; i++) {
        printf("+%.*s", col_widths[i] + 2, "------------------------------------");
    }
    printf("+\n");

    // Print column names
    printf("| %-*s ", col_widths[0], "S.No");
    for (int i = 0; i < df->num_columns; i++) {
        printf("| %-*.*s ", col_widths[i + 1], col_widths[i + 1], df->column_names[i]);
    }
    printf("|\n");

    // Print separator
    for (int i = 0; i <= df->num_columns; i++) {
        printf("+%.*s", col_widths[i] + 2, "------------------------------------");
    }
    printf("+\n");

    // Print data
    for (int row = 0; row < rows_to_print; row++) {
        int actual_row = row < 10 ? row : df->num_rows - (rows_to_print - row);

        if (print_ellipsis && row == 10) {
            printf("| %-*s ", col_widths[0], "...");
            for (int col = 0; col < df->num_columns; col++) {
                printf("| %-*s ", col_widths[col + 1], "...");
            }
            printf("|\n");
            continue;
        }

        printf("| %-*d ", col_widths[0], actual_row);  // Print serial number
        for (int col = 0; col < df->num_columns; col++) {
            printf("| ");
            switch (df->columns[col].type) {
                case TYPE_INT:
                    printf("%-*d ", col_widths[col + 1], ((int*)df->columns[col].data)[actual_row]);
                    break;
                case TYPE_FLOAT:
                    printf("%-*.2f ", col_widths[col + 1], ((float*)df->columns[col].data)[actual_row]);
                    break;
                case TYPE_STRING: {
                    char *str = ((char**)df->columns[col].data)[actual_row];
                    if (str == NULL) {
                        printf("%-*s ", col_widths[col + 1], "N/A");
                    } else if (strlen(str) == 0) {
                        printf("%-*s ", col_widths[col + 1], "(empty)");
                    } else if (strlen(str) > col_widths[col + 1]) {
                        printf("%-.*s... ", col_widths[col + 1] - 3, str);
                    } else {
                        printf("%-*s ", col_widths[col + 1], str);
                    }
                    break;
                }
            }
        }
        printf("|\n");
    }


    // Print bottom border
    for (int i = 0; i <= df->num_columns; i++) {
        printf("+%.*s", col_widths[i] + 2, "------------------------------------");
    }
    printf("+\n");

    // Print total number of rows if truncated
    if (print_ellipsis) {
        printf("Displayed 20 out of %d rows\n", df->num_rows);
        printf("The total rows cannot be displayed here.\nTry print_dataframe_s(df, \"df_output.txt\") to view the full dataframe.\n");
    }
}



/**
 * @brief Creates a new DataFrame containing a range of rows from an existing DataFrame.
 *
 * This function extracts a subset of rows from the input DataFrame and creates a new
 * DataFrame with those rows. The range is inclusive of both start and end rows.
 *
 * @param df Pointer to the source DataFrame.
 * @param start_row The index of the first row to include in the range (inclusive).
 * @param end_row The index of the last row to include in the range (inclusive).
 *
 * @return A pointer to the newly created DataFrame containing the specified range of rows,
 *         or NULL if an error occurred (e.g., invalid input parameters, memory allocation failure).
 */
DataFrame* get_dataframe_range(DataFrame *df, int start_row, int end_row) {
    if (df == NULL || start_row < 0 || end_row >= df->num_rows || start_row > end_row) {
        set_error("Invalid input parameters for get_dataframe_range");
        return NULL;
    }

    DataFrame *result = create_dataframe();
    if (result == NULL) {
        set_error("Failed to create new DataFrame in get_dataframe_range");
        return NULL;
    }

    int num_rows = end_row - start_row + 1;

    for (int i = 0; i < df->num_columns; i++) {
        void *new_data = malloc(num_rows * get_column_element_size(df->columns[i].type));
        if (new_data == NULL) {
            set_error("Memory allocation failed in get_dataframe_range");
            free_dataframe(result);
            return NULL;
        }

        size_t elem_size = get_column_element_size(df->columns[i].type);
        memcpy(new_data, (char*)df->columns[i].data + start_row * elem_size, num_rows * elem_size);

        if (!add_column(result, df->column_names[i], df->columns[i].type, new_data, num_rows)) {
            set_error("Failed to add column in get_dataframe_range");
            free(new_data);
            free_dataframe(result);
            return NULL;
        }

        free(new_data);
    }

    return result;
}



/**
 * @brief Get the shape (dimensions) of a DataFrame.
 *
 * This function returns the number of rows and columns in the given DataFrame.
 * If the DataFrame is NULL, it returns [0, 0].
 *
 * @param df Pointer to the DataFrame whose shape is to be determined.
 *           If NULL, the function returns [0, 0].
 *
 * @return A pointer to a static array of two integers.
 *         The first element (index 0) represents the number of rows.
 *         The second element (index 1) represents the number of columns.
 *         Note: The returned array is statically allocated and should not be freed.
 */
int* shape_df(DataFrame *df) {
    static int shape[2];
    if (df == NULL) {
        shape[0] = 0;
        shape[1] = 0;
    } else {
        shape[0] = df->num_rows;
        shape[1] = df->num_columns;
    }
    return shape;
}





// Helper function to count null values
#define NULL_FLOAT NAN
// Helper function to get the size of a column type
/**
 * @brief Counts the number of null values in a column of data.
 *
 * This function iterates through the given data array and counts the number of null values
 * based on the specified column type. For integer columns, NULL_INT is considered null.
 * For float columns, NaN values are considered null. For string columns, NULL pointers
 * or empty strings are considered null.
 *
 * @param data Pointer to the array of data to be checked.
 * @param type The type of the column (TYPE_INT, TYPE_FLOAT, or TYPE_STRING).
 * @param length The number of elements in the data array.
 *
 * @return The count of null values found in the data array.
 */
int count_null_values(void *data, ColumnType type, int length) {
    int null_count = 0;
    for (int i = 0; i < length; i++) {
        switch (type) {
            case TYPE_INT:
                if (((int*)data)[i] == NULL_INT) null_count++;
                break;
            case TYPE_FLOAT:
                if (isnan(((float*)data)[i])) null_count++;
                break;
            case TYPE_STRING:
                if (((char**)data)[i] == NULL || strlen(((char**)data)[i]) == 0) null_count++;
                break;
        }
    }
    return null_count;
}


// Helper function to calculate range
/**
 * @brief Calculates the range of a set of float values.
 *
 * This function determines the difference between the maximum and minimum
 * values in the given array of float data.
 *
 * @param data Pointer to an array of float values.
 * @param length The number of elements in the data array.
 * @return The range of the data set (maximum value minus minimum value).
 */
float calculate_range(float *data, int length) {
    float min = data[0], max = data[0];
    for (int i = 1; i < length; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    return max - min;
}


// Helper function to count unique values and their frequencies
/**
 * @brief Counts unique values and their frequencies in an array of strings.
 *
 * This function analyzes an array of strings, identifies unique values,
 * counts their occurrences, and stores the results in provided output parameters.
 *
 * @param data An array of strings to be analyzed.
 * @param length The number of elements in the data array.
 * @param unique_count Pointer to store the number of unique values found.
 * @param unique_values Pointer to store an array of unique string values.
 * @param frequencies Pointer to store an array of frequencies corresponding to unique values.
 *
 * @return This function does not return a value. Results are stored in the provided output parameters.
 *
 * @note The function allocates memory for unique_values and frequencies.
 *       The caller is responsible for freeing this memory when it's no longer needed.
 */
void count_unique_values(char **data, int length, int *unique_count, char ***unique_values, int **frequencies) {
    *unique_count = 0;
    *unique_values = malloc(length * sizeof(char*));
    *frequencies = calloc(length, sizeof(int));


    for (int i = 0; i < length; i++) {

        // Handle null or empty values
        if (data[i] == NULL || strlen(data[i]) == 0) {
            continue;
        }

        int found = 0;
        for (int j = 0; j < *unique_count; j++) {
            if (strcmp(data[i], (*unique_values)[j]) == 0) {
                (*frequencies)[j]++;
                found = 1;
                break;
            }
        }
        if (!found) {
            (*unique_values)[*unique_count] = strdup(data[i]);
            if ((*unique_values)[*unique_count] == NULL) {
                continue;
            }
            (*frequencies)[*unique_count] = 1;
            (*unique_count)++;
        }
    }

}

/**
 * @brief Generates a descriptive summary of a DataFrame.
 *
 * This function creates a new DataFrame containing statistical information
 * about each column in the input DataFrame. The summary includes metrics
 * such as count, null count, mean, median, mode, min, max, quartiles,
 * range, variance, and standard deviation for numeric columns, and unique
 * value counts for string columns.
 *
 * @param df Pointer to the input DataFrame to be described.
 *           If NULL, the function returns NULL and sets an error.
 *
 * @return A pointer to a new DataFrame containing the descriptive summary.
 *         Returns NULL if an error occurs during the process, such as
 *         memory allocation failures or invalid input.
 *         The caller is responsible for freeing the returned DataFrame
 *         using free_dataframe() when it's no longer needed.
 */
DataFrame* describe_dataframe(DataFrame *df) {
    if (!df) {
        set_error("Invalid DataFrame");
        return NULL;
    }

    DataFrame *desc_df = create_dataframe();
    if (!desc_df) {
        set_error("Failed to create description DataFrame");
        return NULL;
    }

    const char *stat_names[] = {"count", "null_count", "mean", "median", "mode", "min", "25%", "50%", "75%", "max", "range", "variance", "std_dev"};
    int num_stats = sizeof(stat_names) / sizeof(stat_names[0]);
    char **stat_names_column = malloc(num_stats * sizeof(char*));
    if (!stat_names_column) {
        set_error("Failed to allocate memory for stat_names_column");
        free_dataframe(desc_df);
        return NULL;
    }
    for (int i = 0; i < num_stats; i++) {
        stat_names_column[i] = strdup(stat_names[i]);
        if (!stat_names_column[i]) {
            set_error("Failed to allocate memory for stat name");
            free_dataframe(desc_df);
            return NULL;
        }
    }
    if (!add_column(desc_df, "Statistic", TYPE_STRING, stat_names_column, num_stats)) {
        for (int i = 0; i < num_stats; i++) {
            free(stat_names_column[i]);
        }
        free(stat_names_column);
        free_dataframe(desc_df);
        return NULL;
    }

    for (int i = 0; i < df->num_columns; i++) {
        ColumnType type;
        void *data = get_column_as_array(df, i, &type);
        int length = df->num_rows;

        char **stats = malloc(num_stats * sizeof(char*));
        if (!stats) {
            set_error("Failed to allocate memory for stats");
            free_dataframe(desc_df);
            return NULL;
        }
        for (int j = 0; j < num_stats; j++) {
            stats[j] = malloc(MAX_STRING_LENGTH * sizeof(char));
            if (!stats[j]) {
                set_error("Failed to allocate memory for stat");
                for (int k = 0; k < j; k++) {
                    free(stats[k]);
                }
                free(stats);
                free_dataframe(desc_df);
                return NULL;
            }
        }

        int null_count = count_null_values(data, type, length);
        snprintf(stats[0], MAX_STRING_LENGTH, "%d", length);
        snprintf(stats[1], MAX_STRING_LENGTH, "%d", null_count);
        
        if (type == TYPE_INT || type == TYPE_FLOAT) {
            float *float_data = malloc(length * sizeof(float));
            if (!float_data) {
                set_error("Failed to allocate memory for float_data");
                for (int j = 0; j < num_stats; j++) {
                    free(stats[j]);
                }
                free(stats);
                free_dataframe(desc_df);
                return NULL;
            }
            for (int j = 0; j < length; j++) {
                if (type == TYPE_INT) {
                    float_data[j] = (((int*)data)[j] == NULL_INT) ? NAN : (float)((int*)data)[j];
                } else {
                    float_data[j] = ((float*)data)[j];
                }
            }

            float mean = calculate_mean(float_data, length);
            float median = calculate_median(float_data, length);
            float mode = calculate_mode(float_data, length);
            float variance = calculate_variance(float_data, length, mean);
            float std_dev = calculate_std_deviation(variance);
            float q25 = calculate_quartile(float_data, length, 0.25);
            float q50 = calculate_quartile(float_data, length, 0.5);
            float q75 = calculate_quartile(float_data, length, 0.75);
            float min = INFINITY, max = -INFINITY;
            for (int j = 0; j < length; j++) {
                if (!isnan(float_data[j])) {
                    if (float_data[j] < min) min = float_data[j];
                    if (float_data[j] > max) max = float_data[j];
                }
            }
            float range = (min != INFINITY && max != -INFINITY) ? max - min : NAN;

            snprintf(stats[2], MAX_STRING_LENGTH, "%.2f", mean);
            snprintf(stats[3], MAX_STRING_LENGTH, "%.2f", median);
            snprintf(stats[4], MAX_STRING_LENGTH, "%.2f", mode);
            snprintf(stats[5], MAX_STRING_LENGTH, "%.2f", min);
            snprintf(stats[6], MAX_STRING_LENGTH, "%.2f", q25);
            snprintf(stats[7], MAX_STRING_LENGTH, "%.2f", q50);
            snprintf(stats[8], MAX_STRING_LENGTH, "%.2f", q75);
            snprintf(stats[9], MAX_STRING_LENGTH, "%.2f", max);
            snprintf(stats[10], MAX_STRING_LENGTH, "%.2f", range);
            snprintf(stats[11], MAX_STRING_LENGTH, "%.2f", variance);
            snprintf(stats[12], MAX_STRING_LENGTH, "%.2f", std_dev);

            free(float_data);
        } else if (type == TYPE_STRING) {
            int unique_count;
            char **unique_values;
            int *frequencies;
            count_unique_values((char**)data, length, &unique_count, &unique_values, &frequencies);
            snprintf(stats[2], MAX_STRING_LENGTH, "N/A");
            snprintf(stats[3], MAX_STRING_LENGTH, "N/A");
            snprintf(stats[4], MAX_STRING_LENGTH, "%d unique values", unique_count);
            snprintf(stats[5], MAX_STRING_LENGTH, "N/A");
            snprintf(stats[6], MAX_STRING_LENGTH, "N/A");
            snprintf(stats[7], MAX_STRING_LENGTH, "N/A");
            snprintf(stats[8], MAX_STRING_LENGTH, "N/A");
            snprintf(stats[9], MAX_STRING_LENGTH, "N/A");
            snprintf(stats[10], MAX_STRING_LENGTH, "N/A");
            snprintf(stats[11], MAX_STRING_LENGTH, "N/A");
            snprintf(stats[12], MAX_STRING_LENGTH, "N/A");

            for (int j = 0; j < unique_count; j++) {
                free(unique_values[j]);
            }
            free(unique_values);
            free(frequencies);
        }

        if (!add_column(desc_df, df->column_names[i], TYPE_STRING, stats, num_stats)) {
            for (int j = 0; j < num_stats; j++) {
                free(stats[j]);
            }
            free(stats);
            free_dataframe(desc_df);
            return NULL;
        }
    }

    return desc_df;
}

 #include <math.h>

 // Helper function to calculate mean
 /**
  * @brief Calculates the mean of a set of float values, ignoring NaN values.
  *
  * This function computes the arithmetic mean of the non-NaN values in the given array.
  * If all values are NaN or the array is empty, it returns NaN.
  *
  * @param data Pointer to an array of float values.
  * @param length The number of elements in the data array.
  * @return The mean of the non-NaN values in the array, or NaN if no valid values are found.
  */
 float calculate_mean(float *data, int length) {
     float sum = 0;
     int count = 0;
     for (int i = 0; i < length; i++) {
         if (!isnan(data[i])) {
             sum += data[i];
             count++;
         }
     }
     return count > 0 ? sum / count : NAN;
 }


// Helper function to calculate median
/**
 * @brief Calculates the median value of a float array, ignoring NaN values.
 *
 * This function computes the median of the given array by first removing NaN values,
 * then sorting the remaining values and selecting the middle value(s).
 *
 * @param data Pointer to the float array containing the data.
 * @param length The number of elements in the data array.
 * @return The median value of the non-NaN elements in the array.
 *         Returns NAN if all elements are NaN or if memory allocation fails.
 */
float calculate_median(float *data, int length) {
    // Allocate memory for temporary array
    float *temp = (float *)malloc(length * sizeof(float));
    if (temp == NULL) {
        set_error("Memory allocation failed in calculate_median");
        return NAN;
    }


    int valid_count = 0;

    // Copy non-NaN values to the temporary array
    for (int i = 0; i < length; i++) {
        if (!isnan(data[i])) {
            temp[valid_count++] = data[i];
        }
    }

    float result = NAN;

    if (valid_count == 0) {
        // No valid data, return NAN
        result = NAN;
    } else {
        // Sort the temporary array
        for (int i = 0; i < valid_count - 1; i++) {
            for (int j = 0; j < valid_count - i - 1; j++) {
                if (temp[j] > temp[j + 1]) {
                    float t = temp[j];
                    temp[j] = temp[j + 1];
                    temp[j + 1] = t;
                }
            }
        }

        // Calculate median
        if (valid_count % 2 == 0) {
            result = (temp[valid_count / 2 - 1] + temp[valid_count / 2]) / 2.0f;
        } else {
            result = temp[valid_count / 2];
        }
    }

    // Free allocated memory
    free(temp);

    return result;
}

 // Helper function to calculate mode
 /**
  * @brief Calculates the mode of a set of float values, ignoring NaN values.
  *
  * This function determines the most frequently occurring value in the given array.
  * If multiple values have the same highest frequency, it returns the first one encountered.
  * NaN values are ignored in the calculation.
  *
  * @param data Pointer to an array of float values.
  * @param length The number of elements in the data array.
  * @return The mode of the non-NaN values in the array, or NaN if no mode is found or all values are NaN.
  */
 float calculate_mode(float *data, int length) {
     float mode = NAN;
     int max_count = 0;

     for (int i = 0; i < length; i++) {
         if (isnan(data[i])) continue;
         int count = 1;
         for (int j = i + 1; j < length; j++) {
             if (data[i] == data[j]) {
                 count++;
             }
         }
         if (count > max_count) {
             max_count = count;
             mode = data[i];
         }
     }

     return mode;
 }



/**
 * @brief Calculates the variance of a set of float values.
 *
 * This function computes the variance of the given data set using the provided mean.
 * The variance is a measure of variability in the data, calculated as the average
 * of squared differences from the mean.
 *
 * @param data Pointer to an array of float values.
 * @param length The number of elements in the data array.
 * @param mean The pre-calculated mean of the data set.
 * @return The variance of the data set.
 */
float calculate_variance(float *data, int length, float mean) {
    float sum_squared_diff = 0;
    for (int i = 0; i < length; i++) {
        float diff = data[i] - mean;
        sum_squared_diff += diff * diff;
    }
    return sum_squared_diff / length;
}


/**
 * @brief Calculates the standard deviation from the given variance.
 *
 * This function computes the standard deviation by taking the square root of the variance.
 * The standard deviation is a measure of the amount of variation or dispersion of a set of values.
 *
 * @param variance The variance of the dataset.
 * @return The standard deviation of the dataset.
 */
float calculate_std_deviation(float variance) {
    return sqrt(variance);
}


/**
 * @brief Calculates the quartile value for a given percentile in a sorted array of floats.
 *
 * This function computes the quartile value for a specified percentile in a pre-sorted
 * array of float values. It uses a simple linear interpolation method.
 *
 * @param data Pointer to a sorted array of float values.
 * @param length The number of elements in the data array.
 * @param percentile The desired percentile as a float between 0 and 1 (e.g., 0.25 for Q1, 0.5 for median, 0.75 for Q3).
 * @return The quartile value corresponding to the specified percentile.
 */
/**
 * @brief Calculates the quartile value for a given percentile in a sorted array of floats.
 *
 * This function computes the quartile value for a specified percentile in a pre-sorted
 * array of float values. It uses a simple linear interpolation method.
 *
 * @param data Pointer to a sorted array of float values.
 * @param length The number of elements in the data array.
 * @param percentile The desired percentile as a float between 0 and 1 (e.g., 0.25 for Q1, 0.5 for median, 0.75 for Q3).
 * @return The quartile value corresponding to the specified percentile.
 */
float calculate_quartile(float *data, int length, float percentile) {
    int index = (int)(percentile * (length - 1));
    return data[index];
}




 // Function to handle missing values
 /**
  * @brief Handles missing values in a specified column of a DataFrame.
  *
  * This function processes missing values in a given column of a DataFrame using
  * the specified strategy. It supports removal of rows with missing values or
  * replacement of missing values with mean, median, or mode of the column.
  *
  * @param df Pointer to the DataFrame to be processed.
  * @param column_index Index of the column to handle missing values in.
  * @param strategy String specifying the strategy to handle missing values.
  *                 Valid options are "remove", "mean", "median", or "mode".
  *
  * @return Pointer to the processed DataFrame if successful, NULL otherwise.
  *         The original DataFrame is modified in place.
  */
 /**
  * @brief Handles missing values in a specified column of a DataFrame.
  *
  * This function processes missing values in a given column of a DataFrame using
  * the specified strategy. It supports removal of rows with missing values or
  * replacement of missing values with mean, median, or mode of the column.
  *
  * @param df Pointer to the DataFrame to be processed.
  * @param column_index Index of the column to handle missing values in.
  * @param strategy String specifying the strategy to handle missing values.
  *                 Valid options are "remove", "mean", "median", or "mode".
  *
  * @return Pointer to the processed DataFrame if successful, NULL otherwise.
  *         The original DataFrame is modified in place.
  */
 DataFrame* handle_missing_values(DataFrame *df, int column_index, const char *strategy) {
     if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
         set_error("Invalid DataFrame or column index");
         return NULL;
     }

     Column *column = &df->columns[column_index];

     if (column->type != TYPE_FLOAT && column->type != TYPE_INT) {
         set_error("Handling missing values is only implemented for float and int columns");
         return NULL;
     }

     // Convert int column to float if necessary
     if (column->type == TYPE_INT) {
         float *new_data = malloc(df->num_rows * sizeof(float));
         if (!new_data) {
             set_error("Failed to allocate memory for float conversion");
             return NULL;
         }

         int *int_data = (int*)column->data;
         for (int i = 0; i < df->num_rows; i++) {
             if (int_data[i] == NULL_INT) {
                 new_data[i] = NAN;
             } else {
                 new_data[i] = (float)int_data[i];
             }
         }

         free(column->data);
         column->data = new_data;
         column->type = TYPE_FLOAT;
     }

     float *data = (float*)column->data;

     if (strcmp(strategy, "remove") == 0) {
         // Remove rows with null values
         int new_row_count = 0;
         for (int i = 0; i < df->num_rows; i++) {
             if (!isnan(data[i])) {
                 for (int j = 0; j < df->num_columns; j++) {
                     size_t element_size;
                     switch (df->columns[j].type) {
                         case TYPE_INT:
                             element_size = sizeof(int);
                             break;
                         case TYPE_FLOAT:
                             element_size = sizeof(float);
                             break;
                         case TYPE_STRING:
                             element_size = sizeof(char*);
                             break;
                         default:
                             set_error("Unknown column type");
                             return NULL;
                     }
                     memcpy((char*)df->columns[j].data + new_row_count * element_size,
                            (char*)df->columns[j].data + i * element_size,
                            element_size);
                 }
                 new_row_count++;
             }
         }
         df->num_rows = new_row_count;
     } else {
         float replacement_value;
         if (strcmp(strategy, "mean") == 0) {
             replacement_value = calculate_mean(data, df->num_rows);
         } else if (strcmp(strategy, "median") == 0) {
             replacement_value = calculate_median(data, df->num_rows);
         } else if (strcmp(strategy, "mode") == 0) {
             replacement_value = calculate_mode(data, df->num_rows);
         } else {
             set_error("Invalid strategy for handling missing values");
             return NULL;
         }

         if (isnan(replacement_value)) {
             set_error("Unable to calculate replacement value (all values might be NaN)");
             return NULL;
         }

         // Replace missing values
         for (int i = 0; i < df->num_rows; i++) {
             if (isnan(data[i])) {
                 data[i] = replacement_value;
             }
         }
     }

     return df;
 }


// Function to normalize a column
/**
 * @brief Normalizes a specified column in a DataFrame.
 *
 * This function creates a new DataFrame with all columns from the original,
 * then normalizes the specified column to a range of [0, 1]. The normalization
 * is performed using min-max scaling.
 *
 * @param df Pointer to the source DataFrame.
 * @param column_index Index of the column to be normalized.
 * @return Pointer to a new DataFrame with the normalized column, or NULL if an error occurs.
 *         The caller is responsible for freeing the returned DataFrame.
 */
DataFrame* normalize_column(DataFrame *df, int column_index) {
    if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
        set_error("Invalid DataFrame or column index");
        return NULL;
    }

    DataFrame *new_df = create_dataframe();
    if (new_df == NULL) {
        set_error("Failed to create new DataFrame for normalization");
        return NULL;
    }

    // Copy all columns
    for (int i = 0; i < df->num_columns; i++) {
        if (!add_column(new_df, df->column_names[i], df->columns[i].type, df->columns[i].data, df->num_rows)) {
            free_dataframe(new_df);
            return NULL;
        }
    }

    Column *column = &new_df->columns[column_index];
    if (column->type != TYPE_FLOAT && column->type != TYPE_INT) {
        set_error("Normalization is only applicable to numeric columns");
        free_dataframe(new_df);
        return NULL;
    }

    float *data = (float*)column->data;
    float min = data[0], max = data[0];

    for (int i = 1; i < new_df->num_rows; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }

    float range = max - min;
    if (range == 0) {
        set_error("Cannot normalize: all values are the same");
        free_dataframe(new_df);
        return NULL;
    }

    for (int i = 0; i < new_df->num_rows; i++) {
        data[i] = (data[i] - min) / range;
    }

    return new_df;
}


/**
 * @brief Scales a specified column in a DataFrame to a new range.
 *
 * This function creates a new DataFrame with all columns from the original,
 * then scales the specified column to a new range defined by new_min and new_max.
 * The scaling is performed using min-max normalization.
 *
 * @param df Pointer to the source DataFrame.
 * @param column_index Index of the column to be scaled.
 * @param new_min The minimum value of the new range.
 * @param new_max The maximum value of the new range.
 * @return Pointer to a new DataFrame with the scaled column, or NULL if an error occurs.
 *         The caller is responsible for freeing the returned DataFrame.
 */
DataFrame* scale_column(DataFrame *df, int column_index, float new_min, float new_max) {
    if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
        set_error("Invalid DataFrame or column index");
        return NULL;
    }

    DataFrame *new_df = create_dataframe();
    if (new_df == NULL) {
        set_error("Failed to create new DataFrame for scaling");
        return NULL;
    }

    // Copy all columns
    for (int i = 0; i < df->num_columns; i++) {
        if (!add_column(new_df, df->column_names[i], df->columns[i].type, df->columns[i].data, df->num_rows)) {
            free_dataframe(new_df);
            return NULL;
        }
    }

    Column *column = &new_df->columns[column_index];
    if (column->type != TYPE_FLOAT && column->type != TYPE_INT) {
        set_error("Scaling is only applicable to numeric columns");
        free_dataframe(new_df);
        return NULL;
    }

    float *data = (float*)column->data;
    float min = data[0], max = data[0];

    for (int i = 1; i < new_df->num_rows; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }

    float old_range = max - min;
    float new_range = new_max - new_min;

    if (old_range == 0) {
        set_error("Cannot scale: all values are the same");
        free_dataframe(new_df);
        return NULL;
    }

    for (int i = 0; i < new_df->num_rows; i++) {
        data[i] = ((data[i] - min) / old_range) * new_range + new_min;
    }

    return new_df;
}




/**
 * @brief Performs label encoding on a specified column in the DataFrame.
 *
 * This function applies label encoding to a categorical column, converting
 * unique string values to integer labels. The original DataFrame is not
 * modified; instead, a new DataFrame is returned with the encoded column.
 *
 * @param df Pointer to the source DataFrame.
 * @param column_name Name of the column to be label encoded.
 * @return Pointer to a new DataFrame with the label encoded column, or NULL if an error occurs.
 *         The caller is responsible for freeing the returned DataFrame.
 */
DataFrame* label_encode(DataFrame *df, const char *column_name) {
    // Implementation for label encoding
    // This is a placeholder and should be implemented based on specific requirements
    set_error("label_encode not implemented yet");
    return NULL;
}



/**
 * @brief Detects the data type of a given string.
 *
 * This function analyzes the content of a string to determine its data type.
 * It can identify integers, floating-point numbers, and strings.
 *
 * @param str The input string to analyze.
 * @return The detected ColumnType:
 *         - TYPE_INT for integer values
 *         - TYPE_FLOAT for floating-point values
 *         - TYPE_STRING for all other cases, including empty strings
 */
ColumnType detect_type(const char *str) {
    int has_decimal = 0;
    int i = 0;

    // Check for empty string
    if (str[0] == '\0') return TYPE_STRING;

    // Check for negative numbers
    if (str[0] == '-') i++;

    for (; str[i] != '\0'; i++) {
        if (isdigit(str[i])) continue;
        if (str[i] == '.' && !has_decimal) {
            has_decimal = 1;
            continue;
        }
        return TYPE_STRING;
    }

    return has_decimal ? TYPE_FLOAT : TYPE_INT;
}


// Function to add a column to the DataFrame
/**
 * @brief Adds a new column to the DataFrame from an array of strings.
 *
 * This function creates a new column in the DataFrame by converting an array of strings
 * to the appropriate data type (int, float, or string) based on the content of the first
 * non-null element. It handles null values and performs necessary memory allocations.
 *
 * @param df Pointer to the DataFrame to which the column will be added.
 * @param name The name of the new column.
 * @param data Array of strings containing the column data.
 * @param num_rows The number of rows in the data array.
 * @return 1 if the column was successfully added, 0 if an error occurred.
 */
int add_column_from_strings(DataFrame *df, const char *name, char **data, int num_rows) {
    if (df == NULL || data == NULL) {
        set_error("Invalid DataFrame or data");
        return 0;
    }

    ColumnType type = detect_type(data[0]);
    void *column_data = malloc(num_rows * (type == TYPE_STRING ? sizeof(char*) : (type == TYPE_FLOAT ? sizeof(float) : sizeof(int))));
    if (column_data == NULL) {
        set_error("Memory allocation failed for column data: %s", name);
        return 0;
    }


    for (int i = 0; i < num_rows; i++) {
        // Check for null values
        int is_null = (data[i][0] == '\0' || strcasecmp(data[i], "NULL") == 0);

        switch (type) {
            case TYPE_INT:
                if (is_null) {
                    ((int*)column_data)[i] = NULL_INT;  // Use 0 for null integers
                } else {
                    ((int*)column_data)[i] = atoi(data[i]);
                }
                break;
            case TYPE_FLOAT:
                if (is_null) {
                    ((float*)column_data)[i] = NAN;  // Use NAN for null floats
                } else {
                    ((float*)column_data)[i] = atof(data[i]);
                }
                break;
            case TYPE_STRING:
                if (is_null) {
                    ((char**)column_data)[i] = NULL;  // Use NULL for null strings
                } else {
                    ((char**)column_data)[i] = strdup(data[i]);
                    if (((char**)column_data)[i] == NULL) {
                        set_error("Memory allocation failed for string data");
                        free(column_data);
                        return 0;
                    }
                }
                break;
        }
    }

    if (!add_column(df, name, type, column_data, num_rows)) {
        free(column_data);
        return 0;
    }

    free(column_data);
    return 1;
}
static int count_columns(const char *line) {
    int count = 1;
    for (const char *p = line; *p; p++) {
        if (*p == ',') count++;
    }
    return count;
}

// Helper function to parse a CSV line
static char** parse_csv_line(char *line, int num_columns) {
    char **row = malloc(num_columns * sizeof(char*));
    if (!row) return NULL;

    char *token = strtok(line, ",");
    for (int i = 0; i < num_columns && token; i++) {
        row[i] = strdup(token);
        token = strtok(NULL, ",");
    }

    return row;
}

#ifdef _WIN32
DWORD WINAPI read_csv_chunk(LPVOID arg) {
#else
void* read_csv_chunk(void* arg) {
#endif
    ThreadArgs *args = (ThreadArgs*)arg;
    char buffer[BUFFER_SIZE];
    char line[MAX_STRING_LENGTH * MAX_COLUMNS];
    int line_pos = 0;
    int num_columns = 0;

    fseek(args->file, args->start_pos, SEEK_SET);

    while (ftell(args->file) < args->end_pos) {
        size_t bytes_read = fread(buffer, 1, sizeof(buffer), args->file);
        if (bytes_read == 0) break;

        for (size_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                line[line_pos] = '\0';
                if (num_columns == 0) {
                    num_columns = count_columns(line);
                }

                char **row = parse_csv_line(line, num_columns);
                if (row) {
                    mutex_lock(args->mutex);
                    if (*args->num_rows >= *args->capacity) {
                        *args->capacity *= 2;
                        *args->data = realloc(*args->data, *args->capacity * sizeof(char**));
                    }
                    (*args->data)[*args->num_rows] = row;
                    (*args->num_rows)++;
                    mutex_unlock(args->mutex);
                }
                line_pos = 0;
            } else if (line_pos < sizeof(line) - 1) {
                line[line_pos++] = buffer[i];
            }
        }
    }

    #ifdef _WIN32
    return 0;
    #else
    return NULL;
    #endif
}
DataFrame* read_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char ***data = malloc(sizeof(char**) * CHUNK_SIZE);
    int num_rows = 0;
    int capacity = CHUNK_SIZE;

    thread_handle threads[NUM_THREADS];
    ThreadArgs thread_args[NUM_THREADS];
    mutex_handle mutex;
    mutex_init(&mutex);

    long chunk_size = file_size / NUM_THREADS;
    long start_pos = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].file = file;
        thread_args[i].start_pos = start_pos;
        thread_args[i].end_pos = (i == NUM_THREADS - 1) ? file_size : start_pos + chunk_size;
        thread_args[i].data = &data;
        thread_args[i].num_rows = &num_rows;
        thread_args[i].capacity = &capacity;
        thread_args[i].mutex = &mutex;

        #ifdef _WIN32
        threads[i] = CreateThread(NULL, 0, read_csv_chunk, &thread_args[i], 0, NULL);
        #else
        pthread_create(&threads[i], NULL, read_csv_chunk, &thread_args[i]);
        #endif

        start_pos += chunk_size;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        #ifdef _WIN32
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
        #else
        pthread_join(threads[i], NULL);
        #endif
    }

    mutex_destroy(&mutex);
    fclose(file);

    if (num_rows == 0) {
        free(data);
        return NULL;
    }

    DataFrame *df = create_dataframe();
    if (!df) {
        for (int i = 0; i < num_rows; i++) {
            free(data[i]);
        }
        free(data);
        return NULL;
    }

    // Assume the first row contains column names
    df->num_columns = count_columns(data[0][0]);
    df->column_names = malloc(df->num_columns * sizeof(char*));
    for (int i = 0; i < df->num_columns; i++) {
        df->column_names[i] = strdup(data[0][i]);
    }

    // Allocate columns
    df->columns = malloc(df->num_columns * sizeof(Column));
    for (int i = 0; i < df->num_columns; i++) {
        df->columns[i].type = TYPE_STRING; // Default to string type
        df->columns[i].length = num_rows - 1; // Exclude header row
        df->columns[i].data = malloc(df->columns[i].length * sizeof(char*));
    }

    // Fill columns with data
    for (int i = 1; i < num_rows; i++) { // Start from 1 to skip header
        for (int j = 0; j < df->num_columns; j++) {
            ((char**)df->columns[j].data)[i-1] = strdup(data[i][j]);
        }
        free(data[i]);
    }
    free(data[0]); // Free header row
    free(data);

    df->num_rows = num_rows - 1; // Exclude header row
    df->max_rows = df->num_rows;

    return df;
}


/**
 * @brief Prints the contents of a DataFrame to a file in a formatted table.
 *
 * This function writes the DataFrame data to a specified file, formatting it as a table
 * with aligned columns. It includes a serial number column, handles different data types,
 * and represents NULL values appropriately.
 *
 * @param df Pointer to the DataFrame to be printed.
 * @param filename The name of the file where the formatted table will be written.
 * @return Returns 1 if the operation was successful, 0 if an error occurred.
 */
int print_dataframe_s(DataFrame *df, const char *filename) {
    if (df == NULL || filename == NULL) {
        set_error("Invalid DataFrame or filename");
        return 0;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        set_error("Unable to open file for writing");
        return 0;
    }

    // Calculate column widths
    int col_widths[MAX_COLUMNS + 1] = {0};  // +1 for serial number column
    col_widths[0] = snprintf(NULL, 0, "%d", df->num_rows);
    col_widths[0] = col_widths[0] < 6 ? 6 : col_widths[0];  // Minimum width of 6 for S.No
    for (int i = 0; i < df->num_columns; i++) {
        col_widths[i + 1] = strlen(df->column_names[i]);
        for (int row = 0; row < df->num_rows; row++) {
            char buffer[64];
            int len = 0;
            switch (df->columns[i].type) {
                case TYPE_INT:
                    if (((int*)df->columns[i].data)[row] == 0) {
                        len = 4; // Length of "NULL"
                    } else {
                        len = snprintf(buffer, sizeof(buffer), "%d", ((int*)df->columns[i].data)[row]);
                    }
                    break;
                case TYPE_FLOAT:
                    if (isnan(((float*)df->columns[i].data)[row])) {
                        len = 4; // Length of "NULL"
                    } else {
                        len = snprintf(buffer, sizeof(buffer), "%.2f", ((float*)df->columns[i].data)[row]);
                    }
                    break;
                case TYPE_STRING:
                    if (((char**)df->columns[i].data)[row] == NULL) {
                        len = 0; // Empty string for NULL
                    } else {
                        len = strlen(((char**)df->columns[i].data)[row]);
                    }
                    break;
            }
            if (len > col_widths[i + 1]) col_widths[i + 1] = len;
        }
    }

    // Print header
    fprintf(file, "| %-*s ", col_widths[0], "S.No");
    for (int i = 0; i < df->num_columns; i++) {
        fprintf(file, "| %-*s ", col_widths[i + 1], df->column_names[i]);
    }
    fprintf(file, "|\n");

    // Print separator
    for (int i = 0; i <= df->num_columns; i++) {
        fprintf(file, "+%.*s", col_widths[i] + 2, "------------------------------------");
    }
    fprintf(file, "+\n");

    // Print data
    for (int row = 0; row < df->num_rows; row++) {
        fprintf(file, "| %-*d ", col_widths[0], row + 1);
        for (int col = 0; col < df->num_columns; col++) {
            switch (df->columns[col].type) {
                case TYPE_INT:
                    if (((int*)df->columns[col].data)[row] == 0) {
                        fprintf(file, "| %-*s ", col_widths[col + 1], "NULL");
                    } else {
                        fprintf(file, "| %-*d ", col_widths[col + 1], ((int*)df->columns[col].data)[row]);
                    }
                    break;
                case TYPE_FLOAT:
                    if (isnan(((float*)df->columns[col].data)[row])) {
                        fprintf(file, "| %-*s ", col_widths[col + 1], "NULL");
                    } else {
                        fprintf(file, "| %-*.2f ", col_widths[col + 1], ((float*)df->columns[col].data)[row]);
                    }
                    break;
                case TYPE_STRING:
                    if (((char**)df->columns[col].data)[row] == NULL) {
                        fprintf(file, "| %-*s ", col_widths[col + 1], "");
                    } else {
                        fprintf(file, "| %-*s ", col_widths[col + 1], ((char**)df->columns[col].data)[row]);
                    }
                    break;
            }
        }
        fprintf(file, "|\n");
    }

    fclose(file);
    return 1;
}


/**
 * @brief Writes the contents of a DataFrame to a CSV file.
 *
 * This function takes a DataFrame and writes its contents to a CSV (Comma-Separated Values) file.
 * It writes the column names as the header and then writes each row of data.
 * The function handles different data types (int, float, string) appropriately.
 *
 * @param df Pointer to the DataFrame to be written to the file.
 * @param filename The name of the file to write the CSV data to.
 * @return Returns 1 if the operation was successful, 0 if an error occurred (e.g., file couldn't be opened).
 */
int write_csv(DataFrame *df, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        set_error("Failed to open file for writing");
        return 0;
    }

    // Write header
    for (int i = 0; i < df->num_columns; i++) {
        fprintf(file, "%s", df->column_names[i]);
        if (i < df->num_columns - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    // Write data
    for (int row = 0; row < df->num_rows; row++) {
        for (int col = 0; col < df->num_columns; col++) {
            Column *column = &df->columns[col];
            switch (column->type) {
                case TYPE_INT:
                    fprintf(file, "%d", ((int*)column->data)[row]);
                    break;
                case TYPE_FLOAT:
                    if (isnan(((float*)column->data)[row])) {
                        // Write blank for NaN values
                        fprintf(file, "");
                    } else {
                        fprintf(file, "%f", ((float*)column->data)[row]);
                    }
                    break;
                case TYPE_STRING:
                    fprintf(file, "%s", ((char**)column->data)[row]);
                    break;
            }
            if (col < df->num_columns - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
}

/**
 * @brief Retrieves a column from a DataFrame as a contiguous array.
 *
 * This function extracts a specified column from the given DataFrame and returns
 * it as a contiguous array of the appropriate data type. It also provides the
 * column's data type through the 'type' parameter.
 *
 * @param df Pointer to the DataFrame containing the column to be extracted.
 * @param column_index The index of the column to be extracted.
 * @param type Pointer to a ColumnType variable where the column's data type will be stored.
 *
 * @return A void pointer to the newly allocated array containing the column data.
 *         Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *         The caller is responsible for freeing the returned memory.
 */
void* get_column_as_array(DataFrame *df, int column_index, ColumnType *type) {
    if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
        set_error("Invalid DataFrame or column index");
        return NULL;
    }

    Column *column = &df->columns[column_index];
    *type = column->type;

    size_t element_size;
    switch (column->type) {
        case TYPE_INT:
            element_size = sizeof(int);
            break;
        case TYPE_FLOAT:
            element_size = sizeof(float);
            break;
        case TYPE_STRING:
            element_size = sizeof(char*);
            break;
        default:
            set_error("Unknown column type");
            return NULL;
    }

    void *array = malloc(element_size * df->num_rows);
    if (array == NULL) {
        set_error("Memory allocation failed");
        return NULL;
    }

    memcpy(array, column->data, element_size * df->num_rows);

    return array;
}


/**
 * @brief Get the size of a single element in a column based on its type.
 *
 * This function returns the size in bytes of a single element in a column,
 * given the column's data type.
 *
 * @param type The ColumnType enum value representing the data type of the column.
 * @return The size in bytes of a single element of the specified type.
 *         Returns 0 if the column type is unknown or invalid.
 */
size_t get_column_element_size(ColumnType type) {
    switch (type) {
        case TYPE_INT:
            return sizeof(int);
        case TYPE_FLOAT:
            return sizeof(float);
        case TYPE_STRING:
            return sizeof(char*);
        default:
            set_error("Unknown column type");
            return 0;
    }
}

/**
 * @brief Appends two DataFrames vertically.
 *
 * This function combines two DataFrames by appending the rows of the second DataFrame
 * to the first one. Both DataFrames must have the same number of columns.
 *
 * @param df1 Pointer to the first DataFrame to be appended.
 * @param df2 Pointer to the second DataFrame to be appended.
 * @return A pointer to a new DataFrame containing the combined data of df1 and df2.
 *         Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 */
DataFrame* append_dataframe(DataFrame *df1, DataFrame *df2) {
    if (df1 == NULL || df2 == NULL) {
        set_error("Invalid DataFrame(s)");
        return NULL;
    }

    if (df1->num_columns != df2->num_columns) {
        set_error("DataFrames have different number of columns");
        return NULL;
    }

    DataFrame *result = create_dataframe();
    if (result == NULL) {
        return NULL;
    }

    // Copy columns from df1
    for (int i = 0; i < df1->num_columns; i++) {
        void *new_data = malloc((df1->num_rows + df2->num_rows) * get_column_element_size(df1->columns[i].type));
        if (new_data == NULL) {
            set_error("Memory allocation failed");
            free_dataframe(result);
            return NULL;
        }

        memcpy(new_data, df1->columns[i].data, df1->num_rows * get_column_element_size(df1->columns[i].type));
        memcpy((char*)new_data + df1->num_rows * get_column_element_size(df1->columns[i].type),
               df2->columns[i].data, df2->num_rows * get_column_element_size(df2->columns[i].type));

        if (!add_column(result, df1->column_names[i], df1->columns[i].type, new_data, df1->num_rows + df2->num_rows)) {
            free(new_data);
            free_dataframe(result);
            return NULL;
        }

        free(new_data);
    }

    return result;
}



// ascending 1 for ascending order and 0 for descending order
/**
 * @brief Sorts a DataFrame based on a specified column.
 *
 * This function creates a new DataFrame that is a sorted version of the input DataFrame.
 * The sorting is performed based on the values in the specified column.
 *
 * @param df Pointer to the DataFrame to be sorted.
 * @param column_index The index of the column to sort by.
 * @param ascending Flag to determine the sort order: 1 for ascending, 0 for descending.
 *
 * @return A pointer to a new DataFrame containing the sorted data.
 *         Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *         The caller is responsible for freeing the returned DataFrame using free_dataframe().
 */
DataFrame* sort_dataframe(DataFrame *df, int column_index, int ascending) {
    if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
        set_error("Invalid DataFrame or column index");
        return NULL;
    }

    DataFrame *sorted_df = create_dataframe();
    if (sorted_df == NULL) {
        set_error("Failed to create new DataFrame for sorting");
        return NULL;
    }

    // Copy all columns to the new DataFrame
    for (int i = 0; i < df->num_columns; i++) {
        void *new_data = malloc(df->num_rows * get_column_element_size(df->columns[i].type));
        if (new_data == NULL) {
            set_error("Memory allocation failed in sort_dataframe");
            free_dataframe(sorted_df);
            return NULL;
        }
        memcpy(new_data, df->columns[i].data, df->num_rows * get_column_element_size(df->columns[i].type));
        if (!add_column(sorted_df, df->column_names[i], df->columns[i].type, new_data, df->num_rows)) {
            free(new_data);
            free_dataframe(sorted_df);
            return NULL;
        }
        free(new_data);
    }

    // Perform bubble sort
    for (int i = 0; i < sorted_df->num_rows - 1; i++) {
        for (int j = 0; j < sorted_df->num_rows - i - 1; j++) {
            int swap = 0;
            switch (sorted_df->columns[column_index].type) {
                case TYPE_INT: {
                    int a = ((int*)sorted_df->columns[column_index].data)[j];
                    int b = ((int*)sorted_df->columns[column_index].data)[j + 1];
                    swap = ascending ? (a > b) : (a < b);
                    break;
                }
                case TYPE_FLOAT: {
                    float a = ((float*)sorted_df->columns[column_index].data)[j];
                    float b = ((float*)sorted_df->columns[column_index].data)[j + 1];
                    swap = ascending ? (a > b) : (a < b);
                    break;
                }
                case TYPE_STRING: {
                    char *a = ((char**)sorted_df->columns[column_index].data)[j];
                    char *b = ((char**)sorted_df->columns[column_index].data)[j + 1];
                    int cmp = strcmp(a, b);
                    swap = ascending ? (cmp > 0) : (cmp < 0);
                    break;
                }
            }

            if (swap) {
                // Swap elements in all columns
                for (int k = 0; k < sorted_df->num_columns; k++) {
                    void *temp = malloc(get_column_element_size(sorted_df->columns[k].type));
                    if (temp == NULL) {
                        set_error("Memory allocation failed during sorting");
                        free_dataframe(sorted_df);
                        return NULL;
                    }
                    memcpy(temp, (char*)sorted_df->columns[k].data + j * get_column_element_size(sorted_df->columns[k].type),
                           get_column_element_size(sorted_df->columns[k].type));
                    memcpy((char*)sorted_df->columns[k].data + j * get_column_element_size(sorted_df->columns[k].type),
                           (char*)sorted_df->columns[k].data + (j + 1) * get_column_element_size(sorted_df->columns[k].type),
                           get_column_element_size(sorted_df->columns[k].type));
                    memcpy((char*)sorted_df->columns[k].data + (j + 1) * get_column_element_size(sorted_df->columns[k].type),
                           temp, get_column_element_size(sorted_df->columns[k].type));
                    free(temp);
                }
            }
        }
    }

    return sorted_df;
}



// Function to change a single value in a column
/**
 * @brief Changes a single value in a specified cell of the DataFrame.
 *
 * This function modifies the value at the given row and column indices in the DataFrame.
 * It handles different data types (int, float, string) appropriately.
 *
 * @param df Pointer to the DataFrame to be modified.
 * @param row_index The index of the row where the value should be changed.
 * @param column_index The index of the column where the value should be changed.
 * @param new_value Pointer to the new value to be set. The type should match the column's data type.
 *
 * @return Returns 1 if the operation was successful, 0 if an error occurred 
 *         (e.g., invalid input, memory allocation failure).
 */
int change_value(DataFrame *df, int row_index, int column_index, void *new_value) {
    if (df == NULL || row_index < 0 || row_index >= df->num_rows || 
        column_index < 0 || column_index >= df->num_columns) {
        set_error("Invalid DataFrame, row index, or column index");
        return 0;
    }

    Column *column = &df->columns[column_index];

    switch (column->type) {
        case TYPE_INT:
            if (new_value == NULL) {
                set_error("New value is NULL");
                return 0;
            }
            ((int*)column->data)[row_index] = *(int*)new_value;
            break;
        case TYPE_FLOAT:
            if (new_value == NULL) {
                set_error("New value is NULL");
                return 0;
            }
            ((float*)column->data)[row_index] = *(float*)new_value;
            break;
        case TYPE_STRING:
            if (new_value == NULL) {
                set_error("New value is NULL");
                return 0;
            }
            // Free the existing string and allocate memory for the new one
            free(((char**)column->data)[row_index]);
            ((char**)column->data)[row_index] = strdup((char*)new_value);
            if (((char**)column->data)[row_index] == NULL) {
                set_error("Memory allocation failed for new string value");
                return 0;
            }
            break;
        default:
            set_error("Unknown column type");
            return 0;
    }

    return 1;
}

/**
 * @brief Deletes a column from the DataFrame.
 *
 * This function removes a specified column from the DataFrame, freeing associated memory
 * and adjusting the DataFrame structure accordingly.
 *
 * @param df Pointer to the DataFrame from which the column will be deleted.
 * @param column_index The index of the column to be deleted.
 * @return Returns 1 if the column was successfully deleted, 0 if an error occurred.
 */
/**
 * @brief Deletes a column from the DataFrame.
 *
 * This function removes a specified column from the DataFrame, freeing associated memory
 * and adjusting the DataFrame structure accordingly.
 *
 * @param df Pointer to the DataFrame from which the column will be deleted.
 * @param column_index The index of the column to be deleted.
 * @return Returns 1 if the column was successfully deleted, 0 if an error occurred.
 */
int delete_column(DataFrame *df, int column_index) {
    if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
        set_error("Invalid DataFrame or column index");
        return 0;
    }

    // Free the memory for the column data
    Column *column = &df->columns[column_index];
    if (column->type == TYPE_STRING) {
        for (int i = 0; i < df->num_rows; i++) {
            free(((char**)column->data)[i]);
        }
    }
    free(column->data);

    // Free the column name
    free(df->column_names[column_index]);

    // Shift the remaining columns to fill the gap
    for (int i = column_index; i < df->num_columns - 1; i++) {
        df->columns[i] = df->columns[i + 1];
        df->column_names[i] = df->column_names[i + 1];
    }

    // Decrease the number of columns
    df->num_columns--;

    // Reallocate memory for columns and column names arrays
    df->columns = realloc(df->columns, df->num_columns * sizeof(Column));
    df->column_names = realloc(df->column_names, df->num_columns * sizeof(char*));

    if (df->columns == NULL || df->column_names == NULL) {
        set_error("Memory reallocation failed during column deletion");
        return 0;
    }

    return 1;
}



/**
 * @brief Replaces all occurrences of a specified value with a new value in a DataFrame column.
 *
 * This function iterates through a specified column in the DataFrame and replaces
 * all instances of the old value with the new value. It supports INT, FLOAT, and STRING
 * column types.
 *
 * @param df Pointer to the DataFrame containing the column to be modified.
 * @param column_index The index of the column in which to replace values.
 * @param old_value Pointer to the value to be replaced. The type should match the column's data type.
 * @param new_value Pointer to the new value to replace the old value. The type should match the column's data type.
 *
 * @return The number of values that were replaced. Returns 0 if an error occurred
 *         (e.g., invalid input, unsupported column type, or memory allocation failure).
 */
int replace_value(DataFrame *df, int column_index, void *old_value, void *new_value) {
    if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
        set_error("Invalid DataFrame or column index");
        return 0;
    }

    Column *column = &df->columns[column_index];
    int replaced_count = 0;


    switch (column->type) {
        case TYPE_INT: {
            int *data = (int *)column->data;
            int old_int = *(int *)old_value;
            int new_int = *(int *)new_value;
            for (int i = 0; i < df->num_rows; i++) {
                if (data[i] == old_int) {
                    data[i] = new_int;
                    replaced_count++;
                }
            }
            break;
        }
        case TYPE_FLOAT: {
            float *data = (float *)column->data;
            float old_float = *(float *)old_value;
            float new_float = *(float *)new_value;
            for (int i = 0; i < df->num_rows; i++) {
                if (fabs(data[i] - old_float) < 1e-6) {
                    data[i] = new_float;
                    replaced_count++;
                }
            }
            break;
        }
        case TYPE_STRING: {
            char **data = (char **)column->data;
            char *old_str = (char *)old_value;
            char *new_str = (char *)new_value;
            for (int i = 0; i < df->num_rows; i++) {
                if (data[i] == NULL) {
                    continue;
                }
                if (strcmp(data[i], old_str) == 0) {
                    free(data[i]);
                    data[i] = strdup(new_str);
                    if (data[i] == NULL) {
                        set_error("Memory allocation failed");
                        return replaced_count;
                    }
                    replaced_count++;
                }
            }
            break;
        }
        default:
            set_error("Unsupported column type");
            return 0;
    }

    return replaced_count;
}




/**
 * @brief Prints the unique values in a DataFrame column.
 *
 * This function prints the unique values in a specified column of the DataFrame.
 * It supports INT, FLOAT, and STRING column types.
 *
 * @param df Pointer to the DataFrame containing the column to be analyzed.
 * @param column_index The index of the column to analyze.
 * @return The number of unique values found in the column. Returns 0 if an error occurs.
 */
int print_unique_values(DataFrame *df, int column_index) {
    
    if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
        set_error("Invalid DataFrame or column index");
        return 0;
    }

    Column *column = &df->columns[column_index];
    int unique_count = 0;


    switch (column->type) {
        case TYPE_INT: {
            int *data = (int *)column->data;
            int *unique = malloc(df->num_rows * sizeof(int));
            if (!unique) {
                set_error("Memory allocation failed");
                return 0;
            }

            for (int i = 0; i < df->num_rows; i++) {
                int is_unique = 1;
                for (int j = 0; j < unique_count; j++) {
                    if (data[i] == unique[j]) {
                        is_unique = 0;
                        break;
                    }
                }
                if (is_unique) {
                    unique[unique_count++] = data[i];
                }
            }

            printf("Unique values in column '%s' (INT):\n", df->column_names[column_index]);
            for (int i = 0; i < unique_count; i++) {
                printf("%d ", unique[i]);
            }
            printf("\n");

            free(unique);
            break;
        }
        case TYPE_FLOAT: {
            float *data = (float *)column->data;
            float *unique = malloc(df->num_rows * sizeof(float));
            if (!unique) {
                set_error("Memory allocation failed");
                return 0;
            }

            for (int i = 0; i < df->num_rows; i++) {
                int is_unique = 1;
                for (int j = 0; j < unique_count; j++) {
                    if (fabs(data[i] - unique[j]) < 1e-6) {
                        is_unique = 0;
                        break;
                    }
                }
                if (is_unique) {
                    unique[unique_count++] = data[i];
                }
            }

            printf("Unique values in column '%s' (FLOAT):\n", df->column_names[column_index]);
            for (int i = 0; i < unique_count; i++) {
                printf("%.6f ", unique[i]);
            }
            printf("\n");

            free(unique);
            break;
        }
        case TYPE_STRING: {
            char **data = (char **)column->data;
            char **unique = malloc(df->num_rows * sizeof(char *));
            if (!unique) {
                set_error("Memory allocation failed");
                return 0;
            }

            for (int i = 0; i < df->num_rows; i++) {
                if (data[i] == NULL) {
                    continue;
                }
                int is_unique = 1;
                for (int j = 0; j < unique_count; j++) {
                    if (strcmp(data[i], unique[j]) == 0) {
                        is_unique = 0;
                        break;
                    }
                }
                if (is_unique) {
                    unique[unique_count++] = data[i];
                }
            }

            for (int i = 0; i < unique_count; i++) {
                printf("%s ", unique[i]);
            }
            printf("\n");

            free(unique);
            break;
        }
        default:
            set_error("Unsupported column type");
            return 0;
    }

    return unique_count;
}

/**
 * @brief Splits a DataFrame into features (X) and target (y) for machine learning tasks.
 *
 * This function separates the specified target column from the input DataFrame,
 * creating a new DataFrame with the remaining columns as features (X) and
 * extracting the target column as a separate array (y).
 *
 * @param df Pointer to the input DataFrame to be split.
 * @param target_column Name of the column to be used as the target variable.
 * @param y Pointer to a void pointer that will store the extracted target column data.
 *
 * @return A pointer to a new DataFrame containing all columns except the target column (X).
 *         Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *         The caller is responsible for freeing the returned DataFrame and the target array.
 */
DataFrame* split_dataframe(DataFrame* df, const char* target_column, void** y) {
    if (df == NULL || target_column == NULL) {
        return NULL;
    }

    int target_col_index = -1;
    for (int i = 0; i < df->num_columns; i++) {
        if (strcmp(df->column_names[i], target_column) == 0) {
            target_col_index = i;
            break;
        }
    }

    if (target_col_index == -1) {
        printf("Target column '%s' not found in the DataFrame.\n", target_column);
        return NULL;
    }

    DataFrame* X = create_dataframe();
    if (X == NULL) {
        return NULL;
    }

    // Copy all columns except the target column to X
    for (int i = 0; i < df->num_columns; i++) {
        if (i != target_col_index) {
            void* column_data = get_column_as_array(df, i, &df->columns[i].type);
            if (column_data == NULL || !add_column(X, df->column_names[i], df->columns[i].type, column_data, df->num_rows)) {
                free_dataframe(X);
                free(column_data);
                return NULL;
            }
            free(column_data);
        }
    }

    // Extract the target column as y
    *y = get_column_as_array(df, target_col_index, &df->columns[target_col_index].type);
    if (*y == NULL) {
        free_dataframe(X);
        return NULL;
    }

    return X;
}


#include <math.h>
/**
 * @brief Performs matrix multiplication of two matrices A and B, storing the result in matrix C.
 *
 * This function multiplies matrix A (m x n) with matrix B (n x p) and stores the result in matrix C (m x p).
 * The matrices are represented as 1D arrays in row-major order.
 *
 * @param A Pointer to the first input matrix A (m x n)
 * @param B Pointer to the second input matrix B (n x p)
 * @param C Pointer to the output matrix C (m x p) where the result will be stored
 * @param m Number of rows in matrix A and C
 * @param n Number of columns in matrix A and number of rows in matrix B
 * @param p Number of columns in matrix B and C
 */
void matrix_multiply(float *A, float *B, float *C, int m, int n, int p) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            C[i*p + j] = 0;
            for (int k = 0; k < n; k++) {
                C[i*p + j] += A[i*n + k] * B[k*p + j];
            }
        }
    }
}


// Helper function for matrix transpose
/**
 * @brief Computes the transpose of a matrix.
 *
 * This function calculates the transpose of matrix A and stores the result in matrix AT.
 * The input matrix A is assumed to be in row-major order, and the output matrix AT
 * will be in column-major order.
 *
 * @param A Pointer to the input matrix A (m x n) in row-major order
 * @param AT Pointer to the output matrix AT (n x m) where the transpose will be stored
 * @param m Number of rows in the input matrix A
 * @param n Number of columns in the input matrix A
 */
void matrix_transpose(float *A, float *AT, int m, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            AT[j*m + i] = A[i*n + j];
        }
    }
}


// Helper function for matrix inversion (using Gauss-Jordan elimination)
/**
 * @brief Computes the inverse of a square matrix using Gauss-Jordan elimination.
 *
 * This function calculates the inverse of the input matrix A using the Gauss-Jordan
 * elimination method. The inverse is computed in-place, overwriting the original matrix.
 *
 * @param A Pointer to the input square matrix to be inverted. On successful completion,
 *          this will contain the inverted matrix.
 * @param n The dimension of the square matrix (number of rows or columns).
 *
 * @return Returns 1 if the matrix inversion was successful, 0 if the matrix is singular
 *         or if memory allocation fails.
 */
int matrix_inverse(float *A, int n) {
    float *temp = malloc(n * n * sizeof(float));
    if (temp == NULL) {
        return 0;
    }
    memcpy(temp, A, n * n * sizeof(float));

    float *identity = calloc(n * n, sizeof(float));
    if (identity == NULL) {
        free(temp);
        return 0;
    }
    for (int i = 0; i < n; i++) {
        identity[i*n + i] = 1.0f;
    }

    for (int i = 0; i < n; i++) {
        float pivot = temp[i*n + i];
        if (fabs(pivot) < 1e-6) {
            free(temp);
            free(identity);
            return 0;  // Matrix is singular
        }

        for (int j = 0; j < n; j++) {
            temp[i*n + j] /= pivot;
            identity[i*n + j] /= pivot;
        }

        for (int k = 0; k < n; k++) {
            if (k != i) {
                float factor = temp[k*n + i];
                for (int j = 0; j < n; j++) {
                    temp[k*n + j] -= factor * temp[i*n + j];
                    identity[k*n + j] -= factor * identity[i*n + j];
                }
            }
        }
    }

    memcpy(A, identity, n * n * sizeof(float));
    free(temp);
    free(identity);
    return 1;
}

/**
 * @brief Creates a new linear regression model.
 *
 * This function allocates memory for a LinearRegressionModel structure and initializes
 * its members. It sets up the model with the specified number of features, allocating
 * memory for the coefficients and initializing the intercept.
 *
 * @param num_features The number of features (independent variables) in the model.
 *
 * @return A pointer to the newly created LinearRegressionModel structure.
 *         Returns NULL if memory allocation fails.
 */
LinearRegressionModel* create_linear_regression_model(int num_features) {
    LinearRegressionModel *model = malloc(sizeof(LinearRegressionModel));
    if (model == NULL) {
        return NULL;
    }
    model->coefficients = calloc(num_features, sizeof(float));
    if (model->coefficients == NULL) {
        free(model);
        return NULL;
    }
    model->intercept = 0.0f;
    model->num_features = num_features;
    return model;
}


/**
 * @brief Frees the memory allocated for a LinearRegressionModel.
 *
 * This function deallocates the memory used by a LinearRegressionModel,
 * including its coefficients and the model structure itself.
 *
 * @param model Pointer to the LinearRegressionModel to be freed.
 *              If NULL, the function does nothing.
 *
 * @return This function does not return a value.
 */
void free_linear_regression_model(LinearRegressionModel *model) {
    if (model) {
        free(model->coefficients);
        free(model);
    }
}


/**
 * @brief Fits a linear regression model using the given features and target values.
 *
 * This function performs linear regression using the ordinary least squares method.
 * It computes the coefficients and intercept for the linear model based on the input
 * features (X) and target values (y).
 *
 * @param model Pointer to the LinearRegressionModel structure to be fitted.
 * @param X Pointer to the DataFrame containing the feature data.
 * @param y Pointer to an array of float values representing the target variable.
 *
 * @return Returns 1 if the fitting process was successful, 0 if an error occurred
 *         (e.g., invalid input, memory allocation failure, or singular matrix).
 */
int fit_linear_regression(LinearRegressionModel *model, DataFrame *X, float *y) {
    if (model == NULL || X == NULL || y == NULL || X->num_columns != model->num_features) {
        return 0;
    }

    int n = X->num_rows;
    int m = X->num_columns;

    // Allocate memory for X matrix (including bias term)
    float *X_mat = malloc((n * (m + 1)) * sizeof(float));
    if (X_mat == NULL) {
        return 0;
    }

    // Fill X matrix
    for (int i = 0; i < n; i++) {
        X_mat[i * (m + 1)] = 1.0f;  // Bias term
        for (int j = 0; j < m; j++) {
            X_mat[i * (m + 1) + j + 1] = ((float*)X->columns[j].data)[i];
        }
    }

    // Allocate memory for intermediate matrices
    float *XT = malloc((n * (m + 1)) * sizeof(float));
    float *XTX = malloc(((m + 1) * (m + 1)) * sizeof(float));
    float *XTy = malloc((m + 1) * sizeof(float));

    if (XT == NULL || XTX == NULL || XTy == NULL) {
        free(X_mat);
        free(XT);
        free(XTX);
        free(XTy);
        return 0;
    }

    // Compute X^T
    matrix_transpose(X_mat, XT, n, m + 1);

    // Compute X^T * X
    matrix_multiply(XT, X_mat, XTX, m + 1, n, m + 1);

    // Compute (X^T * X)^-1
    if (!matrix_inverse(XTX, m + 1)) {
        free(X_mat);
        free(XT);
        free(XTX);
        free(XTy);
        return 0;
    }

    // Compute X^T * y
    for (int i = 0; i < m + 1; i++) {
        XTy[i] = 0;
        for (int j = 0; j < n; j++) {
            XTy[i] += XT[i*n + j] * y[j];
        }
    }

    // Compute (X^T * X)^-1 * (X^T * y)
    float *coefficients = malloc((m + 1) * sizeof(float));
    if (coefficients == NULL) {
        free(X_mat);
        free(XT);
        free(XTX);
        free(XTy);
        return 0;
    }

    matrix_multiply(XTX, XTy, coefficients, m + 1, m + 1, 1);

    // Store the results in the model
    model->intercept = coefficients[0];
    for (int i = 0; i < m; i++) {
        model->coefficients[i] = coefficients[i + 1];
    }

    // Free allocated memory
    free(X_mat);
    free(XT);
    free(XTX);
    free(XTy);
    free(coefficients);

    return 1;
}


/**
 * @brief Predicts target values using a trained linear regression model.
 *
 * This function applies the linear regression model to the input features
 * to generate predictions. It supports both float and integer feature types.
 *
 * @param model Pointer to the trained LinearRegressionModel.
 * @param X Pointer to the DataFrame containing the input features.
 *
 * @return A pointer to a dynamically allocated array of float values
 *         containing the predictions. The caller is responsible for freeing
 *         this memory. Returns NULL if an error occurs (e.g., invalid input
 *         or memory allocation failure).
 */
float* predict_linear_regression(LinearRegressionModel *model, DataFrame *X) {
    if (model == NULL || X == NULL || X->num_columns != model->num_features) {
        return NULL;
    }

    int n = X->num_rows;
    float *predictions = malloc(n * sizeof(float));
    if (predictions == NULL) {
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        predictions[i] = model->intercept;
        for (int j = 0; j < model->num_features; j++) {
            if (X->columns[j].type == TYPE_FLOAT) {
                predictions[i] += model->coefficients[j] * ((float*)X->columns[j].data)[i];
            } else if (X->columns[j].type == TYPE_INT) {
                predictions[i] += model->coefficients[j] * (float)((int*)X->columns[j].data)[i];
            }
        }
    }

    return predictions;
}


/**
 * @brief Calculates the R-squared (coefficient of determination) value for a set of predictions.
 *
 * This function computes the R-squared value, which measures the proportion of the variance 
 * in the dependent variable that is predictable from the independent variable(s).
 *
 * @param y_true Pointer to an array of float values representing the true (observed) values.
 * @param y_pred Pointer to an array of float values representing the predicted values.
 * @param n The number of elements in both y_true and y_pred arrays.
 *
 * @return The R-squared value as a float, ranging from 0 to 1. 
 *         A value closer to 1 indicates a better fit of the model.
 */
float calculate_r_squared(float *y_true, float *y_pred, int n) {
    float mean_y = 0.0f;
    for (int i = 0; i < n; i++) {
        mean_y += y_true[i];
    }
    mean_y /= n;

    float ss_tot = 0.0f, ss_res = 0.0f;
    for (int i = 0; i < n; i++) {
        ss_tot += (y_true[i] - mean_y) * (y_true[i] - mean_y);
        ss_res += (y_true[i] - y_pred[i]) * (y_true[i] - y_pred[i]);
    }

    return 1 - (ss_res / ss_tot);
}


/**
 * @brief Calculates the Mean Squared Error (MSE) between true and predicted values.
 *
 * This function computes the Mean Squared Error, which is a measure of the average
 * squared difference between the predicted values and the true values. It is commonly
 * used to evaluate the performance of regression models.
 *
 * @param y_true Pointer to an array of float values representing the true (observed) values.
 * @param y_pred Pointer to an array of float values representing the predicted values.
 * @param n The number of elements in both y_true and y_pred arrays.
 *
 * @return The Mean Squared Error as a float value. A lower MSE indicates better model performance.
 */
float calculate_mse(float *y_true, float *y_pred, int n) {
    float mse = 0.0f;
    for (int i = 0; i < n; i++) {
        float error = y_true[i] - y_pred[i];
        mse += error * error;
    }
    return mse / n;
}















const char* get_dataclenz_version() {
    return "2.0.0";
}