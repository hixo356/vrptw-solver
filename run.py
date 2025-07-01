import subprocess
import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import re
from collections import defaultdict
from pathlib import Path

cmake_lists_dir = os.path.abspath(".")
build_dir = os.path.abspath("build")
results_dir = os.path.abspath("results")
plots_dir = os.path.abspath("plots")

os.makedirs(build_dir, exist_ok=True)

subprocess.run(["cmake", cmake_lists_dir], cwd=build_dir, check=True)

subprocess.run(["cmake", "--build", "."], cwd=build_dir, check=True)

binary_path = os.path.join(build_dir, "vrptw", "vrptw")

# CPP ARGUMENTS
max_evalutions = 50000
population_size = 100
tournament_size = 12
elite = 2
crossover_probability = 0.85
mutation_probability = 0.4
runs = 10

# SCRIPT
plot = True


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
    instance_groups = defaultdict(list)
    
    for file in folder.iterdir():
        match = re.match(r'([CR]\d+)_\d+', file.stem)
        if match:
            instance_name = match.group(1)
            instance_groups[instance_name].append(file)

    for instance_name, files in instance_groups.items():
        plt.figure()
        bests = []
        for i, file in enumerate(sorted(files)):
            df = pd.read_csv(file, names=["gen", "best", "average", "worst"], dtype={"gen": int, "best": float, "average": float, "worst": float})  # assumes one fitness value per line
            bests.append(df["best"].min())
            plt.plot(df["gen"], df["best"], marker='', label=f'Run {i+1}')

        if min(bests) < 10000:
            step = 1000
        else:
            step = 2000
        first_tick = (min(bests) // step) * step
        plt.yticks(np.arange(first_tick, max(df["best"]) + step, step))
        plt.title(instance_name)
        plt.xlabel("Generation")
        plt.ylabel("Best fitness")
        plt.grid(True)
        plt.legend()
        plt.savefig(f"{plots_dir}/{instance_name}.png", dpi=600)
        plt.show()

proc.wait()