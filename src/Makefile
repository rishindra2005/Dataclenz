CC = gcc
CFLAGS = -Wall -Wextra -g -O2 -lm

# Targets
all: search_complexity_analysis real_world_search_analysis

# Compile the search complexity analysis program
search_complexity_analysis: search_complexity_analysis.c clenzdat.c
	$(CC) $(CFLAGS) -o search_complexity_analysis search_complexity_analysis.c clenzdat.c -lm

# Compile the real-world search analysis program
real_world_search_analysis: real_world_search_analysis.c clenzdat.c
	$(CC) $(CFLAGS) -o real_world_search_analysis real_world_search_analysis.c clenzdat.c -lm

# Run the synthetic search performance tests
run_synthetic_tests: search_complexity_analysis
	./search_complexity_analysis

# Run the real-world search performance tests
run_real_world_tests: real_world_search_analysis
	./real_world_search_analysis

# Run the Python analysis script to generate visualizations
analyze: run_synthetic_tests
	python analyze_search_performance.py

# Run all tests and analyze
full_analysis: run_synthetic_tests run_real_world_tests
	python analyze_search_performance.py

# Clean up compiled files
clean:
	rm -f search_complexity_analysis real_world_search_analysis

.PHONY: all run_synthetic_tests run_real_world_tests analyze full_analysis clean 