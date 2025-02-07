#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "clenzdat.h"
#include <windows.h>
#include <psapi.h>


#define MAX_FILENAME_LENGTH 256
#define MAX_COLUMN_NAME_LENGTH 100
SIZE_T get_memory_usage() {
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return pmc.WorkingSetSize;
}
int main(int argc, char *argv[]) {
    clock_t total_start, total_end;
    SIZE_T initial_memory, final_memory;

    total_start = clock();
    initial_memory = get_memory_usage();

    if (argc != 4) {
        printf("Usage: %s <csv_file_path> <delimiter> <target_column_name>\n", argv[0]);
        return 1;
    }

    char *file_path = argv[1];
    char delimiter = argv[2][0];
    char *target_column = argv[3];

    clock_t start, end;
    double cpu_time_used;

    // Step 1: Read the CSV file
    printf("Reading CSV file: %s\n", file_path);
    start = clock();

    DataFrame* df = read_csv_d(file_path, delimiter);
    if (df == NULL) {
        printf("Failed to read CSV file. Error: %s\n", get_error());
        return 1;
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to read CSV: %.4f seconds\n", cpu_time_used);

    // Step 2: Preprocess data
    printf("\nPreprocessing data...\n");
    start = clock();
    
        // Remove non-numeric columns (if any)
    for (int i = df->num_columns - 1; i >= 0; i--) {
        if (df->columns[i].type == 2) {  
            // printf("Deleting column %d (type: %d)\n", i, df->columns[i].type);
            if (delete_column(df, i) == 0) {
                // printf("Failed to delete column %d. Error: %s\n", i, get_error());
                free_dataframe(df);
                return 1;
            }
            printf("Deleted column %d\n", i);
        }
        // Break the loop if all columns have been deleted
        if (df->num_columns == 0) {
            printf("All columns have been deleted.\n");
            break;
        }
    }

    // Check if there are any columns left before proceeding
    if (df->num_columns == 0) {
        // printf("No numeric columns remain in the DataFrame.\n");
        free_dataframe(df);
        return 1;
    }

    // Handle missing values and remove null rows
    DataFrame* cleaned_df = df;
    for (int i = 0; i < df->num_columns; i++) {
        DataFrame* temp_df = handle_missing_values(cleaned_df, i, "remove");
        if (temp_df == NULL) {
            printf("Failed to handle missing values for column %d. Error: %s\n", i, get_error());
            free_dataframe(cleaned_df);
            return 1;
        }
        if (cleaned_df != df) {
            free_dataframe(cleaned_df);
        }
        cleaned_df = temp_df;
    }
    
    printf("Number of columns after handling missing values: %d\n", cleaned_df->num_columns);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken for preprocessing: %.4f seconds\n", cpu_time_used);

    // Step 3: Normalize data (except the last column, assuming it's the target variable)
    printf("\nNormalizing data...\n");
    start = clock();
    DataFrame* normalized_df = cleaned_df;
    for (int i = 0; i < cleaned_df->num_columns - 1; i++) {
        DataFrame* temp_df = normalize_column(normalized_df, i);
        if (temp_df == NULL) {
            printf("Failed to normalize column %d. Error: %s\n", i, get_error());
            free_dataframe(normalized_df);
            return 1;
        }
        if (normalized_df != cleaned_df) {
            free_dataframe(normalized_df);
        }
        normalized_df = temp_df;
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken for normalization: %.4f seconds\n", cpu_time_used);

    // Step 4: Split the DataFrame
    float* y;
    printf("target variable: %s\n", target_column);
    DataFrame* X = split_dataframe(normalized_df, target_column, (void**)&y);
    if (X == NULL || y == NULL) {
        printf("Failed to split DataFrame. Error: %s\n", get_error());
        free_dataframe(normalized_df);
        return 1;
    }

    // Step 5: Create and fit the linear regression model
    printf("\nFitting linear regression model...\n");
    start = clock();
    LinearRegressionModel* model = create_linear_regression_model(X->num_columns);
    if (model == NULL) {
        printf("Failed to create linear regression model. Error: %s\n", get_error());
        free_dataframe(normalized_df);
        free_dataframe(X);
        free(y);
        return 1;
    }

    if (!fit_linear_regression(model, X, y)) {
        printf("Failed to fit linear regression model. Error: %s\n", get_error());
        free_linear_regression_model(model);
        free_dataframe(normalized_df);
        free_dataframe(X);
        free(y);
        return 1;
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to fit the model: %.4f seconds\n", cpu_time_used);

    // Step 6: Make predictions and calculate metrics
    printf("\nMaking predictions...\n");
    start = clock();
    float* y_pred = predict_linear_regression(model, X);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken for predictions: %.4f seconds\n", cpu_time_used);

    if (y_pred == NULL) {
        printf("Failed to make predictions. Error: %s\n", get_error());
        free_linear_regression_model(model);
        free_dataframe(normalized_df);
        free_dataframe(X);
        free(y);
        return 1;
    }

    float r_squared = calculate_r_squared(y, y_pred, X->num_rows);
    float mse = calculate_mse(y, y_pred, X->num_rows);

    // Print results
    printf("\nLinear Regression Results:\n");
    printf("R-squared: %.4f\n", r_squared);
    printf("Mean Squared Error: %.4f\n", mse);

    total_end = clock();
    final_memory = get_memory_usage();

    double total_time = ((double)(total_end - total_start)) / CLOCKS_PER_SEC;
    double memory_used_mb = (final_memory - initial_memory) / (1024.0 * 1024.0);

    printf("\nTotal execution time: %.4f seconds\n", total_time);
    printf("Total memory used: %.2f MB\n", memory_used_mb);

    printf("\nPress Enter to continue...");
    getchar(); 

    // Clean up
    free_linear_regression_model(model);
    free_dataframe(normalized_df);
    free_dataframe(X);
    free(y);
    free(y_pred);

    printf("\nLinear regression test completed.\n");
    return 0;
}
