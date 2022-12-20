"""This module contains the network for the DRLSN agent."""
import torch
from tianshou.utils.net.common import MLP
from torch import nn


class DRLNSNet(nn.Module):
    """Torch module for the DRLSN agent, compatible with tianshou."""

    def __init__(self, state_shape, action_shape):
        """
        Init the network.

        :param state_shape: shape of the state space
        :param action_shape: shape of the action space
        """
        super().__init__()
        self.model = MLP(
            input_dim=state_shape[1],
            output_dim=1,
            flatten_input=False,
            activation=nn.Tanh,
            hidden_sizes=[20, 20],
        )  # todo do not hardcode

    def forward(self, obs, state=None, info={}):  # noqa: B006
        """
        Do a forward pass.

        :param obs: factors for each station
        :param state: not used
        :param info: not used
        :return: tensor with a score for each station
        """
        # obs is in format batch X stations X features per station
        if not isinstance(obs, torch.Tensor):
            obs = torch.tensor(obs, dtype=torch.float)

        logits = self.model(obs)  # shape: batch x stations x 1
        logits = logits.view(obs.size(0), -1)  # shape batch x stations

        return logits, state
