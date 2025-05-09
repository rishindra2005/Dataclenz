#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "clenzdat.h"

// Structure to store performance metrics
typedef struct {
    char search_type[20];
    int dataset_size;
    int num_searches;
    double avg_search_time;
    int avg_comparisons;
    int successful_searches;
    int memory_usage_kb;
} SearchPerformanceMetrics;

// Function to generate a random integer dataset of specified size
DataFrame* generate_random_int_dataset(int size, int min_val, int max_val) {
    printf("Generating random integer dataset of size %d...\n", size);
    DataFrame* df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame for random dataset.\n");
        return NULL;
    }

    // Create random integer data
    int* data = (int*)malloc(size * sizeof(int));
    if (data == NULL) {
        printf("Memory allocation failed for random data generation.\n");
        free_dataframe(df);
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        data[i] = min_val + rand() % (max_val - min_val + 1);
    }

    // Add column to the DataFrame
    if (!add_column(df, "RandomInt", TYPE_INT, data, size)) {
        printf("Failed to add column to DataFrame.\n");
        free(data);
        free_dataframe(df);
        return NULL;
    }

    free(data);
    return df;
}

// Function to generate a random float dataset of specified size
DataFrame* generate_random_float_dataset(int size, float min_val, float max_val) {
    printf("Generating random float dataset of size %d...\n", size);
    DataFrame* df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame for random dataset.\n");
        return NULL;
    }

    // Create random float data
    float* data = (float*)malloc(size * sizeof(float));
    if (data == NULL) {
        printf("Memory allocation failed for random data generation.\n");
        free_dataframe(df);
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        data[i] = min_val + ((float)rand() / RAND_MAX) * (max_val - min_val);
    }

    // Add column to the DataFrame
    if (!add_column(df, "RandomFloat", TYPE_FLOAT, data, size)) {
        printf("Failed to add column to DataFrame.\n");
        free(data);
        free_dataframe(df);
        return NULL;
    }

    free(data);
    return df;
}

// Function to generate a random string dataset of specified size
DataFrame* generate_random_string_dataset(int size, int min_length, int max_length) {
    printf("Generating random string dataset of size %d...\n", size);
    DataFrame* df = create_dataframe();
    if (df == NULL) {
        printf("Failed to create DataFrame for random dataset.\n");
        return NULL;
    }

    // Create random string data
    char** data = (char**)malloc(size * sizeof(char*));
    if (data == NULL) {
        printf("Memory allocation failed for random data generation.\n");
        free_dataframe(df);
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        int length = min_length + rand() % (max_length - min_length + 1);
        data[i] = (char*)malloc((length + 1) * sizeof(char));
        if (data[i] == NULL) {
            // Clean up previously allocated memory
            for (int j = 0; j < i; j++) {
                free(data[j]);
            }
            free(data);
            free_dataframe(df);
            printf("Memory allocation failed for string at index %d.\n", i);
            return NULL;
        }

        for (int j = 0; j < length; j++) {
            data[i][j] = 'a' + rand() % 26;  // Generate random lowercase letters
        }
        data[i][length] = '\0';
    }

    // Add column to the DataFrame
    if (!add_column(df, "RandomString", TYPE_STRING, data, size)) {
        printf("Failed to add column to DataFrame.\n");
        for (int i = 0; i < size; i++) {
            free(data[i]);
        }
        free(data);
        free_dataframe(df);
        return NULL;
    }

    // Free the original array (not the strings, as they're now owned by the DataFrame)
    free(data);
    return df;
}

