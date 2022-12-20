"""Module for the maximum expected covering agent."""
from typing import Any, Optional, Tuple

import gym
import numpy as np
import pandas as pd
import pulp

from EmRL.agents.base_agent import BaseAgent
from EmRL.config.config import AgentConfig
from EmRL.environment.AmbulanceEnv import AmbulanceEnv


class MaximumExpectedCoveringLocationProblemAgent(BaseAgent):
    """Agent that precomputes some mapping and then acts accordingly."""

    def __init__(
        self,
        observation_space: gym.Space,
        action_space: gym.Space,
        cfg: AgentConfig,
        path_to_data: str,
        busy_fraction: float,
        longest_travel_time: int,
    ):
        """
        Init the agent.

        :param observation_space: cf base
        :param action_space:cf base
        :param busy_fraction: busy fraction
        :param longest_travel_time: an ambulance must be present within certain time units.
        :param cfg: cf base
        :param path_to_data: cf base
        """
        super().__init__(observation_space, action_space, cfg, path_to_data)
        self._precomputed_mapping: dict[int, int] = None  # type:ignore
        self._path_to_weights = f"{path_to_data}/demand_weights.csv"
        self._busy_fraction = busy_fraction
        self._path_to_travel_time_basestation_to_demand_location = (
            f"{path_to_data}/station_to_station.npz"
        )
        self._longest_travel_time = longest_travel_time

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
        :raises RuntimeError: could not solve the lp problem
        """
        #  read the file of weights, travel time and get the list of demand location
        demand_locations_data = pd.read_csv(self._path_to_weights)
        travel_time_basestation_to_demand_location = np.load(
            self._path_to_travel_time_basestation_to_demand_location
        )["arr_0"]

        demand_locations = demand_locations_data["closest_station_index"].tolist()
        weights = demand_locations_data["fraction"].tolist()

        state = env.get_state()
        base_stations_from_env = state.base_stations
        number_of_ambulances = len(state.ambulances)

        # Linear Problem
        meclp_lp = pulp.LpProblem("maximum expected covering location", sense=pulp.LpMaximize)
        demand_location = range(len(demand_locations))
        base_station_ids = [station.base_station_id for station in base_stations_from_env]
        k_covering_ambulance = range(1, number_of_ambulances + 1)
        travel_time_demand_location_to_basestation = travel_time_basestation_to_demand_location.T
        demand_location_covering = pulp.LpVariable.dicts(
            "demand location if covering by base station with k ambulances",
            (demand_location, k_covering_ambulance),
            cat="Binary",
        )

        ambulance_in_each_basestation = pulp.LpVariable.dicts(
            "the number of ambulance in each basestation",
            base_station_ids,
            lowBound=0,
            upBound=number_of_ambulances,
            cat="Integer",
        )
        # objective function
        meclp_lp += pulp.lpSum(
            [
                [
                    weights[i]
                    * ((1 - self._busy_fraction) ** (k - 1))
                    * demand_location_covering[i][k]
                    for k in k_covering_ambulance
                ]
                for i in demand_location
            ]
        )

        for demand_loc in range(len(demand_locations)):
            coverage_basestation = np.where(
                travel_time_demand_location_to_basestation[demand_loc] <= self._longest_travel_time
            )[0]
            meclp_lp += pulp.lpSum(
                [demand_location_covering[demand_loc][k] for k in k_covering_ambulance]
            ) <= pulp.lpSum(
                [ambulance_in_each_basestation[coverage] for coverage in coverage_basestation]
            )

        meclp_lp += (
            pulp.lpSum([ambulance_in_each_basestation[base] for base in base_station_ids])
            == number_of_ambulances
        )

        meclp_lp.solve()
        if pulp.LpStatus[meclp_lp.status] != "Optimal":
            raise RuntimeError("Could not solve LP")

        self._precomputed_mapping = {}
        ambulances_to_assign = list(state.ambulances)
        for base_station_id, variable in ambulance_in_each_basestation.items():
            number_of_ambulances_to_assign = int(variable.varValue)
            # assign n ambulances
            for _ in range(number_of_ambulances_to_assign):
                ambulance = ambulances_to_assign.pop(0)
                self._precomputed_mapping[ambulance.ambulance_id] = base_station_id
