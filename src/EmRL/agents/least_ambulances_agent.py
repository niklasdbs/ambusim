"""Module for the least ambulances baseline."""
from typing import Any, Optional, Tuple

import gym
import numpy as np

from EmRL.agents.base_agent import BaseAgent
from EmRL.config.config import AgentConfig


class LeastAmbulancesAgent(BaseAgent):
    """The agent deploys ambulances to the base station with the least ambulances."""

    # docstr-coverage:inherited
    def __init__(  # noqa D102
        self,
        observation_space: gym.Space,
        action_space: gym.Space,
        cfg: AgentConfig,
        path_to_data: str,
    ):
        super().__init__(observation_space, action_space, cfg, path_to_data)

    # docstr-coverage:inherited
    def act(self, observation: Any, state: Optional[Any] = None) -> Tuple[int, Any]:  # noqa D102
        # todo be very carefull. Do not use this way (StateObservationEncoder),
        # other observation encoders are the prefered way.
        current_min = np.Inf
        current_base_station_id = None

        for base_station in observation.base_stations:
            if len(base_station.ambulances) < current_min:
                # current_min = base_station.number_of_idle_ambulances
                current_min = len(base_station.ambulances)
                current_base_station_id = base_station.base_station_id

        return current_base_station_id, None  # type:ignore
