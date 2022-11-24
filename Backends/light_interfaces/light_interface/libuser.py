import gambit_light

def user_like(iparams, oparams):
  print("libuser.py: user_like: computing loglike.");
  oparams['oname1_py'] = 1;
  oparams['oname2_py'] = 2;
  oparams['oname3_py'] = 3;

  return iparams['p1'] + iparams['p2'];

  # error handling: return a value denoting an invalid point
  # return gambit_light.invalid_point()

  # error handling: throw an exception with 'Invalid point' string as prefix.
  # Effectively the same as return gambit_light.invalid_point()
  # raise Exception("Invalid point: computations failed for these input parameters")
  
  # error handling: report a string warning using gambit_light.warning
  # gambit_light.warning('Some warning')
  # return iparams['p1'] + iparams['p2'];

  # error handling: report a string error using gambit_light.error
  # gambit_light.error('Invalid input parameters');
  # return gambit_light.invalid_point();
  
  # error handling: throw an exception.
  # Effectively the same as using gambit_light.error
  # raise Exception("Date provided can't be in the past")


def init_like(fcn_name, rf_name):
  # gambit_light passes the registration function (rf) as string (rf_name)
  import importlib
  li = importlib.import_module('gambit_light')
  rf = getattr(li, rf_name)
  rf(fcn_name, "user_like")
