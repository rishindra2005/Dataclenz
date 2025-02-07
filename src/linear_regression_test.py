import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import r2_score, mean_squared_error
import time
import psutil
import sys

def get_memory_usage():
    process = psutil.Process()
    return process.memory_info().rss / (1024 * 1024)  # in MB

def time_and_memory(func):
    def wrapper(*args, **kwargs):
        start_time = time.time()
        start_memory = get_memory_usage()
        
        result = func(*args, **kwargs)
        
        end_time = time.time()
        end_memory = get_memory_usage()
        
        print(f"{func.__name__}:")
        print(f"  Time taken: {end_time - start_time:.4f} seconds")
        print(f"  Memory used: {end_memory - start_memory:.2f} MB")
        print()
        
        return result
    return wrapper

@time_and_memory
def read_csv(file_path, delimiter):
    return pd.read_csv(file_path, delimiter=delimiter)


@time_and_memory
def preprocess_data(df, target_column):
    def try_convert_numeric(column):
        try:
            return pd.to_numeric(column, errors='raise')
        except ValueError:
            # If conversion fails, try to replace '?' with NaN and convert again
            try:
                return pd.to_numeric(column.replace('?', np.nan), errors='raise')
            except ValueError:
                return None

    # Convert target column to numeric if possible
    df[target_column] = pd.to_numeric(df[target_column], errors='coerce')
    
    # Try to convert all columns to numeric
    converted_columns = {}
    for col in df.columns:
        if col != target_column:
            converted = try_convert_numeric(df[col])
            if converted is not None:
                converted_columns[col] = converted
            else:
                print(f"Deleting column: {col} (index: {df.columns.get_loc(col)})")

    # Create new dataframe with only numeric columns and target column
    df_numeric = pd.DataFrame(converted_columns)
    df_numeric[target_column] = df[target_column]
    
    # Handle missing values by removing rows with any null values
    df_numeric = df_numeric.dropna()
    
    print(f"Remaining columns: {df_numeric.columns.tolist()}")
    print(f"Shape of preprocessed data: {df_numeric.shape}")
    
    return df_numeric

@time_and_memory
def normalize_data(df, target_column):
    # Check if target_column exists in the DataFrame
    if target_column not in df.columns:
        raise ValueError(f"Target column '{target_column}' not found in the DataFrame")

    # Separate features and target
    y = df[target_column]
    X = df.drop(columns=[target_column])

    # Normalize features
    scaler = StandardScaler()
    X_normalized = pd.DataFrame(scaler.fit_transform(X), columns=X.columns)

    return X_normalized, y

@time_and_memory
def split_data(X, y):
    return train_test_split(X, y, test_size=0.2, random_state=42)

@time_and_memory
def fit_model(X_train, y_train):
    model = LinearRegression()
    model.fit(X_train, y_train)
    return model

@time_and_memory
def make_predictions(model, X_test):
    return model.predict(X_test)

@time_and_memory
def calculate_metrics(y_true, y_pred):
    r2 = r2_score(y_true, y_pred)
    mse = mean_squared_error(y_true, y_pred)
    return r2, mse

def main(file_path, delimiter, target_column):
    start_time = time.time()
    start_memory = get_memory_usage()

    # Read CSV
    df = read_csv(file_path, delimiter)
    print(df.head())
    if target_column in df.columns:
        print(f"Target column '{target_column}' data type: {df[target_column].dtype}")
    else:
        print(f"Warning: Target column '{target_column}' not found in the DataFrame")
        print("Available columns:", df.columns.tolist())
        return


    # Preprocess data
    df = preprocess_data(df, target_column)

    # Normalize data
    X, y = normalize_data(df, target_column)

    # Split data
    X_train, X_test, y_train, y_test = split_data(X, y)

    # Fit model
    model = fit_model(X_train, y_train)

    # Make predictions
    y_pred = make_predictions(model, X_test)

    # Calculate metrics
    r2, mse = calculate_metrics(y_test, y_pred)

    print("Linear Regression Results:")
    print(f"R-squared: {r2:.4f}")
    print(f"Mean Squared Error: {mse:.4f}")
    
    end_time = time.time()
    end_memory = get_memory_usage()


    print(f"\nTotal execution time: {end_time - start_time:.4f} seconds")
    print(f"Total memory used: {end_memory - start_memory:.2f} MB")
    input("Press Enter to exit...")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python linear_regression_test.py <csv_file_path> <delimiter> <target_column_name>")
        sys.exit(1)

    file_path = sys.argv[1]
    delimiter = sys.argv[2]
    target_column = sys.argv[3]

    main(file_path, delimiter, target_column)