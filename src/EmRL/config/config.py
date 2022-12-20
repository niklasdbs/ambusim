# flake8: noqa D101
# """Module for structured config class."""
from dataclasses import dataclass
from typing import Union

import torch
import torch.nn as nn
from omegaconf import MISSING, DictConfig


@dataclass
class EnvironmentConfig:
    """Class for structured env config."""

    path_to_dataset: str = MISSING
    number_of_ambulances: int = 42
    observation_encoder: str = "SimpleObservationEncoder"
    do_not_shuffle_days: bool = True
    name: str = MISSING


@dataclass
class Object:
    _target_: str = MISSING


@dataclass
class TrainerConfig:
    object: Object = MISSING


@dataclass
class DRLSNTrainerConfig(TrainerConfig):
    number_of_epochs: int = 3000
    eval_every_n_epochs: int = 500



@dataclass
class NoTrainingTrainerConfig(TrainerConfig):
    pass


@dataclass
class LoggerConfig:
    _target_: str = MISSING


@dataclass
class AggregateLoggerConfig(LoggerConfig):
    pass


@dataclass
class WANDBLoggerConfig(LoggerConfig):
    pass


@dataclass
class ConsoleLoggerConfig(LoggerConfig):
    pass


@dataclass
class MLPConfig:
    number_of_layers: int = MISSING
    hidden_size: int = 0
    activation: Union[torch.nn.Module, str] = nn.ReLU
    activation_after_last_layer: bool = True




@dataclass
class AgentObjectConfig:
    _target_: str = MISSING


@dataclass
class AgentConfig:
    object: AgentObjectConfig = MISSING
    obs_encoder: str = MISSING
    name: str = MISSING


@dataclass
class RandomAgentConfig(AgentConfig):
    seed: int = MISSING


@dataclass
class DRLSNAgentConfig(AgentConfig):
    lr: float = 0.005



@dataclass
class LeastAmbulancesAgentConfig(AgentConfig):
    pass


@dataclass
class MaximumExpectedCoveringLocationProblemAgentConfig(AgentConfig):
    path_to_weights: str = MISSING
    busy_fraction: float = MISSING
    path_to_travel_time_basestation_to_demand_location: str = MISSING
    longest_travel_time: float = MISSING


@dataclass
class DMECLPAgentConfig(AgentConfig):
    path_to_weights: str = MISSING
    busy_fraction: float = MISSING
    path_to_travel_time_basestation_to_demand_location: str = MISSING
    longest_travel_time: float = MISSING


@dataclass
class ERTMAgentConfig(AgentConfig):
    path_to_weights: str = MISSING
    probability: float = MISSING
    path_to_travel_time_basestation_to_demand_location: str = MISSING


@dataclass
class Config:
    """Base structured config."""

    environment_config: EnvironmentConfig = MISSING
    debug: bool = False
    trainer: TrainerConfig = MISSING
    logger: LoggerConfig = MISSING
    agent: AgentConfig = MISSING
    experiment_name: str = "default"
