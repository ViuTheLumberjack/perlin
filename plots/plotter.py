from matplotlib import pyplot as plt
import numpy as np
import pandas as pd
import os
import re

# open the times file and read the data
def read_csv(file_path: os.path) -> pd.Series:
    try:
        data = pd.read_csv(file_path, header=None, names=["time"])
        return data
    except FileNotFoundError:
        print(f"File {file_path} not found.")
        return pd.DataFrame()
    
def plot_times(data: pd.DataFrame, title: str, line_label: str, xlabel: str, ylabel: str, save_path: str, *args, **kwargs):
    plt.figure(figsize=(10, 6))
    marker = kwargs.get('marker', 'o')  # Optional: allow specifying marker
    color = kwargs.get('color', 'blue')  # Optional: allow specifying color
    plt.plot(data["threads"], data["mean"], marker=marker, color=color, label=line_label)

    additional_lines = kwargs.get('additional_lines')
    if additional_lines:
        for line_info in additional_lines:
            df = line_info.get('data')
            label = line_info.get('label')
            color = line_info.get('color') # Optional: allow specifying color
            marker = line_info.get('marker', 'x') # Optional: allow specifying marker
            if df is not None and label is not None and "threads" in df.columns and "mean" in df.columns:
                plt.plot(df["threads"], df["mean"], marker=marker, label=label, color=color)
    
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.grid(True)
    plt.xticks(data["threads"])  # Ensure all indices are shown on x-axis
    plt.tight_layout()
    plt.legend()
    plt.savefig(save_path)
    plt.close()
    
def plot_speedup(data: pd.DataFrame, title: str, line_label: str, xlabel: str, ylabel: str, save_path: str, *args, **kwargs):
    plt.figure(figsize=(10, 6))

    marker = kwargs.get('marker', 'o')  # Optional: allow specifying marker
    color = kwargs.get('color', 'blue')  # Optional: allow specifying color
    plt.plot(data["threads"], data["speedup"], marker=marker, label=line_label, color=color)

    additional_lines = kwargs.get('additional_lines')
    if additional_lines:
        for line_info in additional_lines:
            df = line_info.get('data')
            label = line_info.get('label')
            color = line_info.get('color') # Optional: allow specifying color
            marker = line_info.get('marker', 'x') # Optional: allow specifying marker
            if df is not None and label is not None and "threads" in df.columns and "speedup" in df.columns:
                plt.plot(df["threads"], df["speedup"], marker=marker, label=label, color=color)

    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.grid(True)
    plt.xticks(data["threads"])  # Ensure all threads values are shown on x-axis
    plt.tight_layout()
    plt.legend()
    plt.savefig(save_path)
    plt.close()

