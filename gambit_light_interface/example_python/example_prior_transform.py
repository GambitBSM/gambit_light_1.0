# To import gambit_light_interface, first append the directory containing 
# gambit_light_interface.so to sys.path. (Alternatively, add this directory 
# to the PYTHONPATH environment variable.)
import sys
import os
current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_dir, "../lib"))
import gambit_light_interface as gambit_light


# User-side prior transform function, which can be called by GAMBIT-light.
def user_prior(input_names, input_vals, output):

    print("example_prior_transform.py: user_prior: Transforming sample from unit hypercube.")

    for i,v in enumerate(input_vals):
        output[i] = v * 10.
