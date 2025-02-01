#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <clenzdat.h>
void test_linear_regression1() {
    printf("\nTesting linear regression...\n");

    // Read the housing.csv file
    DataFrame* df = read_csv("large_sample.csv");
    if (df == NULL) {
        printf("Failed to read housing.csv file.\n");
        return;
    }
    

   
    float* y;
    DataFrame* X = split_dataframe(df, "result", (void**)&y);

    if (X == NULL || y == NULL) {
        printf("Failed to split DataFrame.\n");
        free_dataframe(df);
        return;
    }

    printf("X shape after split: (%d, %d)\n", X->num_rows, X->num_columns);

    // Check for NaN or inf values in X and y
    for (int i = 0; i < X->num_rows; i++) {
        for (int j = 0; j < X->num_columns; j++) {
            float value = ((float*)X->columns[j].data)[i];
            if (isnan(value) || isinf(value)) {
                printf("Warning: NaN or inf value found in X at row %d, column %d\n", i, j);
            }
        }
        if (isnan(y[i]) || isinf(y[i])) {
            printf("Warning: NaN or inf value found in y at index %d\n", i);
        }
    }

    // Create and fit the linear regression model
    LinearRegressionModel* model = create_linear_regression_model(X->num_columns);
    if (model == NULL) {
        printf("Failed to create linear regression model.\n");
        free_dataframe(df);
        free_dataframe(X);
        free(y);
        return;
    }

    printf("Fitting linear regression model...\n");
    clock_t start = clock();
    if (!fit_linear_regression(model, X, y)) {
        printf("Failed to fit linear regression model.\n");
        free_linear_regression_model(model);
        free_dataframe(df);
        free_dataframe(X);
        free(y);
        return;
    }
    clock_t end  = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Model fitted successfully.\n");
    printf("Time taken to fit the model: %.4f seconds\n", cpu_time_used);

    // Make predictions
    float* y_pred = predict_linear_regression(model, X);
    if (y_pred == NULL) {
        printf("Failed to make predictions.\n");
        free_linear_regression_model(model);
        free_dataframe(df);
        free_dataframe(X);
        free(y);
        return;
    }

    printf("Predictions made successfully.\n");

    // Calculate and print accuracy metrics
    float r_squared = calculate_r_squared(y, y_pred, X->num_rows);
    float mse = calculate_mse(y, y_pred, X->num_rows);

    printf("Linear Regression Results:\n");
    printf("R-squared: %.4f\n", r_squared);
    printf("Mean Squared Error: %.4f\n", mse);

    // Print coefficients and intercept
    printf("Coefficients:\n");
    for (int i = 0; i < X->num_columns; i++) {
        printf("%s: %.4f\n", X->column_names[i], model->coefficients[i]);
    }
    printf("Intercept: %.4f\n", model->intercept);    

    // Clean up
    free_linear_regression_model(model);
    free_dataframe(df);
    free_dataframe(X);
    free(y);
    free(y_pred);
    printf("Linear regression test completed.\n");
    scanf("%d");

}

int main() {
    test_linear_regression1();
    return 0;
}