"""Module for the tianshou agent."""
from typing import Any, Optional, Tuple

import gym
import numpy as np
import torch
from tianshou.data import Batch
from tianshou.policy import PGPolicy

from EmRL.agents.base_agent import BaseAgent
from EmRL.config.config import DRLSNAgentConfig
from EmRL.models.drlns import DRLNSNet


class DRLSNAgent(BaseAgent):
    """RL-Base agent, still WIP."""

    # docstr-coverage:inherited
    def __init__(  # noqa D102
        self,
        observation_space: gym.Space,
        action_space: gym.Space,
        cfg: DRLSNAgentConfig,
        path_to_data: str,
    ):
        super().__init__(observation_space, action_space, cfg, path_to_data)
        state_shape = observation_space.shape
        action_shape = action_space.n

        net = DRLNSNet(state_shape=state_shape, action_shape=action_shape)
        optim = torch.optim.Adam(net.parameters(), lr=cfg.lr)

        # docstr-coverage:excused `distribution func`
        def dist(p):
            return torch.distributions.Categorical(logits=p)

        self.policy = PGPolicy(
            net,
            optim,
            dist_fn=dist,
            observation_space=observation_space,
            reward_normalization=True,  # this is equivalent to the baseline used in the approach
            deterministic_eval=True,
            action_space=action_space,
        )

    # docstr-coverage:inherited
    def act(self, observation: Any, state: Optional[Any] = None) -> Tuple[int, Any]:  # noqa D102
        self.policy.eval()

        batch = Batch({"obs": np.expand_dims(observation, 0), "info": {}})
        with torch.no_grad():
            action_batch = self.policy.forward(batch)

        return action_batch["act"].item(), None
