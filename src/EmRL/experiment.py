"""Runs an experiment."""

import hydra
from hydra.utils import instantiate

from EmRL.agents.base_agent import BaseAgent
from EmRL.config import Config, register_configs
from EmRL.environment.AmbulanceEnv import AmbulanceEnv
from EmRL.trainer.trainer import Trainer
from hydra.utils import to_absolute_path
import time
register_configs()


#@hydra.main(version_base=None, config_path="../../config/", config_name="configuration")
def run_experiment(cfg: Config):
    """
    Run a single experiment.

    :param cfg: configuration for the experiment
    """
    cfg.environment_config.path_to_dataset = to_absolute_path(cfg.environment_config.path_to_dataset)
    logger = instantiate(cfg.logger, cfg=cfg, _recursive_=False, logger_cfg=cfg.logger)
    test_env = AmbulanceEnv(cfg.environment_config, mode="test")        
    
    agent: BaseAgent = instantiate(
        cfg.agent.object,
        _recursive_=False,
        observation_space=test_env.observation_space,
        action_space=test_env.action_space,
        cfg=cfg.agent,
        path_to_data=cfg.environment_config.path_to_dataset,
    )

    agent.initial_configuration(test_env)
    env_cfg = cfg.environment_config
    env_cfg.observation_encoder = str(
        env_cfg.observation_encoder
    )  # because hydra can not resolve it
    trainer : Trainer = instantiate(
        cfg.trainer.object, agent, cfg.trainer, env_cfg, logger, _recursive_=False
    )
    trainer.train()
    validation_env = AmbulanceEnv(cfg.environment_config, mode="val")
    
    #for i in range(10):
    #t = time.process_time()
    trainer.evaluate(
            env=validation_env, agent=agent, logger=logger, mode="val", step=None, epoch=-1
    )

    #elapsed_time = time.process_time() - t
    #print(f"val: {elapsed_time}")
    #t = time.process_time()
    trainer.evaluate(
        env=test_env, agent=agent, logger=logger, mode="test", step=None, epoch=-1
    )
    #elapsed_time = time.process_time() - t
    #print(f"test: {elapsed_time}")

    logger.close()
    

if __name__ == "__main__":
    run_experiment()
