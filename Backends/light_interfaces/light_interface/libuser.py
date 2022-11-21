import light_interface

def user_like(iparams, oparams):
  print("libuser.py: user_like: computing loglike.");
  oparams['oname1_py'] = 1;
  oparams['oname2_py'] = 2;
  oparams['oname3_py'] = 3;

  return iparams['p1'] + iparams['p2'];

  # error handling: return a value denoting an invalid point
  # return light_interface.invalid_point()

  # error handling: throw an exception with 'Invalid point' string as prefix.
  # Effectively the same as return light_interface.invalid_point()
  # raise Exception("Invalid point: computations failed for these input parameters")
  
  # error handling: report a string warning using light_interface.warning
  light_interface.warning('Some warning')
  return iparams['p1'] + iparams['p2'];

  # error handling: report a string error using light_interface.error
  # light_interface.error('Invalid input parameters');
  # return light_interface.invalid_point();
  
  # error handling: throw an exception.
  # Effectively the same as using light_interface.error
  # raise Exception("Date provided can't be in the past")


def init_like(fcn_name, rf_name):
  # light_interface passes the registration function (rf) as string (rf_name)
  import importlib
  li = importlib.import_module('light_interface')
  rf = getattr(li, rf_name)
  rf(fcn_name, "user_like")
