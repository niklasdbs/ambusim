#!/usr/bin/env bash
#
#SBATCH --job-name dmeclp
#SBATCH --time=72:00:00
#SBATCH --gres=gpu:0
#SBATCH --cpus-per-task=4
#SBATCH --output=/dev/null
#SBATCH  --error=dmeclp.err


source venv/bin/activate
#install the pip package before because gcc may not be installed as well as some necessary libraries
#pip install .
#make sure to install torch with the right cuda version for some of the gpus
#pip install torch torchvision torchaudio --extra-index-url https://download.pytorch.org/whl/cu113
export LD_LIBRARY_PATH=~/bin #copy cnpy library so file to this directory
export HYDRA_FULL_ERROR=1
python -m EmRL -m +experiment=dmeclp environment_config=sf agent.object.busy_fraction='range(0.05, 0.95, step=0.05)'
