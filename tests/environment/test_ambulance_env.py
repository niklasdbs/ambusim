"""Tests for ambulance env python."""
import unittest
from unittest import SkipTest

from hydra import compose, initialize

from EmRL import config
from EmRL.config import Config
from EmRL.environment.AmbulanceEnv import AmbulanceEnv


class TestAmbulanceEnvPython(unittest.TestCase):
    """Unit tests for the python simulator interface."""

    def setUp(self) -> None:
        """Create instance of the env with hydra config."""
        with initialize(version_base=None, config_path="../../config"):
            config.register_configs()
            cfg: Config = compose(config_name="configuration", overrides=["debug=True"])
            raise SkipTest  # todo add this tests again when the env works
            self.env = AmbulanceEnv(cfg.environment_config)

    def test_reset(self):
        """Test resetting the env."""
        self.env.reset()
        raise SkipTest  # todo add this tests again when the env works

    def test_step(self):
        """Test stepping in the env."""
        self.env.step(3)
        raise SkipTest  # todo add this tests again when the env works


if __name__ == "__main__":
    unittest.main()
