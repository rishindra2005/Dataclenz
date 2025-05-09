import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

def load_performance_data(csv_file):
    """Load performance data from CSV file."""
    print(f"Loading data from {csv_file}...")
    return pd.read_csv(csv_file)

def plot_search_time_comparison(data, output_dir):
    """Plot search time comparison by dataset size for different algorithms."""
    plt.figure(figsize=(12, 8))
    
    # Filter for each search type
    binary_data = data[data['Search Type'] == 'Binary Search']
    jump_data = data[data['Search Type'] == 'Jump Search']
    
    # Group by dataset size
    binary_grouped = binary_data.groupby('Dataset Size')['Average Search Time (s)'].mean()
    jump_grouped = jump_data.groupby('Dataset Size')['Average Search Time (s)'].mean()
    
    # Create log scale plot
    plt.loglog(binary_grouped.index, binary_grouped.values, 'b-o', label='Binary Search')
    plt.loglog(jump_grouped.index, jump_grouped.values, 'r-s', label='Jump Search')
    
    plt.xlabel('Dataset Size (log scale)')
    plt.ylabel('Average Search Time in seconds (log scale)')
    plt.title('Search Algorithm Performance Comparison')
    plt.grid(True, which="both", ls="-", alpha=0.2)
    plt.legend()
    
    plt.savefig(os.path.join(output_dir, 'search_time_comparison.png'), dpi=300)
    plt.close()
    print("Created search time comparison plot")

def plot_comparisons_by_data_type(data, output_dir):
    """Plot number of comparisons by data type."""
    plt.figure(figsize=(14, 8))
    
    # Get unique data types based on structure of the data
    data_types = ['Integer', 'Float', 'String']
    
    # Create a subplot for each data type
    for i, data_type in enumerate(data_types, 1):
        plt.subplot(1, 3, i)
        
        # Filter for alternating rows (since data is organized by type)
        start_idx = (i-1) * 2
        type_data = data.iloc[start_idx::6]  # Every 6th row starting from start_idx
        
        # Also get jump search data
        jump_type_data = data.iloc[start_idx+1::6]  # Every 6th row starting from start_idx+1
        
        plt.loglog(type_data['Dataset Size'], type_data['Average Comparisons'], 'b-o', label='Binary Search')
        plt.loglog(jump_type_data['Dataset Size'], jump_type_data['Average Comparisons'], 'r-s', label='Jump Search')
        
        plt.xlabel('Dataset Size (log scale)')
        plt.ylabel('Average Comparisons (log scale)')
        plt.title(f'{data_type} Data Performance')
        plt.grid(True, which="both", ls="-", alpha=0.2)
        plt.legend()
    
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'comparisons_by_data_type.png'), dpi=300)
    plt.close()
    print("Created comparisons by data type plot")

def plot_memory_usage(data, output_dir):
    """Plot memory usage by dataset size."""
    plt.figure(figsize=(10, 6))
    
    # Group by dataset size and search type
    grouped = data.groupby(['Dataset Size', 'Search Type'])['Memory Usage (KB)'].mean().unstack()
    
    # Plot memory usage
    grouped.plot(kind='bar', figsize=(12, 6))
    plt.xlabel('Dataset Size')
    plt.ylabel('Memory Usage (KB)')
    plt.title('Memory Usage by Search Algorithm')
    plt.xticks(rotation=45)
    plt.grid(True, axis='y', alpha=0.3)
    plt.tight_layout()
    
    plt.savefig(os.path.join(output_dir, 'memory_usage.png'), dpi=300)
    plt.close()
    print("Created memory usage plot")

def plot_successful_searches(data, output_dir):
    """Plot successful searches percentage by dataset size."""
    plt.figure(figsize=(10, 6))
    
    # Calculate success rate
    data['Success Rate'] = data['Successful Searches'] / data['Number of Searches'] * 100
    
    # Group by dataset size and search type
    grouped = data.groupby(['Dataset Size', 'Search Type'])['Success Rate'].mean().unstack()
    
    # Plot success rate
    grouped.plot(kind='line', marker='o', figsize=(12, 6))
    plt.xlabel('Dataset Size')
    plt.ylabel('Success Rate (%)')
    plt.title('Search Success Rate by Algorithm')
    plt.grid(True, alpha=0.3)
    plt.ylim(0, 105)  # Set y-axis limits
    
    plt.savefig(os.path.join(output_dir, 'success_rate.png'), dpi=300)
    plt.close()
    print("Created success rate plot")

