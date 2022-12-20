"""Module for loggers."""
import os
from abc import ABC, abstractmethod
from typing import Any, Callable, Dict, Optional, Tuple

from hydra.utils import to_absolute_path
from omegaconf import OmegaConf

from EmRL.config import Config



class Logger(ABC):
    """Abstract base class for loggers."""

    def __init__(self, cfg: Config):
        """:param cfg: Configuration file."""

    @abstractmethod
    def log_metrics(
        self,
        metrics: Dict[str, Any],
        prefix: Optional[str] = None,
        step: Optional[int] = None,
        epoch: Optional[int] = None,
    ):
        """
        Log a dictionary of metrics in the current step.

        :param epoch: current epoch
        :param step: current step
        :param prefix: groups certain metrics together
        :param metrics: the dictionary containing the metrics.
        """

    @abstractmethod
    def close(self):
        """Tell logger to save all data."""

    @abstractmethod
    def save_data(
        self,
        epoch: int,
        env_step: int,
        gradient_step: int,
        save_checkpoint_fn: Optional[Callable[[int, int, int], str]] = None,
    ) -> None:
        """
        Use writer to log metadata when calling ``save_checkpoint_fn`` in trainer.

        :param epoch: the epoch in trainer.
        :param env_step: the env_step in trainer.
        :param gradient_step: the gradient_step in trainer.
        :param save_checkpoint_fn: a hook defined by user, see trainer documentation for detail.
        """
        pass

    @abstractmethod
    def restore_data(self) -> Tuple[int, int, int]:
        """
        Restore data to resume a run.

        Returns:
            Tuple[int, int, int]: epoch, env_step, gradient_step
        """
        pass
    
    @abstractmethod
    def watch_model(self, models, criterion=None, log="gradients", log_freq=100, idx=None, log_graph=False) -> None:
        """Watch model parameters and gradients.

        :param log_freq: log every N batches
        """

class WANDBLogger(Logger):
    """Log to WANDB."""

    def __init__(
        self, cfg: Config, entity: str, project: str, offline: bool = False, *args, **kwargs
    ):
        """
        Initialize the logger, make sure to run wandb login on the machine when using the first time.

        :param cfg: configuration
        :param entity: WANDB entity
        :param project: WANDB project
        :param offline: Log only offline opr to WANDB db.
        :param args: additional args
        :param kwargs: additional args
        """
        super().__init__(cfg)
        import wandb

        # todo implement resume...

        self.last_save_step = -1
        self.save_interval = 250  # todo do not hardcode
        agent_name = cfg.agent.name
        number_of_ambulances_tag = f"ambu_{cfg.environment_config.number_of_ambulances}"

        self.run = wandb.init(
            entity=entity,
            project=project,
            config=OmegaConf.to_container(cfg, resolve=True, throw_on_missing=True),
            mode="offline" if offline or cfg.debug else "online",
            tags=[agent_name, number_of_ambulances_tag, cfg.experiment_name, cfg.environment_config.name, "new"],
        )  # todo tags, group, ...

        # log the code used for the current run
        #todo enable again
        #self.run.log_code(to_absolute_path("src/EmRL")) 

        # todo implement logging of additional summary metrics
        self.run.define_metric("val/average_response_time_avg", summary="min")
        self.run.define_metric("test/average_response_time_avg", summary="min")

    # docstr-coverage:inherited
    def log_metrics(  # noqa D102
        self,
        metrics: Dict[str, Any],
        prefix: Optional[str] = None,
        step: Optional[int] = None,
        epoch: Optional[int] = None,
    ):
        if prefix is not None:
            metrics = {f"{prefix}/{key}": value for key, value in metrics.items()}

        if epoch is not None:
            metrics["epoch"] = epoch

        self.run.log(metrics, step=step)

    # docstr-coverage:inherited
    def close(self):
        """Cf base class."""
        self.run.finish()

    # docstr-coverage:inherited
    def save_data(  # noqa D102
        self,
        epoch: int,
        env_step: int,
        gradient_step: int,
        save_checkpoint_fn: Optional[Callable[[int, int, int], str]] = None,
    ) -> None:
        if save_checkpoint_fn and epoch - self.last_save_step >= self.save_interval:
            import wandb

            self.last_save_step = epoch
            checkpoint_path = save_checkpoint_fn(epoch, env_step, gradient_step)

            checkpoint_artifact = wandb.Artifact(
                "run_" + self.run.id + "_checkpoint",  # type: ignore
                type="model",
                metadata={
                    "save/epoch": epoch,
                    "save/env_step": env_step,
                    "save/gradient_step": gradient_step,
                    "checkpoint_path": str(checkpoint_path),
                },
            )
            checkpoint_artifact.add_file(str(checkpoint_path))
            self.run.log_artifact(checkpoint_artifact)  # type: ignore

    # docstr-coverage:inherited
    def restore_data(self) -> Tuple[int, int, int]:  # noqa D102
        checkpoint_artifact = self.run.use_artifact(  # type: ignore
            f"run_{self.run.id}_checkpoint:latest"  # type: ignore
        )
        assert checkpoint_artifact is not None, "W&B dataset artifact doesn't exist"

        checkpoint_artifact.download(
            os.path.dirname(checkpoint_artifact.metadata["checkpoint_path"])
        )

        try:  # epoch / gradient_step
            epoch = checkpoint_artifact.metadata["save/epoch"]
            self.last_save_step = self.last_log_test_step = epoch
            gradient_step = checkpoint_artifact.metadata["save/gradient_step"]
            self.last_log_update_step = gradient_step
        except KeyError:
            epoch, gradient_step = 0, 0
        try:  # offline trainer doesn't have env_step
            env_step = checkpoint_artifact.metadata["save/env_step"]
            self.last_log_train_step = env_step
        except KeyError:
            env_step = 0
        return epoch, env_step, gradient_step

    
    # docstr-coverage:inherited
    def watch_model(self, models, criterion=None, log="gradients", log_freq=100, idx=None, log_graph=False) -> None: # noqa D102
        return self.run.watch(models, criterion, log, log_freq, idx, log_graph)

