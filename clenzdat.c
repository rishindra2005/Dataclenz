#include "clenzdat.h"
#include <stdarg.h>
#include <ctype.h>
#include <float.h>
char error_message[256] = {0};


void set_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(error_message, sizeof(error_message) - 1, format, args);
    va_end(args);
}


DataFrame* create_dataframe() {
    DataFrame *df = (DataFrame*)malloc(sizeof(DataFrame));
    if (df == NULL) {
        set_error("Failed to allocate memory for DataFrame");
        return NULL;
    }
    df->num_columns = 0;
    df->num_rows = 0;
    return df;
}

void free_dataframe(DataFrame *df) {
    if (df == NULL) return;

    for (int i = 0; i < df->num_columns; i++) {
        free(df->columns[i].data);
        free(df->column_names[i]);
    }
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
            break;
        case TYPE_FLOAT:
            data_size = sizeof(float) * length;
            break;
        case TYPE_STRING:
            data_size = sizeof(char*) * length;
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
                    len = strlen(((char**)df->columns[i].data)[row]);
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
    if (df->num_rows > 30) {
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
        int actual_row = row < 10 ? row : df->num_rows - (20 - row);

        if (print_ellipsis && row == 10) {
            printf("| %-*s ", col_widths[0], "...");
            for (int col = 0; col < df->num_columns; col++) {
                printf("| %-*s ", col_widths[col + 1], "...");
            }
            printf("|\n");
            continue;
        }

        printf("| %-*d ", col_widths[0], actual_row + 1);  // Print serial number
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
                    if (strlen(str) > col_widths[col + 1]) {
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

 // Function to handle missing values
 DataFrame* handle_missing_values(DataFrame *df, int column_index, const char *strategy) {
     if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
         set_error("Invalid DataFrame or column index");
         return NULL;
     }

     Column *column = &df->columns[column_index];

     if (column->type != TYPE_FLOAT) {
         set_error("Handling missing values is only implemented for float columns");
         return NULL;
     }

     float *data = (float*)column->data;

     if (strcmp(strategy, "remove") == 0) {
         // Remove rows with null values
         int new_row_count = 0;
         for (int i = 0; i < df->num_rows; i++) {
             if (!isnan(data[i])) {
                 for (int j = 0; j < df->num_columns; j++) {
                     memcpy((char*)df->columns[j].data + new_row_count * sizeof(float),
                            (char*)df->columns[j].data + i * sizeof(float),
                            sizeof(float));
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

DataFrame* one_hot_encode(DataFrame *df, int column_index) {
    if (df == NULL || column_index < 0 || column_index >= df->num_columns) {
        set_error("Invalid DataFrame or column index");
        return NULL;
    }

    Column *column = &df->columns[column_index];
    if (column->type != TYPE_STRING) {
        set_error("One-hot encoding is only applicable to string columns");
        return NULL;
    }

    // Count unique values
    char *unique_values[MAX_ROWS];
    int unique_count = 0;

    for (int i = 0; i < df->num_rows; i++) {
        char *value = ((char**)column->data)[i];
        int is_unique = 1;
        for (int j = 0; j < unique_count; j++) {
            if (strcmp(value, unique_values[j]) == 0) {
                is_unique = 0;
                break;
            }
        }
        if (is_unique) {
            unique_values[unique_count++] = value;
        }
    }

    // Create a new DataFrame
    DataFrame *new_df = create_dataframe();
    if (new_df == NULL) {
        set_error("Failed to create new DataFrame for one-hot encoding");
        return NULL;
    }

    // Copy existing columns
    for (int i = 0; i < df->num_columns; i++) {
        if (i != column_index) {
            if (!add_column(new_df, df->column_names[i], df->columns[i].type, df->columns[i].data, df->num_rows)) {
                free_dataframe(new_df);
                return NULL;
            }
        }
    }

    // Create new columns for one-hot encoding
    for (int i = 0; i < unique_count; i++) {
        char new_column_name[MAX_STRING_LENGTH];
        snprintf(new_column_name, MAX_STRING_LENGTH, "%s_%s", df->column_names[column_index], unique_values[i]);

        int *new_column_data = calloc(df->num_rows, sizeof(int));
        if (new_column_data == NULL) {
            set_error("Memory allocation failed for one-hot encoded column");
            free_dataframe(new_df);
            return NULL;
        }

        for (int j = 0; j < df->num_rows; j++) {
            if (strcmp(((char**)column->data)[j], unique_values[i]) == 0) {
                new_column_data[j] = 1;
            }
        }

        if (!add_column(new_df, new_column_name, TYPE_INT, new_column_data, df->num_rows)) {
            free(new_column_data);
            free_dataframe(new_df);
            return NULL;
        }
        free(new_column_data);
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
        switch (type) {
            case TYPE_INT:
                ((int*)column_data)[i] = atoi(data[i]);
                break;
            case TYPE_FLOAT:
                ((float*)column_data)[i] = atof(data[i]);
                break;
            case TYPE_STRING:
                ((char**)column_data)[i] = strdup(data[i]);
                if (((char**)column_data)[i] == NULL) {
                    set_error("Memory allocation failed for string data");
                    free(column_data);
                    return 0;
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
    char *data[MAX_ROWS][MAX_COLUMNS];
    int num_rows = 0;

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
            for (int i = 0; i < num_columns; i++) {
                free(headers[i]);
            }
            free_dataframe(df);
            fclose(file);
            return NULL;
        }
        num_columns++;
        token = strtok(NULL, ",\n");
    }

    if (num_columns == 0) {
        set_error("No columns found in file: %s", filename);
        free_dataframe(df);
        fclose(file);
        return NULL;
    }

    // Read data
    while (fgets(line, sizeof(line), file) != NULL && num_rows < MAX_ROWS) {
        token = strtok(line, ",\n");
        int col = 0;
        while (token != NULL && col < num_columns) {
            data[num_rows][col] = strdup(token);
            if (data[num_rows][col] == NULL) {
                set_error("Memory allocation failed for data: row %d, column %d", num_rows + 1, col + 1);
                for (int i = 0; i < num_rows; i++) {
                    for (int j = 0; j < num_columns; j++) {
                        free(data[i][j]);
                    }
                }
                for (int i = 0; i < num_columns; i++) {
                    free(headers[i]);
                }
                free_dataframe(df);
                fclose(file);
                return NULL;
            }
            col++;
            token = strtok(NULL, ",\n");
        }
        if (col != num_columns) {
            set_error("Inconsistent number of columns in row %d", num_rows + 1);
            for (int i = 0; i <= num_rows; i++) {
                for (int j = 0; j < num_columns; j++) {
                    free(data[i][j]);
                }
            }
            for (int i = 0; i < num_columns; i++) {
                free(headers[i]);
            }
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
        free_dataframe(df);
        return NULL;
    }

    // Add columns using the new function
    for (int i = 0; i < num_columns; i++) {
        char *column_data[MAX_ROWS];
        for (int j = 0; j < num_rows; j++) {
            column_data[j] = data[j][i];
        }
        if (!add_column_from_strings(df, headers[i], column_data, num_rows)) {
            set_error("Failed to add column: %s", headers[i]);
            for (int j = 0; j < num_rows; j++) {
                for (int k = 0; k < num_columns; k++) {
                    free(data[j][k]);
                }
            }
            for (int j = 0; j < num_columns; j++) {
                free(headers[j]);
            }
            free_dataframe(df);
            return NULL;
        }
    }

    // Clean up
    for (int i = 0; i < num_columns; i++) {
        free(headers[i]);
    }
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_columns; j++) {
            free(data[i][j]);
        }
    }

    return df;
}



int write_csv(DataFrame *df, const char *filename) {
    if (df == NULL) {
        set_error("DataFrame is NULL");
        return 0;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        set_error("Unable to open file for writing");
        return 0;
    }

    // Write headers
    for (int i = 0; i < df->num_columns; i++) {
        fprintf(file, "%s%s", df->column_names[i], (i < df->num_columns - 1) ? "," : "\n");
    }

    // Write data
    for (int row = 0; row < df->num_rows; row++) {
        for (int col = 0; col < df->num_columns; col++) {
            switch (df->columns[col].type) {
                case TYPE_INT:
                    fprintf(file, "%d", ((int*)df->columns[col].data)[row]);
                    break;
                case TYPE_FLOAT:
                    fprintf(file, "%.2f", ((float*)df->columns[col].data)[row]);
                    break;
                case TYPE_STRING:
                    fprintf(file, "%s", ((char**)df->columns[col].data)[row]);
                    break;
            }
            fprintf(file, "%s", (col < df->num_columns - 1) ? "," : "\n");
        }
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