def generate_theoretical_comparison(output_dir):
    """Generate theoretical time complexity comparison."""
    plt.figure(figsize=(10, 6))
    
    # Generate dataset sizes on a log scale
    sizes = np.logspace(1, 6, 100)
    
    # Calculate theoretical complexities
    binary_complexity = np.log2(sizes)
    jump_complexity = np.sqrt(sizes)
    linear_complexity = sizes  # For comparison
    
    # Plot theoretical complexities
    plt.loglog(sizes, binary_complexity, 'b-', label='Binary Search: O(log n)')
    plt.loglog(sizes, jump_complexity, 'r-', label='Jump Search: O(sqrt(n))')
    plt.loglog(sizes, linear_complexity, 'g-', label='Linear Search: O(n)')
    
    plt.xlabel('Dataset Size (log scale)')
    plt.ylabel('Time Complexity (log scale)')
    plt.title('Theoretical Time Complexity Comparison')
    plt.grid(True, which="both", ls="-", alpha=0.2)
    plt.legend()
    
    plt.savefig(os.path.join(output_dir, 'theoretical_comparison.png'), dpi=300)
    plt.close()
    print("Created theoretical comparison plot")

def generate_performance_summary(data, output_dir):
    """Generate a performance summary."""
    # Summary statistics
    summary = {
        'binary_vs_jump': data.groupby('Search Type')['Average Search Time (s)'].mean(),
        'scaling_binary': data[data['Search Type'] == 'Binary Search'].groupby('Dataset Size')['Average Search Time (s)'].mean(),
        'scaling_jump': data[data['Search Type'] == 'Jump Search'].groupby('Dataset Size')['Average Search Time (s)'].mean()
    }
    
    # Calculate performance ratio at largest dataset
    largest_size = data['Dataset Size'].max()
    largest_data = data[data['Dataset Size'] == largest_size]
    
    binary_time = largest_data[largest_data['Search Type'] == 'Binary Search']['Average Search Time (s)'].mean()
    jump_time = largest_data[largest_data['Search Type'] == 'Jump Search']['Average Search Time (s)'].mean()
    
    # Create summary report
    with open(os.path.join(output_dir, 'performance_summary.txt'), 'w') as f:
        f.write("SEARCH ALGORITHM PERFORMANCE SUMMARY\n")
        f.write("===================================\n\n")
        
        f.write("Average search times by algorithm:\n")
        f.write(f"Binary Search: {summary['binary_vs_jump']['Binary Search']:.8f} seconds\n")
        f.write(f"Jump Search: {summary['binary_vs_jump']['Jump Search']:.8f} seconds\n\n")
        
        f.write(f"Performance ratio at largest dataset size ({largest_size} items):\n")
        f.write(f"Binary Search: {binary_time:.8f} seconds\n")
        f.write(f"Jump Search: {jump_time:.8f} seconds\n")
        f.write(f"Jump/Binary ratio: {jump_time/binary_time:.2f}x\n\n")
        
        f.write("Theoretical complexity analysis:\n")
        f.write("Binary Search: O(log n) - Logarithmic growth with dataset size\n")
        f.write("Jump Search: O(sqrt(n)) - Square root growth with dataset size\n\n")
        
        f.write("Practical findings:\n")
        
        # Calculate growth rates
        small_size = data['Dataset Size'].min()
        small_binary = data[(data['Dataset Size'] == small_size) & 
                           (data['Search Type'] == 'Binary Search')]['Average Search Time (s)'].mean()
        small_jump = data[(data['Dataset Size'] == small_size) & 
                         (data['Search Type'] == 'Jump Search')]['Average Search Time (s)'].mean()
        
        binary_growth = binary_time / small_binary
        jump_growth = jump_time / small_jump
        size_growth = largest_size / small_size
        
        f.write(f"- For dataset growth of {size_growth:.1f}x (from {small_size} to {largest_size} items):\n")
        f.write(f"  Binary Search time increased by {binary_growth:.1f}x\n")
        f.write(f"  Jump Search time increased by {jump_growth:.1f}x\n\n")
        
        f.write("Recommendations:\n")
        if binary_time < jump_time:
            f.write("- Binary Search performs better overall and should be preferred for most search operations.\n")
        else:
            f.write("- Jump Search showed better performance in these tests and may be preferred.\n")
            
        f.write("- For very small datasets (< 1000 items), the performance difference is negligible.\n")
        f.write("- As dataset size grows, the advantage of Binary Search becomes more significant.\n")
    
    print("Generated performance summary")

def main():
    # Create output directory if it doesn't exist
    output_dir = "Dataclenz/src/performance_analysis"
    os.makedirs(output_dir, exist_ok=True)
    
    # Load data
    data_file = "Dataclenz/src/search_complexity_results.csv"
    data = load_performance_data(data_file)
    
    # Generate plots
    plot_search_time_comparison(data, output_dir)
    plot_comparisons_by_data_type(data, output_dir)
    plot_memory_usage(data, output_dir)
    plot_successful_searches(data, output_dir)
    generate_theoretical_comparison(output_dir)
    
    # Generate summary
    generate_performance_summary(data, output_dir)
    
    print(f"Analysis complete! Results saved to {output_dir}")

if __name__ == "__main__":
    main() 