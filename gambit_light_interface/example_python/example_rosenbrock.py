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


def user_loglike(iparams, oparams):
  print("libuser_rosenbrock.py: user_loglike: computing loglike.");

  x = np.array([iparams['param_name_1'], iparams['param_name_2']])
  oparams['py_user_loglike_rosenbrock_output_1'] = 10;
  result = loglike(x)
  return result

def init_user_loglike(fcn_name, rf_name):
  # gambit_light passes the registration function (rf) as string (rf_name)
  rf = getattr(gambit_light, rf_name)
  rf(fcn_name, "user_loglike")
