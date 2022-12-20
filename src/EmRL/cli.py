# -*- coding: utf-8 -*-

"""Command line interface for :mod:`EmRL`.

Why does this file exist, and why not put this in ``__main__``? You might be tempted to import things from ``__main__``
later, but that will cause problems--the code will get executed twice:

- When you run ``python3 -m EmRL`` python will execute``__main__.py`` as a script.
  That means there won't be any ``EmRL.__main__`` in ``sys.modules``.
- When you import __main__ it will get executed again (as a module) because
  there's no ``EmRL.__main__`` in ``sys.modules``.

.. seealso:: https://click.palletsprojects.com/en/7.x/setuptools/#setuptools-integration
"""


__all__ = [
    "main",
]

import hydra

from EmRL.config import Config, register_configs
from EmRL.experiment import run_experiment

register_configs()


@hydra.main(version_base=None, config_path="../../config/", config_name="configuration")
def main(cfg: Config):
    """CLI for EmRL."""
    run_experiment(cfg)


if __name__ == "__main__":
    main()