class AggregateLogger(Logger):
    """Logger that allows the usage of multiple other loggers."""

    def __init__(self, cfg: Config, logger_cfg: Any, *args, **kwargs):
        """
        Initialize the aggregate logger.

        :param cfg: config
        :param logger_cfg: configuration for the sub loggers
        :param args: various params
        :param kwargs: various params
        """
        super().__init__(cfg)
        from hydra.utils import instantiate

        self._loggers = [
            instantiate(single_logger_cfg, cfg=cfg) for single_logger_cfg in logger_cfg.values()
        ]

    # docstr-coverage:inherited
    def log_metrics(  # noqa D102
        self,
        metrics: Dict[str, Any],
        prefix: Optional[str] = None,
        step: Optional[int] = None,
        epoch: Optional[int] = None,
    ):
        for logger in self._loggers:
            logger.log_metrics(metrics, prefix, step, epoch)

    # docstr-coverage:inherited
    def close(self):
        """Cf base."""
        for logger in self._loggers:
            logger.close()

    # docstr-coverage:inherited
    def restore_data(self) -> Tuple[int, int, int]:  # noqa D102
        pass

    # docstr-coverage:inherited
    def save_data(  # noqa D102
        self,
        epoch: int,
        env_step: int,
        gradient_step: int,
        save_checkpoint_fn: Optional[Callable[[int, int, int], str]] = None,
    ) -> None:
        pass
    
    
    # docstr-coverage:inherited
    def watch_model(self, models, criterion=None, log="gradients", log_freq=100, idx=None, log_graph=False) -> None: # noqa D102
        pass #todo

class ConsoleLogger(Logger):
    """Log to console."""

    def __init__(self, cfg: Config, *args, **kwargs):
        """
        Init console logger.

        :param cfg: cf base
        :param args: various params
        :param kwargs: various params
        """
        super().__init__(cfg)

    # docstr-coverage:inherited
    def log_metrics(  # noqa D102
        self,
        metrics: Dict[str, Any],
        prefix: Optional[str] = None,
        step: Optional[int] = None,
        epoch: Optional[int] = None,
    ):
        print(f"{prefix} @ {step}; {epoch}: {metrics}")  # noqa: T201

    # docstr-coverage:inherited
    def close(self):  # noqa D102
        pass

    # docstr-coverage:inherited
    def restore_data(self) -> Tuple[int, int, int]:  # noqa D102
        pass

    # docstr-coverage:inherited
    def save_data(  # noqa D102
        self,
        epoch: int,
        env_step: int,
        gradient_step: int,
        save_checkpoint_fn: Optional[Callable[[int, int, int], str]] = None,
    ) -> None:
        pass
    
    # docstr-coverage:inherited
    def watch_model(self, models, criterion=None, log="gradients", log_freq=100, idx=None, log_graph=False) -> None: # noqa D102
        pass
