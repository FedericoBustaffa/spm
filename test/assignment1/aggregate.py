import sys

import numpy as np
import pandas as pd

if __name__ == "__main__":
    plain_df = {"elements": [], "time": []}
    auto_df = {"elements": [], "time": []}
    avx_df = {"elements": [], "time": []}

    dfs = {"plain": plain_df, "auto": auto_df, "avx": avx_df}

    for i, path in enumerate(sys.argv):
        if i == 0:
            continue

        # extract properties
        tokens = path.split("_")
        alg = tokens[0]
        dfs[alg]["elements"].append(int(tokens[2].removesuffix(".txt")))

        with open(path, "r") as file:
            lines = file.readlines()
            ts = [
                float(line.split(" ")[4].removesuffix("s\n"))
                for line in lines
                if line != ""
            ]

            dfs[alg]["time"].append(np.mean(ts))

    # save the results
    for k in dfs.keys():
        df = pd.DataFrame(dfs[k])
        df.sort_values(by="elements", inplace=True)
        df.to_csv(f"results/{k}.csv", index=False,
                  header=True, float_format="%g")
