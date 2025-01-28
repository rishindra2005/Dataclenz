#ifndef CLENZDAT_H
#define CLENZDAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_COLUMNS 1000
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
    Column *columns;  // Change to a pointer
    char **column_names;  // Change to a pointer to pointers
    int num_columns;
    int num_rows;
    int max_rows;  // Add this field to keep track of allocated rows
} DataFrame;
typedef struct {
    float *coefficients;
    float intercept;
    int num_features;
} LinearRegressionModel;



// DataFrame operations
/**
 * @brief Creates a new, empty DataFrame.
 *
 * This function initializes a new DataFrame structure, allocating memory for
 * the DataFrame itself and setting initial values for its members. The created
 * DataFrame has no columns or rows initially.
 *
 * @return A pointer to the newly created DataFrame, or NULL if memory allocation fails.
 *         The caller is responsible for freeing the returned DataFrame using free_dataframe().
 */
DataFrame* create_dataframe();



/**
 * @brief Frees all memory associated with a DataFrame.
 *
 * This function deallocates all memory that was dynamically allocated for the DataFrame,
 * including its columns, column names, and the DataFrame structure itself.
 *
 * @param df Pointer to the DataFrame to be freed. If NULL, the function does nothing.
 *
 * @return This function does not return a value.
 *
 * @note After calling this function, the DataFrame pointer should not be used again.
 */
void free_dataframe(DataFrame *df);




/**
 * @brief Adds a new column to the DataFrame.
 *
 * This function adds a new column to the specified DataFrame with the given name,
 * data type, data, and length.
 *
 * @param df Pointer to the DataFrame to which the column will be added.
 * @param name The name of the new column.
 * @param type The data type of the new column (TYPE_INT, TYPE_FLOAT, or TYPE_STRING).
 * @param data Pointer to the data to be added to the new column.
 * @param length The number of elements in the data array.
 *
 * @return Returns 0 on success, or a non-zero value on failure.
 */
// File operations

/**
 * @brief Reads a CSV file and creates a DataFrame from its contents.
 *
 * This function reads the contents of a CSV file and creates a DataFrame
 * structure to store the data. The first row of the CSV file is assumed to
 * contain column names, and the subsequent rows contain data values.
 *
 * @param filename The name of the CSV file to read.
 *
 * @return A pointer to the DataFrame created from the CSV file, or NULL if an error occurs.
 *         The caller is responsible for freeing the returned DataFrame using free_dataframe().
 */
DataFrame* read_csv(const char *filename);
int add_column(DataFrame *df, const char *name, ColumnType type, void *data, int length);


/**
 * @brief Deletes a column from the DataFrame.
 *
 * This function deletes the column at the specified index from the DataFrame.
 *
 * @param df Pointer to the DataFrame from which the column will be deleted.
 * @param column_index The index of the column to be deleted.
 *
 * @return Returns 0 on success, or a non-zero value on failure.
 */
int delete_column(DataFrame *df, int column_index) ;

/**
 * @brief Prints the contents of a DataFrame to the console.
 *
 * This function displays the DataFrame's contents in a tabular format,
 * showing column names and data for each row.
 *
 * @param df Pointer to the DataFrame to be printed. If NULL, the function
 *           will return without printing anything.
 *
 * this prints the DataFrame to the console in the following format:
 * Column 1  Column 2  Column 3  ...
 * -------   -------   -------   ...
 * 1         2         3         ...    
 * 2         4         6         ...    
 * 3         6         9         ...
 */
void print_dataframe(DataFrame *df);



// Data preprocessing functions
/**
 * @brief Handles missing values in a specified column of a DataFrame.
 *
 * This function applies a specified strategy to handle missing values in a given column
 * of the DataFrame. The strategy can be one of: removing rows with null values,
 * replacing with mean, median, or mode.
 *
 * @param df Pointer to the DataFrame to be processed. Must not be NULL.
 * @param column_index The index of the column to handle missing values in.
 *                     Must be a valid column index in the DataFrame.
 * @param strategy A string specifying the strategy to use. Valid options are:
 *                 "remove" - Remove rows with null values
 *                 "mean" - Replace null values with the mean of the column
 *                 "median" - Replace null values with the median of the column
 *                 "mode" - Replace null values with the mode of the column
 *
 * @return A pointer to a new DataFrame with missing values handled according to
 *         the specified strategy. Returns NULL if an error occurs (e.g., invalid
 *         input parameters or memory allocation failure).
 */
