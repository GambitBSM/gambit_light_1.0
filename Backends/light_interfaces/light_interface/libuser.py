def user_like(iparams, oparams):
  print("libuser.py: user_like: computing loglike.");
  oparams['oname1_py'] = 1;
  oparams['oname2_py'] = 2;
  oparams['oname3_py'] = 3;
  return iparams['p1'] + iparams['p2'];

  # error handling: throw an exception
  # raise Exception("Date provided can't be in the past")

def init_like(fcn_name, rf_name):
  # light_interface passes the registration function (rf) as string (rf_name)
  import importlib
  li = importlib.import_module('light_interface')
  rf = getattr(li, rf_name)
  rf(fcn_name, "user_like")
