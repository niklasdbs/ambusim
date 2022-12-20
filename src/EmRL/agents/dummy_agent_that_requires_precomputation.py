"""Module for a dummy agent that precomputes certain data."""
from typing import Any, Optional, Tuple

import gym

from EmRL.agents.base_agent import BaseAgent
from EmRL.config.config import AgentConfig
from EmRL.environment.AmbulanceEnv import AmbulanceEnv


class DummyAgentThatRequiresPrecomputation(BaseAgent):
    """Agent that precomputes some mapping and then acts accordingly."""

    # docstr-coverage:inherited
    def __init__(  # noqa D102
        self,
        observation_space: gym.Space,
        action_space: gym.Space,
        cfg: AgentConfig,
        path_to_data: str,
    ):
        super().__init__(observation_space, action_space, cfg, path_to_data)
        self._precomputed_mapping: dict[int, int] = None  # type:ignore

    def act(self, observation: Any, state: Optional[Any] = None) -> Tuple[int, Any]:
        """
        Act accordingly to the previously computed mapping.

        :return: action, hidden state
        :param observation: the observation
        :param state: a possible hidden state
        """
        ambulance_id_that_needs_to_act = (
            observation  # use the AmbulanceThatNeedsToActEncoder encoder
        )
        return self._precomputed_mapping[ambulance_id_that_needs_to_act], None

    def initial_configuration(self, env: AmbulanceEnv):
        """
        Precompute a static policy.

        :param env: ambulance environment
        """
        # todo pay attention to the fact that get_state() is called before reset!
        state = (
            env.get_state()
        )  # todo maybe we need something like observation encoders for the precomputations!
        self._precomputed_data = {"number_of_hospitals": len(state.hospitals)}
        # create some dummy precomputed mapping
        self._precomputed_mapping = {
            ambulance.ambulance_id: state.base_stations[
                ambulance.ambulance_id % len(state.base_stations)
            ].base_station_id
            for ambulance in state.ambulances
        }
