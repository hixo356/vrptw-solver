import subprocess
import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

cmake_lists_dir = os.path.abspath(".")
build_dir = os.path.abspath("build")

os.makedirs(build_dir, exist_ok=True)

subprocess.run(["cmake", cmake_lists_dir], cwd=build_dir, check=True)

subprocess.run(["cmake", "--build", "."], cwd=build_dir, check=True)

binary_path = os.path.join(build_dir, "vrptw", "vrptw")


max_evalutions = 100000
population_size = 100
tournament_size = 15
elite = 2
crossover_probability = 0.6
mutation_probability = 0.4

runs = 2

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
    df = pd.read_csv("GA_C101.txt", names=["gen", "best", "worst", "average"], dtype={"gen": int, "best": float, "worst": float, "average": float})

    plt.plot(df["gen"], df["best"], marker='o')
    first_tick = (df["best"].min() // 1000) * 1000
    plt.yticks(np.arange(first_tick, max(df["best"]) + 1000, 1000))
    plt.title("Output")
    plt.xlabel("Generation")
    plt.ylabel("Best fitness")
    # plt.gca().invert_yaxis()
    plt.grid(True)
    plt.savefig('results/GA_C101.png', dpi=600)
    # plt.show()

proc.wait()