# Introduction
A high-performance dynamic ambulance redeployment simulator.
# Data
Download the ambulance events from https://data.sfgov.org/Public-Safety/Fire-Department-Calls-for-Service/nuek-vuh3. Process this file and download the road network using data_exploration_sf notebook.
# Build
`pip install .`
This will automatically build the C++ module and python module.
For debug build set DEBUG=1.
# Run
`python -m EmRL +experiment=experiment_name environment_config=sf`
