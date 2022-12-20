"""A module for configuration."""
from hydra.core.config_store import ConfigStore

from EmRL.config.config import (
    AggregateLoggerConfig,
    Config,
    ConsoleLoggerConfig,
    DMECLPAgentConfig,
    DRLSNTrainerConfig,
    EnvironmentConfig,
    ERTMAgentConfig,
    LeastAmbulancesAgentConfig,
    LoggerConfig,
    MaximumExpectedCoveringLocationProblemAgentConfig,
    NoTrainingTrainerConfig,
    RandomAgentConfig,
    WANDBLoggerConfig,
)

__all__ = ["register_configs", "Config", "EnvironmentConfig"]


def register_configs() -> None:
    """Register configuration for hydra."""
    cs = ConfigStore.instance()
    cs.store(
        name="sf",
        node=EnvironmentConfig,
    )


    cs.store(name="random", node=RandomAgentConfig)

    cs.store(name="least_ambulances", node=LeastAmbulancesAgentConfig)

    cs.store(name="no_training", node=NoTrainingTrainerConfig)

    cs.store(name="drlsn", node=DRLSNTrainerConfig)

    cs.store(name="logger", node=LoggerConfig)

    cs.store(name="aggregate", node=AggregateLoggerConfig)

    cs.store(name="console", node=ConsoleLoggerConfig)

    cs.store(name="WANDB", node=WANDBLoggerConfig)

    cs.store(name="MECLP", node=MaximumExpectedCoveringLocationProblemAgentConfig)

    cs.store(name="DMECLP", node=DMECLPAgentConfig)

    cs.store(name="ERTM", node=ERTMAgentConfig)

    cs.store(
        name="base_config",
        node=Config,
    )
