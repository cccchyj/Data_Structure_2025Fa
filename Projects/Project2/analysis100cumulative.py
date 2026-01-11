import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# ----------------------------
# Load the timing log
# ----------------------------
df = pd.read_csv("time_log.csv")
df = df.sort_values(by=["tree", "step", "run", "ops_completed"])

print("Loaded time_log.csv with", len(df), "records.")

# ----------------------------
# Compute averages
# ----------------------------
avg = df.groupby(["tree", "step", "ops_completed"])["time_ms"].mean().reset_index()

steps = ["init", "delete", "insert"]
colors = {"AVL": "blue", "RB": "red"}

# ----------------------------
# Function to draw and save a plot
# ----------------------------
def plot_step(step_name):
    plt.figure(figsize=(10, 6))

    for tree in ["AVL", "RB"]:
        subset = avg[(avg["tree"] == tree) & (avg["step"] == step_name)]
        if len(subset) == 0:
            continue
        
        plt.plot(
            subset["ops_completed"],
            subset["time_ms"],
            marker="o",
            linewidth=2,
            label=f"{tree}",
            color=colors[tree]
        )
    
    plt.xlabel("Operations Completed")
    plt.ylabel("Average Time (ms)")
    plt.title(f"{step_name.capitalize()} Performance (100 Runs)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    outname = f"{step_name}_plot.png"
    plt.savefig(outname, dpi=200)
    plt.close()
    print(f"Saved {outname}")


# ----------------------------
# Generate the three plots
# ----------------------------
for step in steps:
    plot_step(step)

print("\nAll plots generated successfully:")
print(" - init_plot.png")
print(" - delete_plot.png")
print(" - insert_plot.png")