DataFrame* handle_missing_values(DataFrame *df, int column_index, const char *strategy);

/**
 * @brief Normalizes a specified column in the DataFrame.
 *
 * This function applies normalization to the specified column of the DataFrame.
 * Normalization scales the values in the column to have a mean of 0 and a standard deviation of 1.
 *
 * @param df Pointer to the DataFrame containing the column to be normalized.
 *           Must not be NULL.
 * @param column_index The index of the column to be normalized.
 *                     Must be a valid column index in the DataFrame.
 *
 * @return A pointer to a new DataFrame with the specified column normalized.
 *         Returns NULL if an error occurs (e.g., invalid input parameters or memory allocation failure).
 */
DataFrame* normalize_column(DataFrame *df, int column_index);

/**
 * @brief Scales a specified column in the DataFrame to a given range.
 *
 * This function applies min-max scaling to the specified column of the DataFrame,
 * transforming its values to fall within the specified range [min, max].
 *
 * @param df Pointer to the DataFrame containing the column to be scaled.
 *           Must not be NULL.
 * @param column_index The index of the column to be scaled.
 *                     Must be a valid column index in the DataFrame.
 * @param min The minimum value of the desired range for scaling.
 * @param max The maximum value of the desired range for scaling.
 *
 * @return A pointer to a new DataFrame with the specified column scaled.
 *         Returns NULL if an error occurs (e.g., invalid input parameters or memory allocation failure).
 */
DataFrame* scale_column(DataFrame *df, int column_index, float min, float max);

/**
 * @brief Changes a specific value in the DataFrame.
 *
 * This function modifies the value at the specified row and column in the DataFrame.
 *
 * @param df Pointer to the DataFrame to be modified. Must not be NULL.
 * @param row_index The index of the row where the value should be changed.
 *                  Must be a valid row index in the DataFrame.
 * @param column_index The index of the column where the value should be changed.
 *                     Must be a valid column index in the DataFrame.
 * @param new_value Pointer to the new value to be set. The type of the value
 *                  should match the type of the specified column.
 *
 * @return Returns 0 on success, or a non-zero value on failure (e.g., invalid indices).
 */
int change_value(DataFrame *df, int row_index, int column_index, void *new_value);

/**
 * @brief Replaces all occurrences of a specific value with a new value in a DataFrame column.
 *
 * This function searches for all instances of 'old_value' in the specified column of the DataFrame
 * and replaces them with 'new_value'.
 *
 * @param df Pointer to the DataFrame to be modified. Must not be NULL.
 * @param column_index The index of the column where values should be replaced.
 *                     Must be a valid column index in the DataFrame.
 * @param old_value Pointer to the value to be replaced. The type should match the column's data type.
 * @param new_value Pointer to the new value to replace the old value. The type should match the column's data type.
 *
 * @return Returns 0 on success, or a non-zero value on failure (e.g., invalid DataFrame or column index).
 */
int replace_value(DataFrame *df, int column_index, void *old_value, void *new_value);

/**
 * @brief Prints the unique values in a specified column of a DataFrame.
 *
 * This function identifies and prints all unique values present in the specified
 * column of the given DataFrame. It handles different data types (int, float, string)
 * and ensures that each value is printed only once.
 *
 * @param df Pointer to the DataFrame containing the column to be analyzed.
 *           Must not be NULL.
 * @param column_index The index of the column to print unique values from.
 *                     Must be a valid column index in the DataFrame.
 *
 * @return Returns 0 on success, or a non-zero value on failure (e.g., invalid
 *         DataFrame pointer, invalid column index, or memory allocation failure).
 */
int print_unique_values(DataFrame *df, int column_index);

