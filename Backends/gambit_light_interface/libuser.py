import gambit_light

def user_loglike(iparams, oparams):
  print("libuser.py: user_loglike: computing loglike.");
  oparams['py_user_loglike_output_1'] = 1;
  oparams['py_user_loglike_output_2'] = 2;
  oparams['py_user_loglike_output_3'] = 3;

  print("libuser.py: got iparams:", iparams)

  # return iparams['p1'] + iparams['p2'];

  # Error handling: Return a value denoting an invalid point.
  # return gambit_light.invalid_point()

  # Error handling: Throw an exception with 'Invalid point' string as prefix.
  # Effectively the same as return gambit_light.invalid_point()
  # raise Exception("Invalid point: computations failed for these input parameters")
  
  # Error handling: Report a string warning using gambit_light.warning.
  gambit_light.warning('Some warning')
  return iparams['param_name_1'] + iparams['param_name_2'] + iparams['param_name_4'];

  # Error handling: Report a string error using gambit_light.error.
  # gambit_light.error('Invalid input parameters');
  # return gambit_light.invalid_point();
  
  # Error handling: Throw an exception.
  # Effectively the same as using gambit_light.error
  # raise Exception("Date provided can't be in the past")


def init_user_loglike(fcn_name, rf_name):
  # gambit_light passes the registration function (rf) as string (rf_name).
  import importlib
  li = importlib.import_module('gambit_light')
  rf = getattr(li, rf_name)
  rf(fcn_name, "user_loglike")
