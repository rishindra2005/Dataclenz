#include "clenzdat.h"
#include <stdarg.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h> // Add this for bool type support


#define NULL_INT INT_MIN
#define NULL_FLOAT NAN

char error_message[256] = {0};


void debug_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "[DEBUG] ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    fflush(stderr);
}
void set_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(error_message, sizeof(error_message) - 1, format, args);
    va_end(args);
}

const char* get_error() {
    return error_message;
}
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
size_t get_type_size(ColumnType type) {
    switch (type) {
        case TYPE_INT:
            return sizeof(int);
        case TYPE_FLOAT:
            return sizeof(float);
        case TYPE_STRING:
            return sizeof(char*);
        default:
            set_error("Invalid column type");
            return 0;
    }
}

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
float calculate_range(float *data, int length) {
    float min = data[0], max = data[0];
    for (int i = 1; i < length; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    return max - min;
}

// Helper function to count unique values and their frequencies
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
            for (int j = 0; j < i; j++) {
                free(stat_names_column[j]);
            }
            free(stat_names_column);
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
// Helper function to calculate median
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


float calculate_variance(float *data, int length, float mean) {
    float sum_squared_diff = 0;
    for (int i = 0; i < length; i++) {
        float diff = data[i] - mean;
        sum_squared_diff += diff * diff;
    }
    return sum_squared_diff / length;
}

float calculate_std_deviation(float variance) {
    return sqrt(variance);
}

float calculate_quartile(float *data, int length, float percentile) {
    int index = (int)(percentile * (length - 1));
    return data[index];
}


 // Function to handle missing values
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



DataFrame* label_encode(DataFrame *df, const char *column_name) {
    // Implementation for label encoding
    // This is a placeholder and should be implemented based on specific requirements
    set_error("label_encode not implemented yet");
    return NULL;
}


// Helper function to determine the type of a string
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

DataFrame* read_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        set_error("Unable to open file: %s", filename);
        return NULL;
    }

    DataFrame *df = create_dataframe();
    if (df == NULL) {
        fclose(file);
        return NULL;
    }

    char line[MAX_STRING_LENGTH * MAX_COLUMNS];
    char *token;
    char *headers[MAX_COLUMNS];
    int num_columns = 0;
    char **data = NULL;
    int num_rows = 0;
    int capacity = 1000;  // Initial capacity

    // Read headers
    if (fgets(line, sizeof(line), file) == NULL) {
        set_error("File is empty: %s", filename);
        free_dataframe(df);
        fclose(file);
        return NULL;
    }

    token = strtok(line, ",\n");
    while (token != NULL && num_columns < MAX_COLUMNS) {
        headers[num_columns] = strdup(token);
        if (headers[num_columns] == NULL) {
            set_error("Memory allocation failed for header: %s", token);
            free_dataframe(df);
            fclose(file);
            return NULL;
        }
        num_columns++;
        token = strtok(NULL, ",\n");
    }

    // Allocate initial memory for data
    data = (char **)malloc(capacity * sizeof(char *));
    if (data == NULL) {
        set_error("Memory allocation failed for data");
        free_dataframe(df);
        fclose(file);
        return NULL;
    }

    // Read data
    while (fgets(line, sizeof(line), file) != NULL) {
        if (num_rows >= capacity) {
            capacity *= 2;
            data = (char **)realloc(data, capacity * sizeof(char *));
            if (data == NULL) {
                set_error("Memory reallocation failed for data");
                free_dataframe(df);
                fclose(file);
                return NULL;
            }
        }
        data[num_rows] = strdup(line);
    if (data[num_rows] == NULL) {
        set_error("Memory allocation failed for row: %d", num_rows);
        free_dataframe(df);
        fclose(file);
        return NULL;
    }
    num_rows++;
    }

    fclose(file);

    if (num_rows == 0) {
        set_error("No data rows found in file: %s", filename);
        for (int i = 0; i < num_columns; i++) {
            free(headers[i]);
        }
        free(data);
        free_dataframe(df);
        return NULL;
    }

        // Resize the DataFrame to accommodate all rows
    if (!resize_dataframe(df, num_rows)) {
        set_error("Failed to resize DataFrame");
        for (int i = 0; i < num_rows; i++) {
            free(data[i]);
        }
        free(data);
        free_dataframe(df);
        return NULL;
    }

    // Add columns using the new function
    for (int i = 0; i < num_columns; i++) {
    char **column_data = malloc(num_rows * sizeof(char*));
    if (column_data == NULL) {
        set_error("Memory allocation failed for column data");
        // ... (error handling code remains the same)
    }
    
    for (int j = 0; j < num_rows; j++) {
        char *line = data[j];
        for (int k = 0; k < i; k++) {
            line = strchr(line, ',');
            if (line == NULL) {
                column_data[j] = strdup("");
                break;
            }
            line++; // Move past the comma
        }
        
        if (line != NULL) {
            char *end = strchr(line, ',');
            if (end == NULL) {
                end = line + strlen(line);
                // Remove newline character if it's the last column
                if (end > line && (*(end-1) == '\n' || *(end-1) == '\r')) {
                    end--;
                }
                if (end > line && (*(end-1) == '\n' || *(end-1) == '\r')) {
                    end--;
                }
            }
            int len = end - line;
            column_data[j] = malloc(len + 1);
            if (column_data[j] == NULL) {
                column_data[j] = strdup("");
            } else {
                strncpy(column_data[j], line, len);
                column_data[j][len] = '\0';
            }
        } else {
            column_data[j] = strdup("");
        }
    }

        if (!add_column_from_strings(df, headers[i], column_data, num_rows)) {
            set_error("Failed to add column: %s", headers[i]);
            for (int j = 0; j < num_rows; j++) {
                free(column_data[j]);
            }
            free(column_data);
            for (int j = 0; j < num_rows; j++) {
                free(data[j]);
            }
            free(data);
            for (int j = 0; j < num_columns; j++) {
                free(headers[j]);
            }
            free_dataframe(df);
            return NULL;
        }
        for (int j = 0; j < num_rows; j++) {
            free(column_data[j]);
        }
        free(column_data);
    }

    // Clean up
    for (int i = 0; i < num_columns; i++) {
        free(headers[i]);
    }
    for (int i = 0; i < num_rows; i++) {
        free(data[i]);
    }
    free(data);

    return df;
}
DataFrame* read_csv_d(const char *filename, char delimiter) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        set_error("Unable to open file: %s", filename);
        return NULL;
    }

    DataFrame *df = create_dataframe();
    if (df == NULL) {
        fclose(file);
        return NULL;
    }

    char line[MAX_STRING_LENGTH * MAX_COLUMNS];
    char *token;
    char *headers[MAX_COLUMNS];
    int num_columns = 0;
    char **data = NULL;
    int num_rows = 0;
    int capacity = 1000;  // Initial capacity
    char delim_str[2] = {delimiter, '\0'};  // Create a string from the delimiter

    // Read headers
    if (fgets(line, sizeof(line), file) == NULL) {
        set_error("File is empty: %s", filename);
        free_dataframe(df);
        fclose(file);
        return NULL;
    }

    // Remove newline character if present
    char *newline = strchr(line, '\n');
    if (newline) *newline = '\0';

    token = strtok(line, delim_str);
    while (token != NULL && num_columns < MAX_COLUMNS) {
        headers[num_columns] = strdup(token);
        if (headers[num_columns] == NULL) {
            set_error("Memory allocation failed for header: %s", token);
            free_dataframe(df);
            fclose(file);
            return NULL;
        }
        num_columns++;
        token = strtok(NULL, delim_str);
    }

    // Allocate initial memory for data
    data = (char **)malloc(capacity * sizeof(char *));
    if (data == NULL) {
        set_error("Memory allocation failed for data");
        free_dataframe(df);
        fclose(file);
        return NULL;
    }

    // Read data
    while (fgets(line, sizeof(line), file) != NULL) {
        if (num_rows >= capacity) {
            capacity *= 2;
            data = (char **)realloc(data, capacity * sizeof(char *));
            if (data == NULL) {
                set_error("Memory reallocation failed for data");
                free_dataframe(df);
                fclose(file);
                return NULL;
            }
        }
        // Remove newline character if present
        newline = strchr(line, '\n');
        if (newline) *newline = '\0';

        data[num_rows] = strdup(line);
        if (data[num_rows] == NULL) {
            set_error("Memory allocation failed for row: %d", num_rows);
            free_dataframe(df);
            fclose(file);
            return NULL;
        }
        num_rows++;
    }

    fclose(file);

    if (num_rows == 0) {
        set_error("No data rows found in file: %s", filename);
        for (int i = 0; i < num_columns; i++) {
            free(headers[i]);
        }
        free(data);
        free_dataframe(df);
        return NULL;
    }

    // Resize the DataFrame to accommodate all rows
    if (!resize_dataframe(df, num_rows)) {
        set_error("Failed to resize DataFrame");
        for (int i = 0; i < num_rows; i++) {
            free(data[i]);
        }
        free(data);
        free_dataframe(df);
        return NULL;
    }

    // Add columns using the new function
    for (int i = 0; i < num_columns; i++) {
        char **column_data = malloc(num_rows * sizeof(char*));
        if (column_data == NULL) {
            set_error("Memory allocation failed for column data");
            // ... (error handling code remains the same)
        }
        
        for (int j = 0; j < num_rows; j++) {
            char *line = data[j];
            for (int k = 0; k < i; k++) {
                line = strchr(line, delimiter);
                if (line == NULL) {
                    column_data[j] = strdup("");
                    break;
                }
                line++; // Move past the delimiter
            }
            
            if (line != NULL) {
                char *end = strchr(line, delimiter);
                if (end == NULL) {
                    end = line + strlen(line);
                }
                int len = end - line;
                column_data[j] = malloc(len + 1);
                if (column_data[j] == NULL) {
                    column_data[j] = strdup("");
                } else {
                    strncpy(column_data[j], line, len);
                    column_data[j][len] = '\0';
                }
            } else {
                column_data[j] = strdup("");
            }
        }

        if (!add_column_from_strings(df, headers[i], column_data, num_rows)) {
            set_error("Failed to add column: %s", headers[i]);
            for (int j = 0; j < num_rows; j++) {
                free(column_data[j]);
            }
            free(column_data);
            for (int j = 0; j < num_rows; j++) {
                free(data[j]);
            }
            free(data);
            for (int j = 0; j < num_columns; j++) {
                free(headers[j]);
            }
            free_dataframe(df);
            return NULL;
        }
        for (int j = 0; j < num_rows; j++) {
            free(column_data[j]);
        }
        free(column_data);
    }

    // Clean up
    for (int i = 0; i < num_columns; i++) {
        free(headers[i]);
    }
    for (int i = 0; i < num_rows; i++) {
        free(data[i]);
    }
    free(data);

    return df;
}
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