if __name__ == "__main__":
    # get the path to this file 
    script_dir = os.path.dirname(os.path.abspath(__file__))
    try: 
        #raise FileNotFoundError  # Force the code to read the csv files again
        folder_summary_df = pd.read_csv(os.path.join(script_dir, "all_data_summary.csv"))
    except FileNotFoundError:
        folder_summary_df = pd.DataFrame()
    
    plot_dir = os.path.join(script_dir, "plots")
    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)

    if folder_summary_df.empty:    # for each folder in the script path, read the csv files and plot the data
        for folder in os.listdir(script_dir):
            if not os.path.isdir(os.path.join(script_dir, folder)):
                continue

            folder_path = os.path.join(script_dir, folder)

            # read the csv files in the folder
            for file_name in os.listdir(folder_path):
                if not file_name.endswith(".txt"):
                    continue
                current_file_path = os.path.join(folder_path, file_name)
                
                data = read_csv(current_file_path)
                file_name = file_name[:-4] 

                if data.empty:
                    continue

                # add the mean to a dataframe

                # resolution is in the file name as 640X480
                match = re.search(r'(\d+)x(\d+)', file_name)
                if match:
                    resolution = f"{match.group(1)}x{match.group(2)}"
                else:
                    resolution = "unknown"

                # extraction of noise type from the file name
                # e.g. 640x480_ComplexNoise_1parallel_g -> ComplexNoise
                noise_type_match = re.search(r'\d+x\d+_([a-zA-Z]*)_\d*\w_*g*', file_name)
                if noise_type_match:
                    noise_type = noise_type_match.group(1)
                else:
                    noise_type = "unknown"

                # extraction of parallel type from the file name
                # e.g. 640x480_ComplexNoise_1parallel_g -> parallel_g / omp / sequential / parallel
                parallel_type_match = re.search(r'\d+x\d+_[a-zA-Z]*_\d*([a-zA-Z]*)_*g*', file_name)
                if parallel_type_match:
                    parallel_type = parallel_type_match.group(1)
                else:
                    parallel_type = "unknown"

                if parallel_type != "sequential":
                    threads = re.search(r'\d+x\d+_[a-zA-Z]*_(\d*)parallel_*g*', file_name)
                    if threads:
                        threads = threads.group(1)
                    else:
                        threads = "unknown"

                    graph = re.search(r'\d+x\d+_[a-zA-Z]*_\d*parallel_*(g*)', file_name)
                    if graph is not None and graph.group(1) == 'g':
                        graph = True
                    else:
                        graph = False
                else:
                    threads = "0"
                    graph = False

                mean = {
                    "noise": noise_type,
                    "parallel_type": parallel_type,
                    "graph": graph,
                    "threads": threads,
                    "resolution": resolution,
                    "mean": data["time"].mean(),
                    "std": data["time"].std(),
                    "max": data["time"].max(),
                    "min": data["time"].min(),
                }
            
                new_row_df = pd.DataFrame([mean])
                folder_summary_df = pd.concat([folder_summary_df, new_row_df], ignore_index=True)
            folder_summary_df.to_csv(os.path.join(script_dir, f"all_data_summary.csv"), index=False)
            
    # 1) Print comparison between OpenMP and TBB parallelization (only 12 threads exists)
    openmp_df = folder_summary_df[folder_summary_df["parallel_type"] == "omp"].copy()
    tbb_12_df = folder_summary_df[(folder_summary_df["parallel_type"] == "parallel") & (folder_summary_df["threads"] == "12") & (folder_summary_df["noise"] == "RidgedNoise")].copy()

    print("OpenMP vs TBB (12 threads) comparison:")
    print(openmp_df)
    print(tbb_12_df)

    for resolution in folder_summary_df["resolution"].unique():
        for noise in folder_summary_df["noise"].unique():
            sequential = folder_summary_df[(folder_summary_df["parallel_type"] == "sequential") & (folder_summary_df["resolution"] == resolution) & (folder_summary_df["noise"] == noise)].copy()
            sequential_time = sequential["mean"].values[0] if not sequential.empty else None

            tbb_parallel = folder_summary_df[(folder_summary_df["parallel_type"] == "parallel") & (folder_summary_df["graph"] == False) & (folder_summary_df["resolution"] == resolution) & (folder_summary_df["noise"] == noise)].copy()
            tbb_parallel["speedup"] = sequential_time / tbb_parallel["mean"].values
            tbb_parallel["threads"] = tbb_parallel["threads"].astype(int)
            tbb_parallel = tbb_parallel.sort_values(by="threads")

            tbb_parallel_graph = folder_summary_df[(folder_summary_df["parallel_type"] == "parallel") & (folder_summary_df["graph"] == True) & (folder_summary_df["resolution"] == resolution) & (folder_summary_df["noise"] == noise)].copy()
            tbb_parallel_graph["speedup"] = sequential_time / tbb_parallel_graph["mean"].values
            tbb_parallel_graph["threads"] = tbb_parallel_graph["threads"].astype(int)
            tbb_parallel_graph = tbb_parallel_graph.sort_values(by="threads")

            if tbb_parallel.empty or tbb_parallel_graph.empty:
                print(f"No data for resolution {resolution} and noise {noise}. Skipping plots.")
                continue

            plot_speedup(
                data=tbb_parallel,
                title=f"Speedup for TBB Parallelization for {noise} at ",
                xlabel="Threads",
                ylabel="Speedup",
                line_label="TBB Parallel For Speedup",
                save_path=os.path.join(plot_dir, f"{resolution}_{noise}_speedup.png"),
                color='tab:blue',
                marker='o',
                additional_lines=[
                    {
                        'data': tbb_parallel_graph,
                        'label': 'TBB Graph Speedup',
                        'color': 'tab:red',
                        'marker': 'v'
                    }
                ]
            )

            plot_times(
                data=tbb_parallel,
                title=f"Time for TBB Parallelization for {noise} (Resolution: {resolution})",
                xlabel="Threads",
                ylabel="Time (microseconds)",
                line_label="TBB Parallel For - 640x480",
                save_path=os.path.join(plot_dir, f"{resolution}_{noise}_times.png"),
                color='tab:blue',
                marker='o',
                additional_lines=[
                    {
                        'data': tbb_parallel_graph,
                        'label': 'TBB Graph Time - 640x480',
                        'color': 'tab:red',
                        'marker': 'o'
                    },
                    {
                        'data': pd.DataFrame({
                            'threads': tbb_parallel['threads'],
                            'mean': np.repeat(sequential['mean'].values[0], len(tbb_parallel['threads'].unique()))
                        }),
                        'label': 'Sequential Time - 640x480',
                        'color': 'tab:grey',
                        'marker': 'o'
                    }
                ]
            )

    sequential_all = folder_summary_df[(folder_summary_df["parallel_type"] == "sequential")]
    # Group by resolution and divide by the sequential value of the same 
    folder_summary_df["speedup"] = folder_summary_df.apply(
        lambda row: sequential_all[(sequential_all['resolution'] == row['resolution']) & (sequential_all['noise'] == row['noise'])]['mean'].values[0] / row['mean'],
        axis=1
    )
    for noise in folder_summary_df["noise"].unique():
        sequential = sequential_all[sequential_all['noise']==noise]
        tbb_parallel = folder_summary_df[(folder_summary_df["parallel_type"] == "parallel") & (folder_summary_df["graph"] == False) & (folder_summary_df["noise"] == noise)].copy()
        tbb_parallel['threads'] = tbb_parallel['threads'].astype(int)
        tbb_parallel = tbb_parallel.sort_values(by="threads")
        tbb_parallel_graph = folder_summary_df[(folder_summary_df["parallel_type"] == "parallel") & (folder_summary_df["graph"] == True) & (folder_summary_df["noise"] == noise)].copy()
        tbb_parallel_graph['threads'] = tbb_parallel_graph['threads'].astype(int)
        tbb_parallel_graph = tbb_parallel_graph.sort_values(by="threads")


        plot_speedup(
            data=tbb_parallel[tbb_parallel['resolution'] == "640x480"],
            title=f"Speedup for TBB Parallelization for {noise}",
            xlabel="Threads",
            ylabel="Speedup",
            line_label="TBB Parallel For Speedup - 640x480",
            save_path=os.path.join(plot_dir, f"all_{noise}_speedup.png"),
            color='maroon',
            marker='o',
            additional_lines=[
                {
                    'data': tbb_parallel_graph[tbb_parallel_graph['resolution'] == "640x480"],
                    'label': 'TBB Graph Speedup - 640x480',
                    'color': 'indianred',
                    'marker': 'v'
                },
                {
                    'data': tbb_parallel[tbb_parallel['resolution'] == "1024x786"],
                    'label': 'TBB Parallel For Speedup - 1024x786',
                    'color': 'teal',
                    'marker': 'o'
                },
                {
                    'data': tbb_parallel_graph[tbb_parallel_graph['resolution'] == "1024x786"],
                    'label': 'TBB Graph Speedup - 1024x786',
                    'color': 'lightseagreen',
                    'marker': 'v'
                },
                {
                    'data': tbb_parallel[tbb_parallel['resolution'] == "1920x1080"],
                    'label': 'TBB Parallel For Speedup - 1920x1080',
                    'color': 'darkgoldenrod',
                    'marker': 'o'
                },
                {
                    'data': tbb_parallel_graph[tbb_parallel_graph['resolution'] == "1920x1080"],
                    'label': 'TBB Graph Speedup - 1920x1080',
                    'color': 'tan',
                    'marker': 'v'
                }
            ]
        )

        plot_times(
            data=tbb_parallel[tbb_parallel['resolution'] == "640x480"],
            title=f"Time for TBB Parallelization for {noise}",
            xlabel="Threads",
            ylabel="Time (microseconds)",
            line_label="TBB Parallel For - 640x480",
            save_path=os.path.join(plot_dir, f"all_{noise}_times.png"),
            color='maroon',
            marker='o',
            additional_lines=[
                {
                    'data': tbb_parallel_graph[tbb_parallel_graph['resolution'] == "640x480"],
                    'label': 'TBB Graph Time - 640x480',
                    'color': 'indianred',
                    'marker': 'v'
                },
                
                {
                    'data': pd.DataFrame({
                        'threads': sorted(tbb_parallel['threads'].unique().astype(int)),
                        'mean': np.repeat(sequential[sequential['resolution']=="640x480"]['mean'].values[0], len(tbb_parallel['threads'].unique()))
                    }),
                    'label': 'Sequential Time - 640x480',
                    'color': 'lightsalmon',
                    'marker': '^'
                },
                {
                    'data': tbb_parallel[tbb_parallel['resolution'] == "1024x786"],
                    'label': 'TBB Par For Time - 1024x786',
                    'color': 'teal',
                    'marker': 'o'
                },
                {
                    'data': tbb_parallel_graph[tbb_parallel_graph['resolution'] == "1024x786"],
                    'label': 'TBB Graph Time - 1024x786',
                    'color': 'lightseagreen',
                    'marker': 'v'
                },
                {
                    'data': pd.DataFrame({
                        'threads': sorted(tbb_parallel['threads'].unique().astype(int)),
                        'mean': np.repeat(sequential[sequential['resolution']=="1024x786"]['mean'].values[0], len(tbb_parallel['threads'].unique()))
                    }),
                    'label': 'Sequential Time - 1024x786',
                    'color': 'aquamarine',
                    'marker': '^'
                },
                {
                    'data': tbb_parallel[tbb_parallel['resolution'] == "1920x1080"],
                    'label': 'TBB Par For Time - 1920x1080',
                    'color': 'darkgoldenrod',
                    'marker': 'o'
                },
                {
                    'data': tbb_parallel_graph[tbb_parallel_graph['resolution'] == "1920x1080"],
                    'label': 'TBB Graph Time - 1920x1080',
                    'color': 'tan',
                    'marker': 'v'
                },
                {
                    'data': pd.DataFrame({
                        'threads': sorted(tbb_parallel['threads'].unique().astype(int)),
                        'mean': np.repeat(sequential[sequential['resolution']=="1920x1080"]['mean'].values[0], len(tbb_parallel['threads'].unique()))
                    }),
                    'label': 'Sequential Time - 1920x1080',
                    'color': 'wheat',
                    'marker': '^'
                }
            ]
        )