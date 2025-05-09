# Dataclenz

Dataclenz is a C library for data manipulation, analysis, and machine learning. It provides functionality similar to pandas in Python but implemented in C for high performance and low-level control.

## Features

- **DataFrame Operations**: Create, manipulate, and analyze tabular data in a structured manner
- **Data Types Support**: Handle integer, float, and string data types
- **Data Preprocessing**: Handle missing values, normalize/scale data, and perform data transformations
- **Data Analysis**: Calculate statistical measures like mean, median, mode, variance, and standard deviation
- **File I/O**: Read and write CSV files with custom delimiters
- **Data Selection**: Sort, filter, and extract specific data ranges
- **Machine Learning**: Built-in linear regression with model evaluation metrics
- **Memory Management**: Efficient memory allocation and cleanup for large datasets

## Data Structures

### DataFrame

The core data structure that holds tabular data with columns of various types:

```c
typedef struct {
    Column *columns;
    char **column_names;
    int num_columns;
    int num_rows;
    int max_rows;
} DataFrame;
```

### Column

Represents a single column in a DataFrame:

```c
typedef struct {
    void *data;
    ColumnType type;
    int length;
} Column;
```

### ColumnType

An enumeration for the supported data types:

```c
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} ColumnType;
```

### LinearRegressionModel

Holds the coefficients and metadata for a linear regression model:

```c
typedef struct {
    float *coefficients;
    float intercept;
    int num_features;
} LinearRegressionModel;
```

## Basic Usage

Here's a simple example of creating a DataFrame with different types of data:

```c
#include <stdio.h>
#include <clenzdat.h>

int main() {
    // Create an empty DataFrame
    DataFrame *df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame.\n");
        return 1;
    }

    // Define some data
    int int_data[] = {1, 2, 3, 4, 5};
    float float_data[] = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f};
    char *string_data[] = {"one", "two", "three", "four", "five"};

    // Add columns to the DataFrame
    add_column(df, "Int Column", TYPE_INT, int_data, 5);
    add_column(df, "Float Column", TYPE_FLOAT, float_data, 5);
    add_column(df, "String Column", TYPE_STRING, string_data, 5);

    // Print the DataFrame
    printf("Sample DataFrame:\n");
    print_dataframe(df);

    // Free the DataFrame when done
    free_dataframe(df);
    return 0;
}
```

## Key Functions

### DataFrame Operations

- `DataFrame* create_dataframe()`: Create a new empty DataFrame
- `void free_dataframe(DataFrame *df)`: Free memory associated with a DataFrame
- `int add_column(DataFrame *df, const char *name, ColumnType type, void *data, int length)`: Add a column to a DataFrame
- `int delete_column(DataFrame *df, int column_index)`: Delete a column from a DataFrame
- `void print_dataframe(DataFrame *df)`: Display a DataFrame in tabular format
- `int print_dataframe_s(DataFrame *df, const char *filename)`: Save DataFrame output to a file

### Data Analysis

- `DataFrame* describe_dataframe(DataFrame *df)`: Generate descriptive statistics
- `float calculate_mean(float *data, int length)`: Calculate the mean of a numeric array
- `float calculate_median(float *data, int length)`: Calculate the median
- `float calculate_mode(float *data, int length)`: Calculate the mode
- `float calculate_variance(float *data, int length, float mean)`: Calculate variance
- `float calculate_std_deviation(float variance)`: Calculate standard deviation

### File Operations

- `DataFrame* read_csv(const char *filename)`: Read data from a CSV file
- `DataFrame* read_csv_d(const char *filename, char delimiter)`: Read CSV with custom delimiter
- `int write_csv(DataFrame *df, const char *filename)`: Write DataFrame to a CSV file

### Data Preprocessing

- `DataFrame* handle_missing_values(DataFrame *df, int column_index, const char *strategy)`: Handle missing values using various strategies ("remove", "mean", "median", "mode")
- `DataFrame* normalize_column(DataFrame *df, int column_index)`: Normalize a column to range [0,1]
- `DataFrame* scale_column(DataFrame *df, int column_index, float min, float max)`: Scale a column to a specified range

### Data Manipulation