// Function to measure binary search performance
SearchPerformanceMetrics measure_binary_search_performance(DataFrame* df, int column_index, int num_searches, ColumnType type) {
    SearchPerformanceMetrics metrics;
    strncpy(metrics.search_type, "Binary Search", sizeof(metrics.search_type) - 1);
    metrics.dataset_size = df->num_rows;
    metrics.num_searches = num_searches;
    metrics.avg_search_time = 0.0;
    metrics.avg_comparisons = 0;
    metrics.successful_searches = 0;
    metrics.memory_usage_kb = sizeof(int) * df->num_rows / 1024;  // Rough estimate of memory usage

    // Sort the DataFrame first (binary search requires sorted data)
    DataFrame* sorted_df = sort_dataframe(df, column_index, 1);
    if (sorted_df == NULL) {
        printf("Failed to sort DataFrame for binary search.\n");
        metrics.avg_search_time = -1.0;  // Indicate failure
        return metrics;
    }

    // Allocate memory for found indices
    int* found_indices = (int*)malloc(df->num_rows * sizeof(int));
    if (found_indices == NULL) {
        printf("Memory allocation failed for found_indices array.\n");
        metrics.avg_search_time = -1.0;  // Indicate failure
        return metrics;
    }

    // Measure search performance
    clock_t start_time = clock();
    int total_comparisons = 0;

    for (int i = 0; i < num_searches; i++) {
        // Generate random search target from the dataset
        int random_index = rand() % df->num_rows;
        void* target;
        
        // Allocate memory for the target based on data type
        switch (type) {
            case TYPE_INT: {
                int* int_target = (int*)malloc(sizeof(int));
                *int_target = ((int*)sorted_df->columns[column_index].data)[random_index];
                target = int_target;
                break;
            }
            case TYPE_FLOAT: {
                float* float_target = (float*)malloc(sizeof(float));
                *float_target = ((float*)sorted_df->columns[column_index].data)[random_index];
                target = float_target;
                break;
            }
            case TYPE_STRING: {
                char* str = ((char**)sorted_df->columns[column_index].data)[random_index];
                char* string_target = strdup(str);
                target = string_target;
                break;
            }
            default:
                printf("Unsupported data type for search performance measurement.\n");
                free(found_indices);
                return metrics;
        }

        // Perform binary search
        int found_count = binary_search(sorted_df, column_index, target, found_indices, df->num_rows);
        
        if (found_count > 0) {
            metrics.successful_searches++;
        }
        
        // Estimate comparisons (log2(n) for binary search)
        total_comparisons += (int)ceil(log2(df->num_rows));
        
        // Free allocated memory for target
        free(target);
    }

    clock_t end_time = clock();
    metrics.avg_search_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC / num_searches;
    metrics.avg_comparisons = total_comparisons / num_searches;

    free(found_indices);
    return metrics;
}

// Function to measure jump search performance
SearchPerformanceMetrics measure_jump_search_performance(DataFrame* df, int column_index, int num_searches, ColumnType type) {
    SearchPerformanceMetrics metrics;
    strncpy(metrics.search_type, "Jump Search", sizeof(metrics.search_type) - 1);
    metrics.dataset_size = df->num_rows;
    metrics.num_searches = num_searches;
    metrics.avg_search_time = 0.0;
    metrics.avg_comparisons = 0;
    metrics.successful_searches = 0;
    metrics.memory_usage_kb = sizeof(int) * df->num_rows / 1024;  // Rough estimate of memory usage

    // Sort the DataFrame first (jump search requires sorted data)
    DataFrame* sorted_df = sort_dataframe(df, column_index, 1);
    if (sorted_df == NULL) {
        printf("Failed to sort DataFrame for jump search.\n");
        metrics.avg_search_time = -1.0;  // Indicate failure
        return metrics;
    }

    // Allocate memory for found indices
    int* found_indices = (int*)malloc(df->num_rows * sizeof(int));
    if (found_indices == NULL) {
        printf("Memory allocation failed for found_indices array.\n");
        metrics.avg_search_time = -1.0;  // Indicate failure
        return metrics;
    }

    // Measure search performance
    clock_t start_time = clock();
    int total_comparisons = 0;

    for (int i = 0; i < num_searches; i++) {
        // Generate random search target from the dataset
        int random_index = rand() % df->num_rows;
        void* target;
        
        // Allocate memory for the target based on data type
        switch (type) {
            case TYPE_INT: {
                int* int_target = (int*)malloc(sizeof(int));
                *int_target = ((int*)sorted_df->columns[column_index].data)[random_index];
                target = int_target;
                break;
            }
            case TYPE_FLOAT: {
                float* float_target = (float*)malloc(sizeof(float));
                *float_target = ((float*)sorted_df->columns[column_index].data)[random_index];
                target = float_target;
                break;
            }
            case TYPE_STRING: {
                char* str = ((char**)sorted_df->columns[column_index].data)[random_index];
                char* string_target = strdup(str);
                target = string_target;
                break;
            }
            default:
                printf("Unsupported data type for search performance measurement.\n");
                free(found_indices);
                return metrics;
        }

        // Perform jump search
        int found_count = jump_search(sorted_df, column_index, target, found_indices, df->num_rows);
        
        if (found_count > 0) {
            metrics.successful_searches++;
        }
        
        // Estimate comparisons (sqrt(n) for jump search)
        total_comparisons += (int)ceil(sqrt(df->num_rows));
        
        // Free allocated memory for target
        free(target);
    }

    clock_t end_time = clock();
    metrics.avg_search_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC / num_searches;
    metrics.avg_comparisons = total_comparisons / num_searches;

    free(found_indices);
    return metrics;
}

