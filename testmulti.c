#include "clenzdat.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to test read_csv and measure its performance
double test_read_csv(const char* filename) {
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    DataFrame* df = read_csv(filename);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to read CSV: %f seconds\n", cpu_time_used);

    if (df == NULL) {
        printf("Error: Failed to read CSV file g.\n");
        return -1.0;
    }

    printf("DataFrame successfully created.\n");
    printf("Number of rows: %d\n", df->num_rows);
    printf("Number of columns: %d\n", df->num_columns);

    // Print full DataFrame to a file
    print_dataframe_s(df, "output_sample.txt");
    printf("Sample output written to output_sample.txt\n");

    // Free the DataFrame
    free_dataframe(df);

    return cpu_time_used;
}

int main() {
    const char* filename = "large_matrix.csv";  // Replace with your actual CSV file name
    
    printf("Testing multithreaded read_csv function...\n");
    double time_taken = test_read_csv(filename);
    
    if (time_taken >= 0) {
        printf("Time taken to read CSV: %f seconds\n", time_taken);
    }

    return 0;
}