int find_column_index(DataFrame *df, const char *column_name) {
    if (df == NULL || column_name == NULL) {
        set_error("Invalid DataFrame or column name");
        return -1;
    }

    for (int i = 0; i < df->num_columns; i++) {
        if (strcmp(df->column_names[i], column_name) == 0) {
            return i;
        }
    }

    set_error("Column not found");
    return -1;
}
// ascending 1 for ascending order and 0 for descending order
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
int delete_column(DataFrame *df, int column_index) {
    // debug_log("Entering delete_column function");
    // debug_log("DataFrame pointer: %p, column_index: %d", (void*)df, column_index);

    if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
        set_error("Invalid DataFrame or column index");
        // debug_log("Error: Invalid DataFrame or column index");
        return 0;
    }

    // debug_log("Current number of columns: %d", df->num_columns);

    // Free the memory for the column data
    Column *column = &df->columns[column_index];
    // debug_log("Deleting column of type: %d", column->type);

    if (column->type == TYPE_STRING) {
        // debug_log("Freeing string data for column");
        for (int i = 0; i < df->num_rows; i++) {
            free(((char**)column->data)[i]);
        }
    }
    free(column->data);
    // debug_log("Column data freed");

    // Free the column name
    // debug_log("Freeing column name: %s", df->column_names[column_index]);
    free(df->column_names[column_index]);

    // Shift the remaining columns to fill the gap
    // debug_log("Shifting remaining columns");
    for (int i = column_index; i < df->num_columns - 1; i++) {
        df->columns[i] = df->columns[i + 1];
        df->column_names[i] = df->column_names[i + 1];
    }

    // Decrease the number of columns
    df->num_columns--;
    // debug_log("Number of columns decreased to: %d", df->num_columns);

    // Only reallocate if there are remaining columns
    if (df->num_columns > 0) {
        // debug_log("Reallocating memory for columns and column names");
        Column *new_columns = realloc(df->columns, df->num_columns * sizeof(Column));
        char **new_column_names = realloc(df->column_names, df->num_columns * sizeof(char*));

        if (new_columns == NULL || new_column_names == NULL) {
            set_error("Memory reallocation failed during column deletion");
            // debug_log("Error: Memory reallocation failed");
            return 0;
        }

        df->columns = new_columns;
        df->column_names = new_column_names;
    } else {
        // If no columns remain, free the arrays and set pointers to NULL
        // debug_log("No columns remain, freeing arrays");
        free(df->columns);
        free(df->column_names);
        df->columns = NULL;
        df->column_names = NULL;
    }

    // debug_log("Column deletion successful");
    return 1;
}
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

