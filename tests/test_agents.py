"""Module for testing the python agents."""
import unittest
from unittest import SkipTest

from hydra import compose, initialize

from EmRL import config
from EmRL.agents.random_agent import RandomAgent
from EmRL.config import Config
from EmRL.environment.AmbulanceEnv import AmbulanceEnv


class TestAgents(unittest.TestCase):
    """Unit test for testing the agents."""

    def setUp(self) -> None:
        """Create instance of the env with hydra config."""
        with initialize(version_base=None, config_path="../config"):
            config.register_configs()
            cfg: Config = compose(
                config_name="configuration",
                overrides=[
                    "debug=True",
                    "environment_config.path_to_dataset=/ambdata",
                ],
            )
            raise SkipTest  # todo add this tests again when the env works
            self.env = AmbulanceEnv(cfg.environment_config)

    def test_random_agent(self):
        """Test the random agent."""
        # raise SkipTest  # todo add this tests again when the env works
        random_agent = RandomAgent(self.env.observation_space, self.env.action_space)
        obs = self.env.reset()
        for _ in range(99999):
            action = random_agent.act(obs)
            obs, reward, done, info = self.env.step(action)


if __name__ == "__main__":
    unittest.main()
