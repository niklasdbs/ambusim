"""Base agent."""
from abc import ABC, abstractmethod
from typing import Any, Optional, Tuple

import gym

from EmRL.config.config import AgentConfig


class BaseAgent(ABC):
    """Base class for all agents."""

    def __init__(
        self,
        observation_space: gym.Space,
        action_space: gym.Space,
        cfg: AgentConfig,
        path_to_data: str,
    ):
        """
        Initialize the base agent.

        :param observation_space:  observation space of the env
        :param action_space: action space of the env
        :param cfg: configuration
        :param path_to_data: root path to dataset
        """
        self._action_space = action_space
        self._observation_space = observation_space

    @abstractmethod
    def act(self, observation: Any, state: Optional[Any] = None) -> Tuple[int, Any]:
        """
        Take an observation and output a valid action.

        :param observation: the current observation
        :param state: possible hidden state
        :return: the action and a possible hidden state
        """
        pass

    def test(self):
        """Set the agent into test mode."""
        pass

    def train(self):
        """Set the agent into training mode."""
        pass

    def initial_configuration(self, env):
        """Do initial configuration, some agents require to precompute certain things specific to the environment."""
        pass