/**
 * @brief Appends one DataFrame to another.
 *
 * This function combines two DataFrames by appending the rows of the second DataFrame (df2)
 * to the end of the first DataFrame (df1). The columns of both DataFrames must match
 * in number and type for the operation to succeed.
 *
 * @param df1 Pointer to the first DataFrame, to which df2 will be appended.
 *            Must not be NULL and must have a valid structure.
 * @param df2 Pointer to the second DataFrame, whose rows will be appended to df1.
 *            Must not be NULL and must have a valid structure.
 *
 * @return A pointer to a new DataFrame containing the combined data of df1 and df2.
 *         Returns NULL if the operation fails (e.g., if the DataFrames are incompatible
 *         or if memory allocation fails).
 */
DataFrame* append_dataframe(DataFrame *df1, DataFrame *df2);

/**
 * @brief Writes the contents of a DataFrame to a CSV file.
 *
 * This function takes a DataFrame and writes its contents to a CSV (Comma-Separated Values) file
 * with the specified filename. The first row of the CSV file will contain the column names,
 * and subsequent rows will contain the data from the DataFrame.
 *
 * @param df Pointer to the DataFrame to be written to the CSV file. Must not be NULL.
 * @param filename The name of the file to write the CSV data to. Must be a valid filename string.
 *
 * @return Returns 0 on successful write operation, or a non-zero value if an error occurs
 *         (e.g., file creation failure, invalid DataFrame, or write operation failure).
 */
int write_csv(DataFrame *df, const char *filename);

/**
 * @brief Retrieves the shape (dimensions) of a DataFrame.
 *
 * This function calculates and returns the number of rows and columns in the given DataFrame.
 *
 * @param df Pointer to the DataFrame whose shape is to be determined. Must not be NULL.
 *
 * @return A pointer to an integer array of size 2, where:
 *         - The first element (index 0) represents the number of rows.
 *         - The second element (index 1) represents the number of columns.
 *         Returns NULL if the input DataFrame is invalid or if memory allocation fails.
 *         The caller is responsible for freeing the returned array.
 */
int* shape_df(DataFrame *df);

/**
 * @brief Extracts a range of rows from a DataFrame.
 *
 * This function creates a new DataFrame containing a subset of rows from the input DataFrame,
 * starting from 'start_row' and ending at 'end_row' (inclusive).
 *
 * @param df Pointer to the source DataFrame. Must not be NULL.
 * @param start_row The index of the first row to include in the range (0-based).
 * @param end_row The index of the last row to include in the range (inclusive).
 *
 * @return A pointer to a new DataFrame containing the specified range of rows.
 *         Returns NULL if an error occurs (e.g., invalid input parameters or memory allocation failure).
 */
DataFrame* get_dataframe_range(DataFrame *df, int start_row, int end_row);

/**
 * @brief Sorts a DataFrame based on a specified column.
 *
 * This function creates a new DataFrame that is a sorted version of the input DataFrame.
 * The sorting is performed based on the values in the specified column.
 *
 * @param df Pointer to the DataFrame to be sorted. Must not be NULL.
 * @param column_index The index of the column to use for sorting. Must be a valid column index.
 * @param ascending If non-zero, sort in ascending order; if zero, sort in descending order.
 *
 * @return A pointer to a new DataFrame containing the sorted data.
 *         Returns NULL if an error occurs (e.g., invalid input parameters or memory allocation failure).
 */
DataFrame* sort_dataframe(DataFrame *df, int column_index, int ascending);

// New function prototype
/**
 * @brief Generates a statistical summary of a DataFrame.
 *
 * This function creates a new DataFrame containing statistical descriptions
 * of the numerical columns in the input DataFrame. The summary includes
 * count,nulls, mean, standard deviation, minimum, 25th percentile, median (50th percentile),
 * 75th percentile, and maximum for each numerical column,unique values for String coloumns.
 *
 * @param df Pointer to the DataFrame to be described. Must not be NULL and
 *           should contain at least one numerical column.
 *
 * @return A pointer to a new DataFrame containing the statistical summary.
 *         Each row represents a statistic, and each column corresponds to
 *         a numerical column in the input DataFrame. Returns NULL if an error
 *         occurs (e.g., invalid input, memory allocation failure).
 */
DataFrame* describe_dataframe(DataFrame *df);

