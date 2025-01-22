#ifndef CLENZDAT_H
#define CLENZDAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_COLUMNS 100
#define MAX_ROWS 100
#define MAX_STRING_LENGTH 256

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} ColumnType;

typedef struct {
    void *data;
    ColumnType type;
    int length;
} Column;

typedef struct {
    Column columns[MAX_COLUMNS];
    char *column_names[MAX_COLUMNS];
    int num_columns;
    int num_rows;
} DataFrame;

// DataFrame operations
DataFrame* create_dataframe();
void free_dataframe(DataFrame *df);
int add_column(DataFrame *df, const char *name, ColumnType type, void *data, int length);
void print_dataframe(DataFrame *df);

// Data preprocessing functions
/*
 * Handle missing values with the following strategies:
 * 1. Remove rows with null values
 * 2. Replace with mean
 * 3. Replace with median
 * 4. Replace with mode
 */
DataFrame* handle_missing_values(DataFrame *df, int column_index, const char *strategy);
DataFrame* normalize_column(DataFrame *df, int column_index);
DataFrame* scale_column(DataFrame *df, int column_index, float min, float max);
DataFrame* one_hot_encode(DataFrame *df, int column_index);

// Helper function prototypes
float calculate_mean(float *data, int length);
float calculate_median(float *data, int length);
float calculate_mode(float *data, int length);
float calculate_variance(float *data, int length, float mean);
float calculate_std_deviation(float variance);
float calculate_quartile(float *data, int length, float percentile);
// File operations
// Add these function prototypes
DataFrame* read_csv(const char *filename);
// Update the function declaration
void* get_column_as_array(DataFrame *df, int column_index, ColumnType *type);
DataFrame* append_dataframe(DataFrame *df1, DataFrame *df2);
int write_csv(DataFrame *df, const char *filename);
int* shape_df(DataFrame *df);
DataFrame* get_dataframe_range(DataFrame *df, int start_row, int end_row);
DataFrame* sort_dataframe(DataFrame *df, int column_index, int ascending);
size_t get_column_element_size(ColumnType type);

// New function prototype
DataFrame* describe_dataframe(DataFrame *df);

// Error handling
extern char error_message[256];
void set_error(const char *format, ...);

#endif

