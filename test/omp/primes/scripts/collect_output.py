import os
import sys

import matplotlib.pyplot as plt
import numpy as np

if __name__ == "__main__":
    outputs = []
    results = {2**i: 0.0 for i in range(0, 6)}
    filepaths = os.listdir(sys.argv[1])
    for fp in filepaths:
        with open(f"{sys.argv[1]}/{fp}", "r") as f:
            line = f.readline()[:-1].split(" ")
            results[int(line[0])] += float(line[3])
            outputs.append(f.readline()[:-1])

    for k in results.keys():
        results[k] /= 10.0

    plt.figure(figsize=(12, 9), dpi=200)
    plt.plot(np.array(list(results.keys())), np.array(list(results.values())))
    plt.grid(True)
    plt.tight_layout()
    plt.savefig("times.svg")
    # plt.show()