/**
 * @brief Prints the contents of a DataFrame to a file or the console.
 *
 * This function outputs the DataFrame's contents in a tabular format,
 * either to a specified file or to the console if no filename is provided.
 *
 * @param df Pointer to the DataFrame to be printed. Must not be NULL.
 * @param filename The name of the file to write the output to. If NULL,
 *                 the function will print to the console (stdout).
 *
 * @return Returns 0 on successful printing, or a non-zero value if an error occurs
 *         (e.g., file opening failure, invalid DataFrame pointer).
 */
int print_dataframe_s(DataFrame *df, const char *filename);


// Add this new function prototype
/**
 * @brief Resizes a DataFrame to a new number of rows.
 *
 * This function adjusts the size of a DataFrame by either increasing or decreasing
 * the number of rows. If increasing, it allocates additional memory for new rows.
 * If decreasing, it truncates the DataFrame to the specified number of rows.
 *
 * @param df Pointer to the DataFrame to be resized. Must not be NULL.
 * @param new_size The desired new number of rows for the DataFrame. Must be non-negative.
 *
 * @return Returns 0 on successful resizing, or a non-zero value if an error occurs
 *         (e.g., memory allocation failure, invalid input parameters).
 */
int resize_dataframe(DataFrame *df, int new_size);

/**
 * @brief Splits a DataFrame into features (X) and target (y) for machine learning tasks.
 *
 * This function separates the specified target column from the input DataFrame,
 * creating a new DataFrame with the remaining columns as features (X) and
 * storing the target column data separately (y).
 *
 * @param df Pointer to the input DataFrame to be split. Must not be NULL.
 * @param target_column Name of the column to be used as the target variable.
 *                      Must be a valid column name in the DataFrame.
 * @param y Pointer to a void pointer that will store the target column data.
 *          The function will allocate memory for this data, which the caller
 *          is responsible for freeing.
 *
 * @return A pointer to a new DataFrame containing all columns except the target column (X).
 *         Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *         The caller is responsible for freeing the returned DataFrame.
 */
DataFrame* split_dataframe(DataFrame* df, const char* target_column, void** y);











// Helper function prototypes
/**
 * @brief Calculates the arithmetic mean of an array of float values.
 *
 * This function computes the average value of the given array of floats.
 *
 * @param data Pointer to an array of float values.
 * @param length The number of elements in the data array.
 *
 * @return The arithmetic mean of the values in the data array.
 *         Returns 0.0 if the length is 0 or if data is NULL.
 */
float calculate_mean(float *data, int length);

/**
 * @brief Calculates the median value of an array of float values.
 *
 * This function computes the median (middle value) of the given array of floats.
 * If the array has an odd number of elements, it returns the middle value.
 * If the array has an even number of elements, it returns the average of the two middle values.
 *
 * @param data Pointer to an array of float values. Must not be NULL.
 * @param length The number of elements in the data array. Must be greater than 0.
 *
 * @return The median value of the data array.
 *         Returns 0.0 if the length is 0 or if data is NULL.
 */
float calculate_median(float *data, int length);

/**
 * @brief Calculates the mode (most frequent value) of an array of float values.
 *
 * This function determines the mode of the given array of floats. If multiple
 * values have the same highest frequency, it returns the first one encountered.
 *
 * @param data Pointer to an array of float values. Must not be NULL.
 * @param length The number of elements in the data array. Must be greater than 0.
 *
 * @return The mode (most frequent value) of the data array.
 *         Returns 0.0 if the length is 0 or if data is NULL.
 */
float calculate_mode(float *data, int length);

/**
 * @brief Calculates the variance of a set of float values.
 *
 * This function computes the variance of the given array of float values,
 * which measures how far a set of numbers are spread out from their average value.
 *
 * @param data Pointer to an array of float values. Must not be NULL.
 * @param length The number of elements in the data array. Must be greater than 0.
 * @param mean The pre-calculated arithmetic mean of the data array.
 *
 * @return The variance of the data array.
 *         Returns 0.0 if the length is 0 or if data is NULL.
 */
float calculate_variance(float *data, int length, float mean);

/**
 * @brief Calculates the standard deviation from the given variance.
 *
 * This function computes the standard deviation, which is the square root of the variance.
 * Standard deviation measures the amount of variation or dispersion of a set of values.
 *
 * @param variance The variance of the dataset. Must be a non-negative float value.
 *
 * @return The standard deviation as a float value.
 *         Returns 0.0 if the input variance is negative.
 */
