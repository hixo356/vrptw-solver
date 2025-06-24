import subprocess
import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

cmake_lists_dir = os.path.abspath(".")
build_dir = os.path.abspath("build")
results_dir = os.path.abspath("results")

os.makedirs(build_dir, exist_ok=True)

subprocess.run(["cmake", cmake_lists_dir], cwd=build_dir, check=True)

subprocess.run(["cmake", "--build", "."], cwd=build_dir, check=True)

binary_path = os.path.join(build_dir, "vrptw", "vrptw")

# CPP ARGUMENTS
max_evalutions = 50000
population_size = 100
tournament_size = 15
elite = 2
crossover_probability = 0.6
mutation_probability = 0.4
runs = 2

# SCRIPT
plot = True
step = 2000


args = [str(max_evalutions), str(population_size), str(tournament_size), str(elite), str(crossover_probability), str(mutation_probability), str(runs)]

proc = subprocess.Popen([binary_path] + args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

output = []
for line in proc.stdout:
    print(line, end="")
    output.append(line)

# data = [list(map(int, line.split(","))) for line in output]
# df = pd.DataFrame(data, columns=["gen", "best", "worst", "average"])

if plot:
    folder = Path(results_dir)

    for file in folder.iterdir():
        df = pd.read_csv(file, names=["gen", "best", "average", "worst"], dtype={"gen": int, "best": float, "average": float, "worst": float})

        plt.plot(df["gen"], df["best"], marker='o')
        first_tick = (df["best"].min() // step) * step
        plt.yticks(np.arange(first_tick, max(df["best"]) + step, step))
        plt.title(file.stem)
        plt.xlabel("Generation")
        plt.ylabel("Best fitness")
        plt.grid(True)
        plt.savefig(f"{results_dir}/{file.stem}.png", dpi=600)
        plt.show()

proc.wait()