- `int change_value(DataFrame *df, int row_index, int column_index, void *new_value)`: Change a specific value
- `int replace_value(DataFrame *df, int column_index, void *old_value, void *new_value)`: Replace all occurrences of a value
- `DataFrame* sort_dataframe(DataFrame *df, int column_index, int ascending)`: Sort DataFrame by a column
- `DataFrame* get_dataframe_range(DataFrame *df, int start_row, int end_row)`: Extract a range of rows
- `DataFrame* append_dataframe(DataFrame *df1, DataFrame *df2)`: Combine two DataFrames

### Machine Learning

- `DataFrame* split_dataframe(DataFrame* df, const char* target_column, void** y)`: Split into features and target
- `LinearRegressionModel* create_linear_regression_model(int num_features)`: Create a linear regression model
- `int fit_linear_regression(LinearRegressionModel *model, DataFrame *X, float *y)`: Fit a linear model
- `float* predict_linear_regression(LinearRegressionModel *model, DataFrame *X)`: Make predictions
- `float calculate_r_squared(float *y_true, float *y_pred, int n)`: Calculate R² score
- `float calculate_mse(float *y_true, float *y_pred, int n)`: Calculate mean squared error

## Example: Linear Regression

```c
#include <stdio.h>
#include <stdlib.h>
#include <clenzdat.h>

int main() {
    // Read data from CSV
    DataFrame* df = read_csv("housing.csv");
    if (df == NULL) {
        printf("Failed to read CSV file.\n");
        return 1;
    }
    
    // Preprocess data: remove non-numeric columns and handle missing values
    for (int i = 0; i < df->num_columns; i++) {
        if (df->columns[i].type == TYPE_STRING) {
            delete_column(df, i);
            i--; // Adjust index after deletion
        }
    }
    
    // Handle missing values by removing rows
    for (int i = 0; i < df->num_columns; i++) {
        df = handle_missing_values(df, i, "remove");
    }
    
    // Normalize features
    for (int i = 0; i < df->num_columns - 1; i++) {
        df = normalize_column(df, i);
    }
    
    // Split into features (X) and target (y)
    float* y;
    DataFrame* X = split_dataframe(df, "median_house_value", (void**)&y);
    
    // Create and fit linear regression model
    LinearRegressionModel* model = create_linear_regression_model(X->num_columns);
    fit_linear_regression(model, X, y);
    
    // Make predictions
    float* y_pred = predict_linear_regression(model, X);
    
    // Evaluate model
    float r_squared = calculate_r_squared(y, y_pred, X->num_rows);
    float mse = calculate_mse(y, y_pred, X->num_rows);
    
    printf("Model Evaluation:\n");
    printf("R-squared: %.4f\n", r_squared);
    printf("Mean Squared Error: %.4f\n", mse);
    
    // Clean up
    free_linear_regression_model(model);
    free_dataframe(df);
    free_dataframe(X);
    free(y);
    free(y_pred);
    
    return 0;
}
```

## Building and Installation

### For Windows

1. Compile the library:
   ```
   gcc -c clenzdat.c -o clenzdat.o
   ```

2. Create a static library:
   ```
   ar rcs libclenzdat.a clenzdat.o
   ```

3. Create a shared library (DLL):
   ```
   gcc -shared -o clenzdat.dll clenzdat.c -lclenzdat
   ```

4. Compile your program with the library:
   ```
   gcc -o your_program your_program.c -lm -lclenzdat
   ```

### For Linux

1. Compile the library:
   ```
   gcc -c -fPIC clenzdat.c -o clenzdat.o
   ```

2. Create a static library:
   ```
   ar rcs libclenzdat.a clenzdat.o
   ```

3. Create a shared library:
   ```
   gcc -shared -o libclenzdat.so clenzdat.o
   ```

4. Compile your program with the library:
   ```
   gcc -o your_program your_program.c -lm -lclenzdat
   ```

## Memory Management

Dataclenz allocates memory dynamically, so it's essential to free resources when they're no longer needed:

- Always call `free_dataframe()` for DataFrames you've created
- Call `free_linear_regression_model()` for any models you've created
- Free any arrays returned by functions like `predict_linear_regression()`

## License

This library is available under the MIT License. See the LICENSE file for more details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Support

For issues and questions, please open an issue on the project's GitHub page. 