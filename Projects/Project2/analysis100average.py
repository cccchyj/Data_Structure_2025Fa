import pandas as pd
import matplotlib.pyplot as plt

# ----------------------------------------------------
# Load CSV
# ----------------------------------------------------
df = pd.read_csv("time_log.csv")
df = df.sort_values(by=["tree", "step", "run", "ops_completed"])

steps = ["init", "delete", "insert"]
interval = {"init": 100, "delete": 10, "insert": 10}
colors = {"AVL": "blue", "RB": "red"}

# ----------------------------------------------------
# Compute per-interval cost (Δ cumulative time)
# ----------------------------------------------------
records = []

for step in steps:
    step_df = df[df["step"] == step]

    for tree in ["AVL", "RB"]:
        tree_df = step_df[step_df["tree"] == tree]

        for run in tree_df["run"].unique():
            sub = tree_df[tree_df["run"] == run].sort_values("ops_completed")

            prev_time = 0.0
            prev_ops = 0

            for _, row in sub.iterrows():
                ops = int(row["ops_completed"])
                cum_time = float(row["time_ms"])

                # Convert cumulative → per interval
                delta_time = cum_time - prev_time

                # Store interval record
                records.append({
                    "step": step,
                    "tree": tree,
                    "run": run,
                    "interval_ops": interval[step],
                    "ops_completed": ops,
                    "time_per_interval": delta_time
                })

                prev_time = cum_time
                prev_ops = ops

# Convert processed data to DataFrame
processed = pd.DataFrame(records)
print("Processed intervals:", len(processed))


# ----------------------------------------------------
# Draw plots for each step
# ----------------------------------------------------
def plot_step(step):
    plt.figure(figsize=(10, 6))

    for tree in ["AVL", "RB"]:
        sub = processed[(processed["step"] == step) &
                        (processed["tree"] == tree)]

        # Compute average time across all runs
        avg = sub.groupby("ops_completed")["time_per_interval"].mean()

        plt.plot(
            avg.index,
            avg.values,
            marker="o",
            linewidth=2,
            label=f"{tree}",
            color=colors[tree]
        )

    plt.xlabel("Operations Completed")
    plt.ylabel(f"Time per {interval[step]} operations (ms)")
    plt.title(f"{step.capitalize()} Performance per Interval")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{step}_plot2.png", dpi=200)
    plt.close()
    print(f"Saved {step}_plot2.png")


# Generate plots
for step in steps:
    plot_step(step)

print("All plots generated successfully.")
