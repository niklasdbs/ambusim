"""Module for the nearest base station agent."""
from typing import Any, Optional, Tuple

import gym

from EmRL.agents.base_agent import BaseAgent
from EmRL.config.config import AgentConfig


class NearestBaseStationAgent(BaseAgent):
    """Class for the nearest base station agent. The nearest base station is computed by the obs encoder."""

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
        return observation, None
