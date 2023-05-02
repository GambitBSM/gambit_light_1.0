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

  x = np.array([iparams['p1'], iparams['p2']])
  oparams['rosenbrock_oname1_py'] = 10;
  result = loglike(x)
  return result

def init_user_loglike(fcn_name, rf_name):
  # gambit_light passes the registration function (rf) as string (rf_name)
  import importlib
  li = importlib.import_module('gambit_light')
  rf = getattr(li, rf_name)
  rf(fcn_name, "user_loglike")
