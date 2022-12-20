# flake8: noqa T201
"""Finds runs and prints them."""
from collections import defaultdict

import wandb


# docstr-coverage:excused `plot`
def main():
    metric_to_find_best_run = "val/average_response_time_avg"
    api = wandb.Api(timeout=40)
    filters = {"$and":
                   [{"config.environment_config.number_of_ambulances": 20},
                    {"tags": "new"}
                    ]}
    #filters = None
    runs = filter_runs(api, filters, metric_to_find_best_run)

    runs_sorted = sorted(
        runs, key=lambda t: t.summary[metric_to_find_best_run]["min"], reverse=False
    )
    bests_by_kind = find_bests_of_its_kind(runs_sorted)
    print_bests_of_its_kind(bests_by_kind, metric_to_find_best_run)


# docstr-coverage:excused `plot`
def filter_runs(api, filters, metric):
    runs = api.runs(path="safety-reinfirst-ss22/ambulance", filters=filters)
    runs = [run for run in runs if metric in run.summary]

    return runs


# docstr-coverage:excused `plot`
def find_bests_of_its_kind(runs_sorted):
    bests_by_its_kind = defaultdict(list)

    for run in runs_sorted:
        bests_by_its_kind[run.config["agent"]["name"]].append(run)

    return bests_by_its_kind


# docstr-coverage:excused `plot`
def print_bests_of_its_kind(bests_by_its_kind, metric):
    for kind in bests_by_its_kind.keys():
        print(f"---- {kind} ----")

        for run in bests_by_its_kind[kind]:
            print(f"{metric}: {round(run.summary[metric]['min'], ndigits=2)} : {run.name}")
            for key, value in run.summary.items():
                try:
                    print(f"{key}: {round(value, ndigits=2)} : {run.name}")
                except:
                    pass

        print("------------------------")


if __name__ == "__main__":
    main()
