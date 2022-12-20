"""This module contains various trainers."""
from numbers import Number
import os
from abc import ABC, abstractmethod
from collections import defaultdict
from typing import DefaultDict, Dict, List, Union

import numpy as np
import torch
from tqdm import tqdm

from EmRL.agents.base_agent import BaseAgent
from EmRL.agents.drlsn_agent import DRLSNAgent
from EmRL.config.config import (
    DRLSNTrainerConfig,
    EnvironmentConfig,
    NoTrainingTrainerConfig,
    TrainerConfig,
)
from EmRL.environment.AmbulanceEnv import AmbulanceEnv
from EmRL.logger import Logger

import tianshou as ts
from hydra.utils import to_absolute_path

from EmRL.trainer.tianshou_collector import AmbulanceCollector

class Trainer(ABC):
    """Base class for trainers."""

    def __init__(
        self,
        agent: BaseAgent,
        trainer_cfg: TrainerConfig,
        env_cfg: EnvironmentConfig,
        logger: Logger,
    ):
        """
        Init the trainer.

        :param agent: Agent to use
        :param trainer_cfg: configuration
        :param env_cfg: configuration of env
        :param logger: logger
        """
        self.logger = logger
        self.agent = agent

        self.env_step = 0
        self.epoch = 0
        self.gradient_step = 0

    @abstractmethod
    def train(self):
        """Training the agent (some trainers may do not train)."""
        pass

    def evaluate(
        self,
        env: AmbulanceEnv,
        agent: BaseAgent,
        logger: Logger,
        mode: str,
        step: int,
        epoch: int,
    ) -> float:
        """
        Run the evaluation.

        :param epoch: current epoch
        :param step: the current gradient step
        :param mode: train, val, or, test
        :param env: Instance of the environment
        :param agent: The agent to use
        :param logger: The Logger to use
        :return: Metric as float for early stopping
        """
        agent.test()
        metrics: DefaultDict[str, List[float]] = defaultdict(list)
        all_days_done = False
        first_reset = True

        while True:
            if all_days_done:
                break

            obs = env.reset(options={"full_reset": first_reset})
            first_reset = False

            hidden_state = None

            while True:
                action, hidden_state = agent.act(
                    obs, state=hidden_state
                )  # todo implement in all agents
                obs, reward, done, info = env.step(action)

                if done:
                    metrics_current_episode = env.collect_metrics
                    # store metrics for aggregation
                    [
                        metrics[key].append(value)  # type:ignore
                        for key, value in metrics_current_episode.items()
                    ]

                    if info["all_days_done"]:
                        all_days_done = True

                    break

        aggregated_metrics = {f"{key}_avg": np.mean(value) for key, value in metrics.items()}
        logger.log_metrics({**metrics, **aggregated_metrics}, prefix=mode, step=step, epoch=epoch)

        return 0.0  # todo maybe some metric for early stopping


class DRLSNTrainer(Trainer):
    """Trainer for Tainshou still WIP."""

    def __init__(
        self,
        agent: DRLSNAgent,
        trainer_cfg: DRLSNTrainerConfig,
        env_cfg: EnvironmentConfig,
        logger: Logger,
    ):
        """Init everything here."""
        super().__init__(agent, trainer_cfg, env_cfg, logger)

        self.eval_every_n_epochs = trainer_cfg.eval_every_n_epochs
        self.number_of_epochs = trainer_cfg.number_of_epochs
        train_num = 1
        self.train_envs = ts.env.DummyVectorEnv(
            [lambda: AmbulanceEnv(env_cfg, mode="train") for _ in range(train_num)]
        )

        self.validation_env = AmbulanceEnv(env_cfg, mode="val")
        self.policy = agent.policy
        self.train_collector = ts.data.Collector(
            self.policy,
            self.train_envs,
            ts.data.VectorReplayBuffer(100000, train_num),
            exploration_noise=False,
        )

        log_path = "./"

        def save_checkpoint_fn(epoch, env_step, gradient_step):
            """Save checkpoint."""
            # see also: https://pytorch.org/tutorials/beginner/saving_loading_models.html
            ckpt_path = os.path.join(log_path, f"checkpoint_{epoch}.pth")
            torch.save({"model": self.policy.state_dict()}, ckpt_path)
            return ckpt_path

        self.save_checkpoint_fn = save_checkpoint_fn

    def train(self):
        """Training the agent."""
        for epoch in tqdm(range(self.number_of_epochs)):
            self.epoch += 1
            if epoch % self.eval_every_n_epochs == 0:
                _evaluation_result = self.evaluate(  # noqa: F841
                    env=self.validation_env,
                    agent=self.agent,
                    logger=self.logger,
                    mode="val",
                    step=self.epoch,
                    epoch=self.epoch,
                )

            self.policy.train()
            collect_result = self.train_collector.collect(n_episode=1)
            self.env_step += int(collect_result["n/st"])

            losses = self.policy.update(
                sample_size=sum(collect_result["lens"]),
                batch_size=sum(collect_result["lens"]),
                buffer=self.train_collector.buffer,
                repeat=1,
            )

            step = max([1] + [len(v) for v in losses.values() if isinstance(v, list)])
            self.gradient_step += step

            # todo this way of logging will slow us down as it is called way to often...
            self.logger.log_metrics(
                {**{key: np.mean(value) for key, value in losses.items()}, **collect_result},
                prefix="train",
                step=self.epoch,
                epoch=self.epoch,
            )

            self.train_collector.buffer.reset(keep_statistics=False)
            self.logger.save_data(
                self.epoch, self.env_step, self.gradient_step, self.save_checkpoint_fn
            )



class NoTrainingTrainer(Trainer):
    """A trainer for agents that do not require training."""

    def __init__(
        self,
        agent: BaseAgent,
        trainer_cfg: NoTrainingTrainerConfig,
        env_cfg: EnvironmentConfig,
        logger: Logger,
    ):
        """Init."""
        super().__init__(agent, trainer_cfg, env_cfg, logger)

    def train(self):
        """Empty training method."""
        pass
