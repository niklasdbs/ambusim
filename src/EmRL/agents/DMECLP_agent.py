"""Baseline: Dynamic maximum expected covering location problem."""
from typing import Any, Optional, Tuple

import gym
import numpy as np
import pandas as pd

from EmRL.agents.base_agent import BaseAgent
from EmRL.config.config import AgentConfig


class DMECLPAgent(BaseAgent):
    """This agent dynamic redeployed the ambulance."""

    def __init__(
        self,
        observation_space: gym.Space,
        action_space: gym.Space,
        cfg: AgentConfig,
        path_to_data: str,
        busy_fraction,
        longest_travel_time,
    ):
        """
        C.f. with DMECLP class.

        :param observation_space: the obs space
        :param action_space: the action space
        :param longest_travel_time: an ambulance must be present within T time units.
        :param busy_fraction: busy fraction.
        :param cfg: cf base
        :param path_to_data: cf base
        """
        super().__init__(observation_space, action_space, cfg, path_to_data)
        self._busy_fraction = busy_fraction
        demand_locations_data = pd.read_csv(f"{path_to_data}/demand_weights.csv")

        self._demand_locations = demand_locations_data["closest_station_index"].tolist()
        self._weights = demand_locations_data["fraction"].tolist()
        self._response_time = longest_travel_time
        self._travel_time = np.load(f"{path_to_data}/station_to_station.npz")["arr_0"]

    # docstr-coverage:inherited
    def act(self, observation: Any, state: Optional[Any] = None) -> Tuple[int, Any]:  # noqa D102
        obs = observation
        number_of_base_stations = int(obs[-1])
        destinations_observation = obs[:-1]

        destinations = {}
        idle_ambulances = []
        for ambulance_id in range(len(destinations_observation)):
            if destinations_observation[ambulance_id] >= 0:
                destinations[ambulance_id] = int(destinations_observation[ambulance_id])
                idle_ambulances.append(ambulance_id)

        best_improvement = 0
        station = -1
        for base_station in range(number_of_base_stations):
            coverage_improvement = 0
            for demand_location in self._demand_locations:
                k = 0
                if self._travel_time[base_station][demand_location] <= self._response_time:
                    k += 1
                    for ambulance in idle_ambulances:
                        if (
                            self._travel_time[destinations[ambulance]][demand_location]
                            <= self._response_time
                        ):
                            k += 1
                    coverage_improvement += (
                        self._weights[demand_location]
                        * (1 - self._busy_fraction)
                        * self._busy_fraction ** (k - 1)
                    )
            if coverage_improvement > best_improvement:
                station = base_station
                best_improvement = coverage_improvement

        return station, None
