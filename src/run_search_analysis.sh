#!/bin/bash

echo "====================================================="
echo "   Search Algorithm Complexity Analysis"
echo "====================================================="
echo

# Create necessary directories
mkdir -p performance_analysis

# Compile the programs
echo "Compiling search analysis programs..."
make all

# Run the synthetic dataset tests
echo 
echo "Running synthetic dataset performance tests..."
echo "This may take a while for larger datasets..."
./search_complexity_analysis

# Run the real-world dataset tests
echo
echo "Running real-world dataset performance tests..."
./real_world_search_analysis

# Run the analysis script
echo
echo "Generating analysis and visualizations..."
python analyze_search_performance.py

echo
echo "====================================================="
echo "Analysis complete! Results are available in:"
echo "- search_complexity_results.csv (raw synthetic data)"
echo "- real_world_search_results.csv (raw real-world data)"
echo "- performance_analysis/ (visualizations and reports)"
echo "====================================================="
echo
echo "You can view the study documentation in SEARCH_COMPLEXITY_STUDY.md"
echo 