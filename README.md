# Dataclenz Library

Dataclenz is a C library for data manipulation and analysis, providing functionality similar to pandas in Python. It offers a range of tools for working with structured data, including data preprocessing, statistical analysis, and machine learning capabilities.

## Key Features

- DataFrame operations (creation, manipulation, analysis)
- Data preprocessing (handling missing values, normalization, scaling)
- Statistical functions (mean, median, mode, variance, standard deviation)
- Basic machine learning (linear regression)
- CSV file reading and writing
- Data visualization (printing DataFrames)

## Type Definitions

The library uses several custom type definitions to represent data structures and models:

1. `DataFrame`: The main data structure for holding tabular data.
   - Contains information about columns, rows, and the actual data.

2. `Column`: Represents a single column in a DataFrame.
   - Includes the column name, data type, and a pointer to the data.

3. `ColumnType`: An enumeration representing the possible data types for columns.
   - `TYPE_INT`: Integer type
   - `TYPE_FLOAT`: Float type
   - `TYPE_STRING`: String type

4. `LinearRegressionModel`: Represents a linear regression model.
   - Contains coefficients, intercept, and the number of features.

## Function Overview

### DataFrame Operations
- `create_dataframe`: Create a new DataFrame
- `add_column`: Add a new column to a DataFrame
- `delete_column`: Remove a column from a DataFrame
- `print_dataframe`: Display the contents of a DataFrame
- `print_dataframe_s`: Print DataFrame to a file or console
- `shape_df`: Get the dimensions of a DataFrame
- `get_dataframe_range`: Extract a range of rows from a DataFrame
- `sort_dataframe`: Sort a DataFrame based on a specified column
- `resize_dataframe`: Adjust the number of rows in a DataFrame
- `split_dataframe`: Split a DataFrame into features and target for machine learning

### Data Preprocessing
- `handle_missing_values`: Handle missing values in a DataFrame
- `normalize_column`: Normalize values in a column
- `scale_column`: Scale values in a column

### Statistical Analysis
- `describe_dataframe`: Generate statistical summary of a DataFrame
- `calculate_mean`: Calculate the arithmetic mean of an array
- `calculate_median`: Calculate the median of an array
- `calculate_mode`: Calculate the mode of an array
- `calculate_variance`: Calculate the variance of an array
- `calculate_std_deviation`: Calculate the standard deviation
- `calculate_quartile`: Calculate quartiles (including percentiles)

### Machine Learning
- `create_linear_regression_model`: Create a new linear regression model
- `fit_linear_regression`: Fit a linear regression model to data
- `predict_linear_regression`: Make predictions using a fitted linear regression model
- `calculate_r_squared`: Calculate the R-squared value for regression results
- `calculate_mse`: Calculate the Mean Squared Error

### File I/O
- `read_csv`: Read data from a CSV file into a DataFrame
- `write_csv`: Write DataFrame data to a CSV file

### Utility Functions
- `get_column_as_array`: Retrieve a column from a DataFrame as a contiguous array
- `get_column_element_size`: Get the size in bytes of a column element

### Error Handling
- `set_error`: Set an error message
- `debug_log`: Log debug messages


## Usage

To use the Dataclenz library, include the `clenzdat.h` header file in your C program and link against the compiled library. The library provides a wide range of functions for data manipulation and analysis, allowing you to work with structured data efficiently in C.

For detailed function descriptions and usage examples, refer to the comments in the `clenzdat.h` header file.

## Error Handling

The library includes error handling mechanisms:
- `set_error`: Sets an error message that can be accessed globally.
- `debug_log`: Logs debug messages for tracing and debugging purposes.

## Contributing

Contributions to the Dataclenz library are welcome. Please ensure that you follow the existing code style and include appropriate documentation for any new features or changes.

## License

Copyright © 2025 Risheendra

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

This project is developed by Risheendra.

## Citations

If you use Dataclenz in your research or project, please cite it as follows:

### APA Format
Risheendra. (2025). Dataclenz: A C library for data manipulation and analysis. GitHub. https://github.com/rishindra2005/Dataclenz

### BibTeX
```bibtex
@software{dataclenz,
  author = {Risheendra},
  title = {Dataclenz: A C library for data manipulation and analysis},
  year = {2025},
  publisher = {GitHub},
  journal = {GitHub repository},
  url = {https://github.com/github.com/rishindra2005/Dataclenz}
}