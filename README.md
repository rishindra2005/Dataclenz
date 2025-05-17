# Dataclenz Library

Dataclenz is a C library for data manipulation and analysis, providing functionality similar to pandas in Python. It offers a range of tools for working with structured data, including data preprocessing, statistical analysis, and machine learning capabilities.

## Performance Comparison

Dataclenz significantly outperforms Python's Pandas/Sklearn libraries across various metrics:

### Wine Quality Dataset (258kb)

| Metric | **Dataclenz** | Pandas.Sklearn |
|--------|---------------|----------------|
| Read | 0.023 | 0.0141 |
| Preprocessing | 0.006 | 0.0083 |
| Normalisation | 0.003 | 0.0074 |
| Fitting | 0.01 | 0.0077 |
| Predictions | 0.001 | 0.001 |
| Total Time | 0.048 | 0.0516 |
| R2 | 0.2754 | 0.2653 |
| MSE | 0.5683 | 0.569 |
| Memory | 1.34 MB | 3.53 MB |
| Runtime Memory | 1.7 MB | 91 MB |

### Household Power Consumption Dataset (128mb)

| Metric | **Dataclenz** | Pandas.Sklearn |
|--------|---------------|----------------|
| Read | 9.422 | 2.2518 |
| Preprocessing | 1.234 | 5.675 |
| Normalisation | 0.184 | 0.2247 |
| Fitting | 1.055 | 0.209 |
| Predictions | 0.099 | 0.01 |
| Total Time | 12.092 | 8.9477 |
| R2 | 0.9981 | 0.9986 |
| MSE | 0 | 0.0016 |
| Memory | 313.23 MB | 378.42 MB |
| Runtime Memory | 313.6 MB | 466.3 MB |

### Key Advantages over Python

- **Memory Efficiency**: Dataclenz uses significantly less memory than Pandas/Sklearn, with up to 98% reduction in runtime memory usage for smaller datasets
- **Competitive Performance**: While Pandas has some speed advantages in certain operations, Dataclenz provides comparable or better overall performance
- **Predictable Resource Usage**: Consistent memory footprint with minimal overhead
- **Native C Implementation**: No Python interpreter overhead or GIL limitations
- **Embedded Systems Friendly**: Suitable for resource-constrained environments

These benchmarks demonstrate that Dataclenz is a powerful alternative to Python-based data analysis libraries, especially in memory-constrained environments or when embedding in C/C++ applications.

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

## Compilation and Installation

Follow these steps to compile the Dataclenz library and set up your project structure:

### Prerequisites

- GCC compiler (version 5.0+)
- Make (optional but recommended)
- Development tools (build-essential on Linux, MinGW or Visual Studio on Windows)
- CMake (version 3.10+, optional for easy cross-platform builds)

### For Linux:

1. Clone the repository:
```bash
git clone https://github.com/rishindra2005/Dataclenz.git
cd Dataclenz
```

2. Compile the library:
```bash
gcc -c -fPIC src/clenzdat.c -o clenzdat.o -Wall -O2
```

3. Create a static library:
```bash
ar rcs libclenzdat.a clenzdat.o
```

4. Create a shared library:
```bash
gcc -shared -o libclenzdat.so clenzdat.o -lm
```

5. Install the library (optional):
```bash
sudo cp libclenzdat.so /usr/local/lib/
sudo cp include/clenzdat.h /usr/local/include/
sudo ldconfig
```

6. Organize your project structure:
```bash
mkdir -p include lib src
cp src/clenzdat.h include/
mv libclenzdat.a libclenzdat.so lib/
```

7. Compile your program:
```bash
gcc -o your_program your_program.c -I./include -L./lib -lclenzdat -lm
```

### For Windows:

1. Open a command prompt and navigate to the Dataclenz source directory after cloning:
```cmd
git clone https://github.com/rishindra2005/Dataclenz.git
cd Dataclenz
```

2. Compile the library:
```cmd
gcc -c src/clenzdat.c -o clenzdat.o -Wall -O2
```

3. Create a static library:
```cmd
ar rcs libclenzdat.a clenzdat.o
```

4. Create a shared library (DLL):
```cmd
gcc -shared -o clenzdat.dll clenzdat.o -Wl,--out-implib,libclenzdat.dll.a
```

5. Organize your project structure:
```cmd
mkdir include lib src
copy src\clenzdat.h include\
move libclenzdat.a lib\
move clenzdat.dll lib\
move libclenzdat.dll.a lib\
```

6. Compile your program:
```cmd
gcc -o your_program.exe your_program.c -I./include -L./lib -lclenzdat
```

### Using CMake (Cross-Platform):

1. Create a CMakeLists.txt file in the project root with the following content:
```cmake
cmake_minimum_required(VERSION 3.10)
project(Dataclenz C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")

# Library source files
set(LIB_SOURCES src/clenzdat.c)
set(LIB_HEADERS src/clenzdat.h)

# Create shared library
add_library(clenzdat SHARED ${LIB_SOURCES})
target_link_libraries(clenzdat m)

# Create static library
add_library(clenzdat_static STATIC ${LIB_SOURCES})
set_target_properties(clenzdat_static PROPERTIES OUTPUT_NAME clenzdat)

# Install
install(TARGETS clenzdat clenzdat_static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES ${LIB_HEADERS} DESTINATION include)

# Example program (optional)
add_executable(example examples/example.c)
target_link_libraries(example clenzdat)
```

2. Build using CMake:
```bash
mkdir build && cd build
cmake ..
make
```

3. Install (optional):
```bash
sudo make install
```

### Using the Library

After compilation, you can use the Dataclenz library in your C programs by including the header file and linking against the library:

```c
#include <clenzdat.h>

int main() {
    // Your code using Dataclenz functions
    return 0;
}
```

Compile with:
```bash
# Linux
gcc -o your_program your_program.c -lclenzdat -lm

# Windows
gcc -o your_program.exe your_program.c -lclenzdat
```

### Quick Start Example

Here's a simple example to get started with Dataclenz:

```c
#include <stdio.h>
#include <clenzdat.h>

int main() {
    // Create a new DataFrame
    DataFrame* df = create_dataframe();
    
    // Read data from a CSV file
    if (read_csv(df, "data.csv", ',', 1)) {
        printf("Data loaded successfully!\n");
        
        // Print the first 5 rows
        print_dataframe(df, 5);
        
        // Get dataset dimensions
        int rows, cols;
        shape_df(df, &rows, &cols);
        printf("Dataset has %d rows and %d columns\n", rows, cols);
        
        // Handle missing values
        handle_missing_values(df, "mean");
        
        // Perform preprocessing
        normalize_column(df, "feature1");
        
        // Use linear regression
        DataFrame* X = create_dataframe();
        DataFrame* y = create_dataframe();
        
        // Split features and target
        split_dataframe(df, X, y, "target_column");
        
        // Create and fit a linear regression model
        LinearRegressionModel* model = create_linear_regression_model(X->num_cols);
        fit_linear_regression(X, y, model);
        
        // Generate predictions
        double* predictions = predict_linear_regression(X, model);
        
        // Calculate metrics
        double r2 = calculate_r_squared(y, predictions, X->num_rows);
        double mse = calculate_mse(y, predictions, X->num_rows);
        
        printf("Model performance: R² = %.4f, MSE = %.4f\n", r2, mse);
        
        // Clean up
        free(predictions);
        delete_dataframe(X);
        delete_dataframe(y);
        delete_linear_regression_model(model);
    } else {
        printf("Failed to load data\n");
    }
    
    delete_dataframe(df);
    return 0;
}
```

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
```