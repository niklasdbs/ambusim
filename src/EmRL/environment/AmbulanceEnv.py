"""Ambulance env class."""
import typing
from typing import Optional, Tuple, Union

import gym
import numpy as np
from gym.core import ObsType
from gym.spaces import Box, Dict, Discrete

import ambulance_env  # noqa:I001,F811
from EmRL.config.config import EnvironmentConfig


class AmbulanceEnv(gym.Env):
    """This class implements a gym env and interacts with the cpp env under the hood."""

    def __init__(self, env_config: EnvironmentConfig, mode: str = "train"):
        """Create an instance of the simulator.

        :param mode: specify whether train, val, or test mode should be used
        :param env_config: the config file
        :raises Exception: invalid observation shape
        """
        self._cpp_ambu_env = ambulance_env.AmbulanceEnvironment(
            env_config.path_to_dataset,
            env_config.observation_encoder,
            mode,
            env_config.do_not_shuffle_days,
            env_config.number_of_ambulances,
        )  # reference to the c++ simulator
        obs_shape = self._cpp_ambu_env.get_observation_shape()

        if type(obs_shape) is dict:
            self.observation_space = Dict(
                {
                    key: Box(shape=tuple(space), low=-1.0, high=1.0, dtype=np.float32)
                    for key, space in obs_shape.items()
                }
            )
        elif type(obs_shape) is list:
            self.observation_space = Box(
                shape=tuple(obs_shape), low=-1.0, high=1.0, dtype=np.float32
            )
        elif type(obs_shape) is int:
            self.observation_space = Discrete(obs_shape)
        else:
            raise Exception("Unknown observation shape")

        self.action_space = Discrete(
            self._cpp_ambu_env.number_of_actions
        )  # get number of actions  from simulator

    def step(self, action: int) -> Tuple[ObsType, float, bool, dict]:
        """
        Take a single step in the simulation. This means until the next ambulance needs to act again.

        :param action: the action to execute
        :return: next obs, reward, done, info
        """
        return self._cpp_ambu_env.step(action)

    def render(self, mode="human", reset_after_render=False):
        """Render.

        :param mode: the mode
        :param reset_after_render: bool
        """
        state = self.get_state()
        number_of_idle_ambu_at_station = {
            station.base_station_id: station.number_of_idle_ambulances for station in state.stations
        }
        print(  # noqa:T201
            f"number of idle ambs at each base station: {number_of_idle_ambu_at_station}"
        )
        # todo implement: 1. set flag in env so that this information will get computed when init env
        # 2. only if the flag is set aggregate this data in the env
        # 3. Than at this state get this data from state or some method and print it
        print(  # noqa:T201
            "number of incidents at each demand location: TODO",
        )  # noqa:T201

    def reset(
        self,
        *,
        seed: Optional[int] = None,
        return_info: bool = False,
        options: Optional[dict] = None,
    ) -> Union[ObsType, Tuple[ObsType, dict]]:
        """
        Reset the environment.

        :param seed: seed
        :param return_info: cf. base
        :param options: full_reset to force a full reset of days when not shuffling the days
        :return: initial observation
        """
        full_reset = False

        if options:
            full_reset = options["full_reset"]

        return self._cpp_ambu_env.reset(full_reset)

    @property
    def collect_metrics(self) -> typing.Dict[str, float]:
        """Collect advanced metrics at the end of an episode."""
        return self._cpp_ambu_env.get_metrics()

    def get_state(self) -> ambulance_env.AmbulanceEnvironmentState:
        """Return the state object of the ambulance env. Use this only for precomputation."""
        return self._cpp_ambu_env.get_state()

    def get_secondary_observation(self) -> ObsType:
        """Return secondary observation for offline reinforcment learning."""
        return self._cpp_ambu_env.get_secondary_observation()