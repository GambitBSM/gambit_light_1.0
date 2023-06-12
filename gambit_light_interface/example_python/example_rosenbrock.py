# To import gambit_light_interface, first append the directory containing 
# gambit_light_interface.so to sys.path. (Alternatively, add this directory 
# to the PYTHONPATH environment variable.)
import sys
import os
current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_dir, "../lib"))
import gambit_light_interface as gambit_light


import numpy as np

def rosenbrock(x, y):
    """
    @returns Rosenbrock function
    """
    a = 1.
    b = 100.
    return (a - x)**2 + b * (y - x**2)**2

def rosenbrock_general(x):
    """
    @returns Generalization of Rosenbrock function
    """
    n = len(x)
    return sum(rosenbrock(x[i], x[i+1]) for i in range(n - 1))

def loglike(x):
    """
    @returns Log-likelihood
    """
    return -rosenbrock_general(x)


def user_loglike(input_names, input_vals, output):

    print("example_rosenbrock.py: user_loglike: Computing loglike.")

    # Make a dictionary of the inputs?
    input = {input_names[i]: input_vals[i] for i in range(len(input_names))}

    x = np.array([input['param_name_1'], input['param_name_2']])

    output['py_user_loglike_rosenbrock_output_1'] = 10;

    return loglike(x)


def init_user_loglike(fcn_name, rf_name):
    print("example_rosenbrock.py: init_user_loglike: Registering loglike function.")
    rf = getattr(gambit_light, rf_name)
    rf(fcn_name, "user_loglike")
