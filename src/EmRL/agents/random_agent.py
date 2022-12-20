"""Random agent."""
from typing import Any, Optional, Tuple

import gym

from EmRL.agents.base_agent import BaseAgent
from EmRL.config.config import AgentConfig


class RandomAgent(BaseAgent):
    """This agent acts randomly."""

    def __init__(
        self,
        observation_space: gym.Space,
        action_space: gym.Space,
        cfg: AgentConfig,
        path_to_data: str,
        seed: int,
    ):
        """
        C.f. with base class.

        :param seed: to use for the agent
        :param observation_space: the obs space
        :param action_space: the action space
        :param cfg: configuration
        :param path_to_data: cf base
        """
        super().__init__(observation_space, action_space, cfg, path_to_data)
        self._seed = seed
        self._action_space.seed(self._seed)

    # docstr-coverage:inherited
    def act(self, observation: Any, state: Optional[Any] = None) -> Tuple[int, Any]:  # noqa D102
        return self._action_space.sample(), None