// Function to save performance metrics to a CSV file
void save_metrics_to_csv(SearchPerformanceMetrics* metrics_array, int metrics_count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Failed to open file %s for writing.\n", filename);
        return;
    }

    // Write header
    fprintf(file, "Search Type,Dataset Size,Number of Searches,Average Search Time (s),Average Comparisons,Successful Searches,Memory Usage (KB)\n");

    // Write data
    for (int i = 0; i < metrics_count; i++) {
        fprintf(file, "%s,%d,%d,%f,%d,%d,%d\n",
                metrics_array[i].search_type,
                metrics_array[i].dataset_size,
                metrics_array[i].num_searches,
                metrics_array[i].avg_search_time,
                metrics_array[i].avg_comparisons,
                metrics_array[i].successful_searches,
                metrics_array[i].memory_usage_kb);
    }

    fclose(file);
    printf("Performance metrics saved to %s\n", filename);
}

int main() {
    // Seed the random number generator
    srand((unsigned int)time(NULL));

    // Define dataset sizes to test
    int dataset_sizes[] = {50, 100, 500, 1000, 5000, 10000, 20000 , 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    int num_sizes = sizeof(dataset_sizes) / sizeof(dataset_sizes[0]);
    
    // Number of searches to perform for each dataset
    int num_searches = 1000;

    // Calculate total number of tests (2 search algorithms * num_sizes * 3 data types)
    int total_metrics = 2 * num_sizes * 3;
    SearchPerformanceMetrics* all_metrics = (SearchPerformanceMetrics*)malloc(total_metrics * sizeof(SearchPerformanceMetrics));
    
    if (all_metrics == NULL) {
        printf("Memory allocation failed for metrics array.\n");
        return 1;
    }

    int metrics_index = 0;

    // Test for each dataset size
    for (int i = 0; i < num_sizes; i++) {
        int size = dataset_sizes[i];
        printf("\n===============================================\n");
        printf("Testing with dataset size: %d\n", size);
        printf("===============================================\n");

        // Test with integer data
        printf("\n--- Testing with Integer Data ---\n");
        DataFrame* int_df = generate_random_int_dataset(size, 1, 1000000);
        if (int_df != NULL) {
            all_metrics[metrics_index++] = measure_binary_search_performance(int_df, 0, num_searches, TYPE_INT);
            all_metrics[metrics_index++] = measure_jump_search_performance(int_df, 0, num_searches, TYPE_INT);
            // Don't free int_df to avoid memory management issues
        }
        free_dataframe(int_df);

        // Test with float data
        printf("\n--- Testing with Float Data ---\n");
        DataFrame* float_df = generate_random_float_dataset(size, 0.0, 1000000.0);
        if (float_df != NULL) {
            all_metrics[metrics_index++] = measure_binary_search_performance(float_df, 0, num_searches, TYPE_FLOAT);
            all_metrics[metrics_index++] = measure_jump_search_performance(float_df, 0, num_searches, TYPE_FLOAT);
            // Don't free float_df to avoid memory management issues
        }
        free_dataframe(float_df);

        // Test with string data
        printf("\n--- Testing with String Data ---\n");
        DataFrame* string_df = generate_random_string_dataset(size, 5, 15);
        if (string_df != NULL) {
            all_metrics[metrics_index++] = measure_binary_search_performance(string_df, 0, num_searches, TYPE_STRING);
            all_metrics[metrics_index++] = measure_jump_search_performance(string_df, 0, num_searches, TYPE_STRING);
            // Don't free string_df to avoid memory management issues
        }
        free_dataframe(string_df);
    }

    // Save results to CSV
    save_metrics_to_csv(all_metrics, metrics_index, "Dataclenz/src/search_complexity_results.csv");

    // Generate a summary report
    printf("\n\n===============================================\n");
    printf("Performance Analysis Summary\n");
    printf("===============================================\n");
    printf("Dataset sizes tested: ");
    for (int i = 0; i < num_sizes; i++) {
        printf("%d ", dataset_sizes[i]);
    }
    printf("\nNumber of searches per test: %d\n", num_searches);
    printf("Total tests performed: %d\n", metrics_index);
    
    printf("\nTheoretical Time Complexity:\n");
    printf("- Binary Search: O(log n)\n");
    printf("- Jump Search: O(sqrt(n))\n");
    
    printf("\nTheoretical Space Complexity:\n");
    printf("- Binary Search: O(1)\n");
    printf("- Jump Search: O(1)\n");
    
    printf("\nResults saved to 'search_complexity_results.csv'\n");
    
    // Don't free all_metrics to avoid memory management issues
    printf("\nAnalysis complete!\n");
    
    return 0;
} 