import time
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import r2_score, mean_squared_error

# Start time
start = time.time()

# Load the csv file into a pandas dataframe
df = pd.read_csv('large_sample.csv')

# Remove null lines
df = df.dropna()

# Remove ocean_proximity column
scaler = StandardScaler()


# Separate features (X) and target variable (y)
X = df.drop('result', axis=1)
y = df['result']
# Normalize X columns
scaler = StandardScaler()
X_normalized = scaler.fit_transform(X)
y_normalized = scaler.fit_transform(y.values.reshape(-1, 1)).flatten()

# Split the data into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(X_normalized, y_normalized, test_size=0.2, random_state=42)

# Create and fit the linear regression model
model = LinearRegression()

# Start time for model fitting
fit_start = time.time()

model.fit(X_train, y_train)

# End time for model fitting
fit_end = time.time()

# Make predictions
y_pred = model.predict(X_test)

# Calculate R2 score and MSE
r2 = r2_score(y_test, y_pred)
mse = mean_squared_error(y_test, y_pred)

# End time
end = time.time()

# Print results
print('Time taken to load and preprocess the data:', fit_start - start, 'seconds')
print('Time taken to fit the model:', fit_end - fit_start, 'seconds')
print('Total time taken:', end - start, 'seconds')
print('R2 Score:', r2)
print('Mean Squared Error:', mse)
# take input from user
num_rows = input('input any key ')