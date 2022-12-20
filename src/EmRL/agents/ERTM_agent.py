"""Baseline: Expected Response Time Model."""
from typing import Any, Optional, Tuple

import gym
import numpy as np
import pandas as pd
import pulp

from EmRL.config.config import AgentConfig
from EmRL.environment.AmbulanceEnv import AmbulanceEnv
from src.EmRL.agents.base_agent import BaseAgent


class ERTMAgent(BaseAgent):
    """This agent acts according to the distribution by ERTM baseline."""

    def __init__(
        self,
        observation_space: gym.Space,
        action_space: gym.Space,
        cfg: AgentConfig,
        path_to_data: str,
        busy_fraction: float,
    ):
        """
        C.f. with base class.

        :param cfg: cf base
        :param observation_space: the obs space
        :param action_space: the action space
                path to the file of travel time from each stations to demand locations.
        :param busy_fraction: probability of a demand location can be served by nearest ambulance.
        :param path_to_data: cf base
        """
        super().__init__(observation_space, action_space, cfg, path_to_data)
        self._precomputed_mapping: dict[int, int] = None  # type:ignore
        self._busy_fraction = busy_fraction
        demand_locations_data = pd.read_csv(f"{path_to_data}/demand_weights.csv")
        self._demand_locations = demand_locations_data["closest_station_index"].tolist()
        self._weights = demand_locations_data["fraction"].tolist()
        self._travel_time = np.load(f"{path_to_data}/station_to_station.npz")["arr_0"]
        # save how many ambulances in each base station

    def _ertm_lp(self, demand_locations, base_stations, p, d, q, travel_time):
        """
        Solve LP-Problem.

        :raises RuntimeError: could not solve the lp problem
        :param demand_locations: demand locations ids
        :param base_stations: base_stations ids
        :param p: the number of ambulances
        :param d: weights
        :param q: busy_fraction
        :param travel_time: travel time matrix

        :return: the distribution of ambulances
        """
        demand_locations_range = range(len(demand_locations))
        base_stations_range = range(len(base_stations))
        k_nearest_ambulance = range(1, p + 1)

        k_nearest_ambulance_lp = pulp.LpProblem(
            "Expected_response_time_model", sense=pulp.LpMinimize
        )
        z = pulp.LpVariable.dicts(
            "z", (demand_locations_range, base_stations_range, k_nearest_ambulance), cat="Binary"
        )
        x = pulp.LpVariable.dicts("x", base_stations_range, lowBound=0, upBound=p, cat="Integer")

        ################################################################
        # objective function
        k_nearest_ambulance_lp += pulp.lpSum(
            [
                [
                    [
                        d[i] * travel_time[j][i] * (1 - q) * (q ** (k - 1)) * z[i][j][k]
                        for k in range(1, p)
                    ]
                    for i in demand_locations_range
                ]
                for j in base_stations_range
            ]
        ) + pulp.lpSum(
            [
                [
                    d[i] * travel_time[j][i] * q ** (p - 1) * z[i][j][p]
                    for i in demand_locations_range
                ]
                for j in base_stations_range
            ]
        )
        ################################################################
        ################################################################
        # conditions in paper
        k_nearest_ambulance_lp += pulp.lpSum([x[j] for j in base_stations_range]) <= p

        for i in demand_locations_range:
            for k in k_nearest_ambulance:
                k_nearest_ambulance_lp += pulp.lpSum([z[i][j][k] for j in base_stations_range]) == 1

        for j in base_stations_range:
            for i in demand_locations_range:
                k_nearest_ambulance_lp += (
                    pulp.lpSum([z[i][j][k] for k in k_nearest_ambulance]) <= x[j]
                )
        ################################################################
        k_nearest_ambulance_lp.solve()
        if pulp.LpStatus[k_nearest_ambulance_lp.status] != "Optimal":
            raise RuntimeError("Could not solve LP")
        distribution_of_ambulances = {}
        for base_station_id, variable in x.items():
            distribution_of_ambulances[base_station_id] = int(variable.varValue)
        return distribution_of_ambulances

    # docstr-coverage:inherited
    def act(self, observation: Any, state: Optional[Any] = None) -> Tuple[int, Any]:  # noqa D102
        ambulance_id_that_needs_to_act = (
            observation  # use the AmbulanceThatNeedsToActEncoder encoder
        )
        return self._precomputed_mapping[ambulance_id_that_needs_to_act], None

    def initial_configuration(self, env: AmbulanceEnv):
        """
        Precompute a static policy.

        :param env: ambulance environment
        """
        state = env.get_state()
        base_stations = state.base_stations

        num_amb = len(state.ambulances)

        distribution_of_ambulances = ERTMAgent._ertm_lp(
            self,
            self._demand_locations,
            base_stations,
            num_amb,
            self._weights,
            self._busy_fraction,
            self._travel_time,
        )

        self._precomputed_mapping = {}
        ambulances_to_assign = list(state.ambulances)
        for base_station_id, ambs_num in distribution_of_ambulances.items():
            number_of_ambulances_to_assign = ambs_num
            # assign n ambulances
            for _ in range(number_of_ambulances_to_assign):
                ambulance = ambulances_to_assign.pop(0)
                self._precomputed_mapping[ambulance.ambulance_id] = base_station_id
