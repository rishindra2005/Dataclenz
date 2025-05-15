#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "clenzdat.h"

// Structure to store performance metrics for real-world data
typedef struct {
    char search_type[20];
    char dataset_name[50];
    char column_name[50];
    int dataset_size;
    int num_searches;
    double avg_search_time;
    int avg_comparisons;
    int successful_searches;
} RealWorldMetrics;

// Function to measure binary search performance on real data
RealWorldMetrics measure_binary_search_real_data(DataFrame* df, int column_index, const char* column_name, 
                                               int num_searches, const char* dataset_name) {
    RealWorldMetrics metrics;
    strncpy(metrics.search_type, "Binary Search", sizeof(metrics.search_type) - 1);
    strncpy(metrics.dataset_name, dataset_name, sizeof(metrics.dataset_name) - 1);
    strncpy(metrics.column_name, column_name, sizeof(metrics.column_name) - 1);
    metrics.dataset_size = df->num_rows;
    metrics.num_searches = num_searches;
    metrics.avg_search_time = 0.0;
    metrics.avg_comparisons = 0;
    metrics.successful_searches = 0;

    // Replace all NULL strings with empty strings
    if (df->columns[column_index].type == TYPE_STRING) {
        for (int i = 0; i < df->num_rows; i++) {
            if (((char**)df->columns[column_index].data)[i] == NULL) {
                ((char**)df->columns[column_index].data)[i] = strdup("");
            }
        }
    }

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
        void* target = NULL;
        
        // Allocate memory for the target based on data type
        switch (df->columns[column_index].type) {
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
                if (str == NULL) {
                    // If str is NULL, use empty string
                    char* string_target = strdup("");
                    target = string_target;
                } else if (strlen(str) == 0) {
                    // If empty string, use a copy
                    char* string_target = strdup("");
                    target = string_target;
                } else {
                    char* string_target = strdup(str);
                    target = string_target;
                }
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

// Function to measure jump search performance on real data
RealWorldMetrics measure_jump_search_real_data(DataFrame* df, int column_index, const char* column_name, 
                                             int num_searches, const char* dataset_name) {
    RealWorldMetrics metrics;
    strncpy(metrics.search_type, "Jump Search", sizeof(metrics.search_type) - 1);
    strncpy(metrics.dataset_name, dataset_name, sizeof(metrics.dataset_name) - 1);
    strncpy(metrics.column_name, column_name, sizeof(metrics.column_name) - 1);
    metrics.dataset_size = df->num_rows;
    metrics.num_searches = num_searches;
    metrics.avg_search_time = 0.0;
    metrics.avg_comparisons = 0;
    metrics.successful_searches = 0;

    // Replace all NULL strings with empty strings
    if (df->columns[column_index].type == TYPE_STRING) {
        for (int i = 0; i < df->num_rows; i++) {
            if (((char**)df->columns[column_index].data)[i] == NULL) {
                ((char**)df->columns[column_index].data)[i] = strdup("");
            }
        }
    }

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
        void* target = NULL;
        
        // Allocate memory for the target based on data type
        switch (df->columns[column_index].type) {
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
                if (str == NULL) {
                    // If str is NULL, use empty string
                    char* string_target = strdup("");
                    target = string_target;
                } else if (strlen(str) == 0) {
                    // If empty string, use a copy
                    char* string_target = strdup("");
                    target = string_target;
                } else {
                    char* string_target = strdup(str);
                    target = string_target;
                }
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

// Function to save real-world performance metrics to a CSV file
void save_real_world_metrics_to_csv(RealWorldMetrics* metrics_array, int metrics_count, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Failed to open file %s for writing.\n", filename);
        return;
    }

    // Write header
    fprintf(file, "Search Type,Dataset Name,Column Name,Dataset Size,Number of Searches,Average Search Time (s),Average Comparisons,Successful Searches\n");

    // Write data
    for (int i = 0; i < metrics_count; i++) {
        fprintf(file, "%s,%s,%s,%d,%d,%f,%d,%d\n",
                metrics_array[i].search_type,
                metrics_array[i].dataset_name,
                metrics_array[i].column_name,
                metrics_array[i].dataset_size,
                metrics_array[i].num_searches,
                metrics_array[i].avg_search_time,
                metrics_array[i].avg_comparisons,
                metrics_array[i].successful_searches);
    }

    fclose(file);
    printf("Real-world performance metrics saved to %s\n", filename);
}

// Function to preprocess DataFrame by replacing NULL strings with empty strings
void preprocess_dataframe(DataFrame* df) {
    if (df == NULL) {
        return;
    }
    
    // For each string column, replace NULL values with empty strings
    for (int col = 0; col < df->num_columns; col++) {
        if (df->columns[col].type == TYPE_STRING) {
            for (int row = 0; row < df->num_rows; row++) {
                char** string_data = (char**)df->columns[col].data;
                if (string_data[row] == NULL) {
                    string_data[row] = strdup("");
                    if (string_data[row] == NULL) {
                        printf("Warning: Failed to allocate memory for empty string\n");
                    }
                }
            }
        }
    }
}

int main() {
    // Seed the random number generator
    srand((unsigned int)time(NULL));

    printf("===========================================\n");
    printf("Real-World Data Search Performance Analysis\n");
    printf("===========================================\n\n");

    // Number of searches to perform for each dataset
    int num_searches = 1000;

    // Array to store metrics
    int max_metrics = 20;  // Adjust as needed
    RealWorldMetrics* all_metrics = (RealWorldMetrics*)malloc(max_metrics * sizeof(RealWorldMetrics));
    
    if (all_metrics == NULL) {
        printf("Memory allocation failed for metrics array.\n");
        return 1;
    }

    int metrics_index = 0;

    // Test with MoMA artists dataset
    printf("Loading MoMA artists dataset...\n");
    DataFrame* moma_df = read_csv("Datasets/MoMA_OnView.csv");
    
    if (moma_df != NULL) {
        printf("MoMA dataset loaded: %d rows, %d columns\n", moma_df->num_rows, moma_df->num_columns);
        
        // Preprocess the dataset to handle NULL strings
        printf("Preprocessing MoMA dataset to handle NULL values...\n");
        preprocess_dataframe(moma_df);
        
        // Find the column indices we want to search
        int name_col = -1;
        int nationality_col = -1;
        int gender_col = -1;
        
        for (int i = 0; i < moma_df->num_columns; i++) {
            if (strcmp(moma_df->column_names[i], "DisplayName") == 0) {
                name_col = i;
            } else if (strcmp(moma_df->column_names[i], "Nationality") == 0) {
                nationality_col = i;
            } else if (strcmp(moma_df->column_names[i], "Gender") == 0) {
                gender_col = i;
            }
        }
        
        // Test searching by artist name
        if (name_col >= 0) {
            printf("\nTesting search by artist name...\n");
            all_metrics[metrics_index++] = measure_binary_search_real_data(
                moma_df, name_col, "DisplayName", num_searches, "MoMA_Artists");
            all_metrics[metrics_index++] = measure_jump_search_real_data(
                moma_df, name_col, "DisplayName", num_searches, "MoMA_Artists");
        }
        
        // Test searching by nationality
        if (nationality_col >= 0) {
            printf("\nTesting search by nationality...\n");
            all_metrics[metrics_index++] = measure_binary_search_real_data(
                moma_df, nationality_col, "Nationality", num_searches, "MoMA_Artists");
            all_metrics[metrics_index++] = measure_jump_search_real_data(
                moma_df, nationality_col, "Nationality", num_searches, "MoMA_Artists");
        }
        
        // Test searching by gender
        if (gender_col >= 0) {
            printf("\nTesting search by gender...\n");
            all_metrics[metrics_index++] = measure_binary_search_real_data(
                moma_df, gender_col, "Gender", num_searches, "MoMA_Artists");
            all_metrics[metrics_index++] = measure_jump_search_real_data(
                moma_df, gender_col, "Gender", num_searches, "MoMA_Artists");
        }
        
        // Don't free moma_df to avoid memory management issues
    } else {
        printf("Failed to load MoMA dataset.\n");
    }

    // Test with housing dataset if available
    printf("\nLoading housing dataset...\n");
    DataFrame* housing_df = read_csv("housing.csv");
    
    if (housing_df != NULL) {
        printf("Housing dataset loaded: %d rows, %d columns\n", housing_df->num_rows, housing_df->num_columns);
        
        // Preprocess the dataset to handle NULL strings
        printf("Preprocessing housing dataset to handle NULL values...\n");
        preprocess_dataframe(housing_df);
        
        // Test with numeric columns - median_house_value and median_income
        int house_value_col = -1;
        int income_col = -1;
        
        for (int i = 0; i < housing_df->num_columns; i++) {
            if (strcmp(housing_df->column_names[i], "median_house_value") == 0) {
                house_value_col = i;
            } else if (strcmp(housing_df->column_names[i], "median_income") == 0) {
                income_col = i;
            }
        }
        
        if (house_value_col >= 0) {
            printf("\nTesting search by median house value...\n");
            all_metrics[metrics_index++] = measure_binary_search_real_data(
                housing_df, house_value_col, "median_house_value", num_searches, "Housing");
            all_metrics[metrics_index++] = measure_jump_search_real_data(
                housing_df, house_value_col, "median_house_value", num_searches, "Housing");
        }
        
        if (income_col >= 0) {
            printf("\nTesting search by median income...\n");
            all_metrics[metrics_index++] = measure_binary_search_real_data(
                housing_df, income_col, "median_income", num_searches, "Housing");
            all_metrics[metrics_index++] = measure_jump_search_real_data(
                housing_df, income_col, "median_income", num_searches, "Housing");
        }
        
        // Don't free housing_df to avoid memory management issues
    } else {
        printf("Failed to load housing dataset. Continuing with other tests.\n");
    }

    // Save results to CSV
    if (metrics_index > 0) {
        save_real_world_metrics_to_csv(all_metrics, metrics_index, "Dataclenz/src/real_world_search_results.csv");
        printf("\nSaved results to real_world_search_results.csv\n");
    } else {
        printf("\nNo metrics collected. Please check the datasets and search parameters.\n");
    }

    // Don't free all_metrics to avoid memory management issues
    printf("\nReal-world analysis complete!\n");
    
    return 0;
} 