float calculate_std_deviation(float variance);

/**
 * @brief Calculates the quartile value for a given percentile in a dataset.
 *
 * This function computes the quartile value corresponding to the specified percentile
 * in the given array of float values. It can be used to find various quartiles,
 * including the median (50th percentile), first quartile (25th percentile),
 * third quartile (75th percentile), or any other custom percentile.
 *
 * @param data Pointer to an array of float values. Must not be NULL.
 * @param length The number of elements in the data array. Must be greater than 0.
 * @param percentile The desired percentile, expressed as a float between 0 and 1.
 *                   For example, 0.25 for the first quartile, 0.5 for the median,
 *                   or 0.75 for the third quartile.
 *
 * @return The calculated quartile value as a float.
 *         Returns 0.0 if the input is invalid (NULL data, length <= 0, or percentile out of range).
 */
float calculate_quartile(float *data, int length, float percentile);


// Update the function declaration
/**
 * @brief Retrieves a column from a DataFrame as a contiguous array.
 *
 * This function extracts the data from a specified column in the DataFrame and
 * returns it as a contiguous array. It also provides the type of the column.
 *
 * @param df Pointer to the DataFrame from which to extract the column. Must not be NULL.
 * @param column_index The index of the column to retrieve. Must be a valid column index.
 * @param type Pointer to a ColumnType variable where the type of the column will be stored.
 *
 * @return A void pointer to the array containing the column data. The caller is
 *         responsible for casting this to the appropriate type and freeing the memory.
 *         Returns NULL if an error occurs (e.g., invalid DataFrame or column index).
 */
void* get_column_as_array(DataFrame *df, int column_index, ColumnType *type);


/**
 * @brief Determines the size in bytes of a single element for a given column type.
 *
 * This function returns the size in bytes that a single element occupies in memory
 * based on the specified ColumnType. It is useful for memory allocation and
 * pointer arithmetic operations on DataFrame columns.
 *
 * @param type The ColumnType for which to determine the element size.
 *             Valid values are TYPE_INT, TYPE_FLOAT, and TYPE_STRING.
 *
 * @return The size in bytes of a single element of the specified type.
 *         Returns sizeof(int) for TYPE_INT,
 *         sizeof(float) for TYPE_FLOAT,
 *         sizeof(char*) for TYPE_STRING,
 *         or 0 if an invalid type is provided.
 */
size_t get_column_element_size(ColumnType type);




//    Linear regression functions ML



/**
 * @brief Creates a new LinearRegressionModel with the specified number of features.
 *
 * This function allocates memory for a new LinearRegressionModel and initializes its
 * coefficients and intercept. The model is prepared to handle the specified number of features.
 *
 * @param num_features The number of features (independent variables) the model will use.
 *                     Must be a positive integer.
 *
 * @return A pointer to the newly created LinearRegressionModel.
 *         Returns NULL if memory allocation fails or if num_features is invalid.
 *         The caller is responsible for freeing the returned model using free_linear_regression_model().
 */
LinearRegressionModel* create_linear_regression_model(int num_features);

/**
 * @brief Frees the memory allocated for a LinearRegressionModel.
 *
 * This function deallocates all memory associated with the given LinearRegressionModel,
 * including the coefficients array and the model structure itself.
 *
 * @param model Pointer to the LinearRegressionModel to be freed. If NULL, the function does nothing.
 *
 * @return This function does not return a value.
 *
 * @note After calling this function, the model pointer should not be used again.
 */
void free_linear_regression_model(LinearRegressionModel *model);


/**
 * @brief Fits a linear regression model using the provided features and target values.
 *
 * This function performs linear regression to find the best-fit coefficients and intercept
 * for the given set of features (X) and target values (y). It updates the provided
 * LinearRegressionModel with the calculated coefficients and intercept.
 *
 * @param model Pointer to the LinearRegressionModel to be fitted. Must not be NULL and
 *              should be initialized with the correct number of features.
 * @param X Pointer to the DataFrame containing the feature data. Each column represents
 *          a feature, and each row represents an observation. Must not be NULL and
 *          should have the same number of rows as the length of y.
 * @param y Pointer to an array of float values representing the target variable.
 *          Must not be NULL and should have the same number of elements as rows in X.
 *
 * @return Returns 0 on successful fitting of the model, or a non-zero value if an error
 *         occurs (e.g., invalid input parameters, memory allocation failure, or
 *         computational error during fitting).
 */
