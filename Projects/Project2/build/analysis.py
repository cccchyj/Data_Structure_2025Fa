import pandas as pd
import matplotlib.pyplot as plt

#---------------------------------------
# Load timing data
#---------------------------------------
df = pd.read_csv("./build/time_log.csv")

# Ensure correct numeric types
df['operations'] = df['operations'].astype(int)
df['time_ms'] = df['time_ms'].astype(float)

#---------------------------------------
# Helper: Plot a single step (init/delete/insert)
#---------------------------------------
def plot_step(step_name, output_png):
    data = df[df['step'] == step_name]

    avl = data[data['tree'] == "AVL"]
    rb  = data[data['tree'] == "RB"]

    plt.figure(figsize=(8,5))
    plt.plot(avl['operations'], avl['time_ms'], label="AVL Tree", marker='o')
    plt.plot(rb['operations'], rb['time_ms'],  label="Red-Black Tree", marker='o')

    plt.title(f"{step_name.capitalize()} Performance Comparison")
    plt.xlabel("Operations")
    plt.ylabel("Time (ms)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(output_png)
    plt.close()
    print(f"Saved: {output_png}")

#---------------------------------------
# Generate all required charts
#---------------------------------------
plot_step("init",   "init_comparison.png")
plot_step("delete", "delete_comparison.png")
plot_step("insert", "insert_comparison.png")

#---------------------------------------
# Textual interpretation
#---------------------------------------
print("\n==== Analysis Summary ====\n")

def analyze_step(step_name):
    step = df[df['step'] == step_name]
    avl = step[step['tree'] == "AVL"]['time_ms'].mean()
    rb  = step[step['tree'] == "RB"]['time_ms'].mean()

    if avl > rb:
        faster = "Red-Black Tree"
    else:
        faster = "AVL Tree"

    print(f"Step: {step_name.upper()}")
    print(f"  AVL avg time: {avl:.3f} ms")
    print(f"  RB  avg time: {rb:.3f} ms")
    print(f"  → Faster: {faster}\n")

analyze_step("init")
analyze_step("delete")
analyze_step("insert")

print("Charts generated and analysis complete.")