DataFrame* split_dataframe(DataFrame* df, const char* target_column, void** y) {
    if (df == NULL || target_column == NULL || y == NULL) {
        set_error("Invalid input parameters for split_dataframe");
        return NULL;
    }

    int target_col_index = -1;
    char* cleaned_target_column = strdup(target_column);
    
    // Remove quotes from the input target_column if present
    int len = strlen(cleaned_target_column);
    if (len >= 2 && cleaned_target_column[0] == '"' && cleaned_target_column[len-1] == '"') {
        cleaned_target_column[len-1] = '\0';
        memmove(cleaned_target_column, cleaned_target_column + 1, len - 1);
    }

    for (int i = 0; i < df->num_columns; i++) {
        char* column_name = strdup(df->column_names[i]);
        
        // Remove quotes from the column name if present
        len = strlen(column_name);
        if (len >= 2 && column_name[0] == '"' && column_name[len-1] == '"') {
            column_name[len-1] = '\0';
            memmove(column_name, column_name + 1, len - 1);
        }
        
        if (strcmp(column_name, cleaned_target_column) == 0) {
            target_col_index = i;
            free(column_name);
            break;
        }
        free(column_name);
    }

    free(cleaned_target_column);

    if (target_col_index == -1) {
        set_error("Target column '%s' not found in the DataFrame", target_column);
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
void matrix_transpose(float *A, float *AT, int m, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            AT[j*m + i] = A[i*n + j];
        }
    }
}

