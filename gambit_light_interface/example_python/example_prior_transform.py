# To import gambit_light_interface, first append the directory containing 
# gambit_light_interface.so to sys.path. (Alternatively, add this directory 
# to the PYTHONPATH environment variable.)
import sys
import os
current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_dir, "../lib"))
import gambit_light_interface as gambit_light


import numpy as np

def user_prior(input, output):

    print("example_prior_transform.py: user_prior: Transforming sample from unit hypercube.")

    x = np.array(list(input.values()))
    # print(f"example_prior_transform.py: input = {input}")
    # print(f"example_prior_transform.py: x = {x}")

    for k,v in input.items():
        output[k] = 10*v


def init_user_prior(rf_name):
    print("example_prior_transform.py: init_user_prior: Registering prior transform function.")
    rf = getattr(gambit_light, rf_name)
    rf("user_prior")