int fit_linear_regression(LinearRegressionModel *model, DataFrame *X, float *y);

/**
 * @brief Predicts target values using a fitted linear regression model.
 *
 * This function applies the linear regression model to the provided feature data
 * to generate predictions for the target variable.
 *
 * @param model Pointer to the fitted LinearRegressionModel. Must not be NULL and
 *              should have been previously fitted using fit_linear_regression().
 * @param X Pointer to the DataFrame containing the feature data for prediction.
 *          Each column represents a feature, and each row represents an observation.
 *          Must not be NULL and should have the same number of features as the model.
 *
 * @return A pointer to a dynamically allocated array of float values containing
 *         the predicted target values. The array will have the same number of elements
 *         as rows in X. Returns NULL if an error occurs (e.g., invalid input parameters
 *         or memory allocation failure). The caller is responsible for freeing this memory.
 */
float* predict_linear_regression(LinearRegressionModel *model, DataFrame *X);

/**
 * @brief Calculates the R-squared (coefficient of determination) value for a regression model.
 *
 * This function computes the R-squared value, which measures the proportion of
 * variance in the dependent variable that is predictable from the independent variable(s).
 * R-squared ranges from 0 to 1, where 1 indicates perfect prediction.
 *
 * @param y_true Pointer to an array of float values representing the true target values.
 *               Must not be NULL and should contain at least 'n' elements.
 * @param y_pred Pointer to an array of float values representing the predicted target values.
 *               Must not be NULL and should contain at least 'n' elements.
 * @param n The number of elements in both y_true and y_pred arrays. Must be greater than 0.
 *
 * @return The calculated R-squared value as a float between 0 and 1.
 *         Returns -1.0 if an error occurs (e.g., invalid input parameters).
 */
float calculate_r_squared(float *y_true, float *y_pred, int n);

/**
 * @brief Calculates the Mean Squared Error (MSE) between true and predicted values.
 *
 * This function computes the Mean Squared Error, which is a measure of the average
 * squared difference between the predicted values and the true values. It is commonly
 * used to evaluate the performance of regression models.
 *
 * @param y_true Pointer to an array of float values representing the true target values.
 *               Must not be NULL and should contain at least 'n' elements.
 * @param y_pred Pointer to an array of float values representing the predicted target values.
 *               Must not be NULL and should contain at least 'n' elements.
 * @param n The number of elements in both y_true and y_pred arrays. Must be greater than 0.
 *
 * @return The calculated Mean Squared Error as a float value. A lower MSE indicates
 *         better model performance. Returns -1.0 if an error occurs (e.g., invalid input parameters).
 */
float calculate_mse(float *y_true, float *y_pred, int n);




// Error handling
extern char error_message[256];
/**
 * @brief Sets an error message using a formatted string.
 *
 * This function creates an error message using the provided format string and
 * any additional arguments. The resulting message is stored in a global error
 * message buffer for later retrieval.
 *
 * @param format A printf-style format string for the error message.
 * @param ... Additional arguments to be formatted according to the format string.
 *
 * @return This function does not return a value.
 *
 * @note The error message is stored in a global buffer and can be accessed
 *       or displayed as needed by other parts of the program.
 */
void set_error(const char *format, ...);

/**
 * @brief Logs debug messages with variable arguments.
 *
 * This function prints debug messages to a specified output stream (e.g., stderr)
 * using a printf-style format string and variable arguments. It's useful for
 * debugging and tracing program execution.
 *
 * @param format A printf-style format string for the debug message.
 * @param ... Additional arguments to be formatted according to the format string.
 *
 * @return This function does not return a value.
 *
 * @note The actual implementation may include additional features such as
 *       log levels, timestamps, or output redirection.
 */
void debug_log(const char *format, ...);

#endif