// Helper function for matrix inversion (using Gauss-Jordan elimination)
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

void free_linear_regression_model(LinearRegressionModel *model) {
    if (model) {
        free(model->coefficients);
        free(model);
    }
}

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

float calculate_mse(float *y_true, float *y_pred, int n) {
    float mse = 0.0f;
    for (int i = 0; i < n; i++) {
        float error = y_true[i] - y_pred[i];
        mse += error * error;
    }
    return mse / n;
}

// New functions for sorting and searching

// Compare functions for different data types
static int compare_int(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

static int compare_float(const void* a, const void* b) {
    float diff = *(float*)a - *(float*)b;
    return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
}

static int compare_string(const void* a, const void* b) {
    return strcmp(*(char**)a, *(char**)b);
}

// Function to sort DataFrame by a specified column
DataFrame* df_sort(const char* column_name, DataFrame* df) {
    if (df == NULL || column_name == NULL) {
        set_error("Invalid DataFrame or column name");
        return NULL;
    }

    // Find the column index
    int column_index = -1;
    for (int i = 0; i < df->num_columns; i++) {
        if (strcmp(df->column_names[i], column_name) == 0) {
            column_index = i;
            break;
        }
    }

    if (column_index == -1) {
        set_error("Column '%s' not found in DataFrame", column_name);
        return NULL;
    }

    // Create a new sorted DataFrame
    DataFrame* sorted_df = sort_dataframe(df, column_index, 1); // 1 for ascending order
    if (sorted_df == NULL) {
        set_error("Failed to sort DataFrame");
        return NULL;
    }

    return sorted_df;
}

// Binary search function that returns indices of matching elements
int binary_search(DataFrame* df, int column_index, void* target, int* found_indices, int max_indices) {
    if (df == NULL || column_index < 0 || column_index >= df->num_columns || found_indices == NULL) {
        set_error("Invalid parameters for binary search");
        return 0;
    }

    Column* column = &df->columns[column_index];
    int found_count = 0;
    
    // Binary search works only on sorted data
    // First check if middle element exists
    if (df->num_rows == 0) {
        return 0;
    }

    int left = 0;
    int right = df->num_rows - 1;
    int mid = 0;
    int compare_result = 0;

    // Binary search to find any occurrence
    while (left <= right) {
        mid = left + (right - left) / 2;
        
        // Compare based on data type
        switch (column->type) {
            case TYPE_INT:
                compare_result = *(int*)target - ((int*)column->data)[mid];
                break;
            case TYPE_FLOAT:
                {
                    float diff = *(float*)target - ((float*)column->data)[mid];
                    compare_result = (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                }
                break;
            case TYPE_STRING:
                compare_result = strcmp((char*)target, ((char**)column->data)[mid]);
                break;
            default:
                set_error("Unsupported data type for binary search");
                return 0;
        }

        if (compare_result == 0) {
            // Found a match
            found_indices[found_count++] = mid;
            
            // Check if we've reached the limit for found indices
            if (found_count >= max_indices) {
                break;
            }
            
            // Search for more matches to the left
            int left_ptr = mid - 1;
            while (left_ptr >= 0) {
                int compare_left = 0;
                switch (column->type) {
                    case TYPE_INT:
                        compare_left = *(int*)target - ((int*)column->data)[left_ptr];
                        break;
                    case TYPE_FLOAT:
                        {
                            float diff = *(float*)target - ((float*)column->data)[left_ptr];
                            compare_left = (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                        }
                        break;
                    case TYPE_STRING:
                        compare_left = strcmp((char*)target, ((char**)column->data)[left_ptr]);
                        break;
                }
                
                if (compare_left == 0) {
                    found_indices[found_count++] = left_ptr;
                    if (found_count >= max_indices) break;
                    left_ptr--;
                } else {
                    break;
                }
            }
            
            // Search for more matches to the right
            int right_ptr = mid + 1;
            while (right_ptr < df->num_rows && found_count < max_indices) {
                int compare_right = 0;
                switch (column->type) {
                    case TYPE_INT:
                        compare_right = *(int*)target - ((int*)column->data)[right_ptr];
                        break;
                    case TYPE_FLOAT:
                        {
                            float diff = *(float*)target - ((float*)column->data)[right_ptr];
                            compare_right = (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                        }
                        break;
                    case TYPE_STRING:
                        compare_right = strcmp((char*)target, ((char**)column->data)[right_ptr]);
                        break;
                }
                
                if (compare_right == 0) {
                    found_indices[found_count++] = right_ptr;
                    if (found_count >= max_indices) break;
                    right_ptr++;
                } else {
                    break;
                }
            }
            
            return found_count;
        } else if (compare_result < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    
    return found_count;
}

// Jump search function that returns indices of matching elements
int jump_search(DataFrame* df, int column_index, void* target, int* found_indices, int max_indices) {
    if (df == NULL || column_index < 0 || column_index >= df->num_columns || found_indices == NULL) {
        set_error("Invalid parameters for jump search");
        return 0;
    }

    Column* column = &df->columns[column_index];
    int found_count = 0;
    int n = df->num_rows;
    
    if (n == 0) {
        return 0;
    }
    
    // Finding block size to be jumped
    int step = (int)sqrt(n);
    
    // Finding the block where element is present (if it is present)
    int prev = 0;
    int compare_result = 0;
    
    // Determine the comparison function based on data type
    while (prev < n) {
        switch (column->type) {
            case TYPE_INT:
                compare_result = *(int*)target - ((int*)column->data)[prev];
                break;
            case TYPE_FLOAT:
                {
                    float diff = *(float*)target - ((float*)column->data)[prev];
                    compare_result = (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                }
                break;
            case TYPE_STRING:
                compare_result = strcmp((char*)target, ((char**)column->data)[prev]);
                break;
            default:
                set_error("Unsupported data type for jump search");
                return 0;
        }
        
        if (compare_result == 0) {
            // Found a match
            found_indices[found_count++] = prev;
            
            // Check for more matches in the vicinity
            int left = prev - 1;
            while (left >= 0 && found_count < max_indices) {
                int compare_left = 0;
                switch (column->type) {
                    case TYPE_INT:
                        compare_left = *(int*)target - ((int*)column->data)[left];
                        break;
                    case TYPE_FLOAT:
                        {
                            float diff = *(float*)target - ((float*)column->data)[left];
                            compare_left = (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                        }
                        break;
                    case TYPE_STRING:
                        compare_left = strcmp((char*)target, ((char**)column->data)[left]);
                        break;
                }
                
                if (compare_left == 0) {
                    found_indices[found_count++] = left;
                    left--;
                } else {
                    break;
                }
            }
            
            int right = prev + 1;
            while (right < n && found_count < max_indices) {
                int compare_right = 0;
                switch (column->type) {
                    case TYPE_INT:
                        compare_right = *(int*)target - ((int*)column->data)[right];
                        break;
                    case TYPE_FLOAT:
                        {
                            float diff = *(float*)target - ((float*)column->data)[right];
                            compare_right = (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                        }
                        break;
                    case TYPE_STRING:
                        compare_right = strcmp((char*)target, ((char**)column->data)[right]);
                        break;
                }
                
                if (compare_right == 0) {
                    found_indices[found_count++] = right;
                    right++;
                } else {
                    break;
                }
            }
            
            return found_count;
        } else if (compare_result < 0) {
            // Element is in this block, perform linear search
            break;
        }
        
        prev = (prev + step < n) ? prev + step : n;
    }
    
    // Linear search in the identified block
    int i = prev - step;
    if (i < 0) i = 0;
    
    while (i < n && i <= prev) {
        int compare_linear = 0;
        switch (column->type) {
            case TYPE_INT:
                compare_linear = *(int*)target - ((int*)column->data)[i];
                break;
            case TYPE_FLOAT:
                {
                    float diff = *(float*)target - ((float*)column->data)[i];
                    compare_linear = (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                }
                break;
            case TYPE_STRING:
                compare_linear = strcmp((char*)target, ((char**)column->data)[i]);
                break;
        }
        
        if (compare_linear == 0) {
            // Found a match
            found_indices[found_count++] = i;
            
            // Check for more matches linearly
            int j = i + 1;
            while (j < n && found_count < max_indices) {
                int compare_next = 0;
                switch (column->type) {
                    case TYPE_INT:
                        compare_next = *(int*)target - ((int*)column->data)[j];
                        break;
                    case TYPE_FLOAT:
                        {
                            float diff = *(float*)target - ((float*)column->data)[j];
                            compare_next = (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
                        }
                        break;
                    case TYPE_STRING:
                        compare_next = strcmp((char*)target, ((char**)column->data)[j]);
                        break;
                }
                
                if (compare_next == 0) {
                    found_indices[found_count++] = j;
                    j++;
                } else {
                    break;
                }
            }
            
            return found_count;
        }
        
        i++;
    }
    
    return found_count;
}

// Function to search for rows containing a value and return a new DataFrame
DataFrame* df_search(const char* search_value, DataFrame* sorted_df) {
    if (sorted_df == NULL || search_value == NULL) {
        set_error("Invalid parameters for df_search");
        return NULL;
    }
    
    // Initialize result DataFrame
    DataFrame* result_df = create_dataframe();
    if (result_df == NULL) {
        return NULL;
    }
    
    // Determine the search column based on the search value type and data in DataFrame
    int search_column_index = -1;
    ColumnType search_type = TYPE_STRING; // Default assumption
    
    // Try to determine if the search value is a number
    char* endptr;
    strtol(search_value, &endptr, 10);
    bool is_int = (*endptr == '\0');
    
    strtod(search_value, &endptr);
    bool is_float = (*endptr == '\0');
    
    // Find an appropriate column to search based on the value type
    if (is_int) {
        search_type = TYPE_INT;
        // Look for the first integer column
        for (int i = 0; i < sorted_df->num_columns; i++) {
            if (sorted_df->columns[i].type == TYPE_INT) {
                search_column_index = i;
                break;
            }
        }
    } else if (is_float) {
        search_type = TYPE_FLOAT;
        // Look for the first float column
        for (int i = 0; i < sorted_df->num_columns; i++) {
            if (sorted_df->columns[i].type == TYPE_FLOAT) {
                search_column_index = i;
                break;
            }
        }
    } else {
        search_type = TYPE_STRING;
        // Look for the first string column
        for (int i = 0; i < sorted_df->num_columns; i++) {
            if (sorted_df->columns[i].type == TYPE_STRING) {
                search_column_index = i;
                break;
            }
        }
    }
    
    // If no suitable column was found
    if (search_column_index == -1) {
        set_error("No suitable column found for search value type");
        free_dataframe(result_df);
        return NULL;
    }
    
    // Create an array to hold the found indices
    int max_indices = sorted_df->num_rows;
    int* found_indices = (int*)malloc(max_indices * sizeof(int));
    if (found_indices == NULL) {
        set_error("Memory allocation failed");
        free_dataframe(result_df);
        return NULL;
    }
    
    // Prepare the search target based on the column type
    void* target = NULL;
    int target_int;
    float target_float;
    
    switch (sorted_df->columns[search_column_index].type) {
        case TYPE_INT:
            target_int = atoi(search_value);
            target = &target_int;
            break;
        case TYPE_FLOAT:
            target_float = atof(search_value);
            target = &target_float;
            break;
        case TYPE_STRING:
            target = (void*)search_value;
            break;
        default:
            set_error("Unsupported data type for search");
            free(found_indices);
            free_dataframe(result_df);
            return NULL;
    }
    
    // Perform binary search
    int found_count = binary_search(sorted_df, search_column_index, target, found_indices, max_indices);
    
    if (found_count == 0) {
        free(found_indices);
        free_dataframe(result_df);
        set_error("No matching rows found");
        return NULL;
    }
    
    // Create columns for the result DataFrame
    for (int col = 0; col < sorted_df->num_columns; col++) {
        Column* src_column = &sorted_df->columns[col];
        void* new_data = malloc(found_count * get_column_element_size(src_column->type));
        if (new_data == NULL) {
            set_error("Memory allocation failed");
            free(found_indices);
            free_dataframe(result_df);
            return NULL;
        }
        
        // Copy the data for the found indices
        for (int i = 0; i < found_count; i++) {
            int row_idx = found_indices[i];
            switch (src_column->type) {
                case TYPE_INT:
                    ((int*)new_data)[i] = ((int*)src_column->data)[row_idx];
                    break;
                case TYPE_FLOAT:
                    ((float*)new_data)[i] = ((float*)src_column->data)[row_idx];
                    break;
                case TYPE_STRING:
                    ((char**)new_data)[i] = strdup(((char**)src_column->data)[row_idx]);
                    if (((char**)new_data)[i] == NULL) {
                        set_error("Memory allocation failed for string copy");
                        free(new_data);
                        free(found_indices);
                        free_dataframe(result_df);
                        return NULL;
                    }
                    break;
            }
        }
        
        // Add the column to the result DataFrame
        if (!add_column(result_df, sorted_df->column_names[col], src_column->type, new_data, found_count)) {
            set_error("Failed to add column to result DataFrame");
            free(new_data);
            free(found_indices);
            free_dataframe(result_df);
            return NULL;
        }
        
        free(new_data);
    }
    
    free(found_indices);
    return result_df;
}