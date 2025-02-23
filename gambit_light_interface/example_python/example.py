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

    # Do the prior transform
    for i,v in enumerate(input_vals):
        output[i] = v * 10.


# User-side log-likelihood function, which can be called by GAMBIT-light
def user_loglike(input_names, input_vals, output):

    # Make a dictionary of the inputs?
    input_dict = {input_names[i]: input_vals[i] for i in range(len(input_names))}

    # Error handling: Report an invalid point using gambit_light.invalid_point.
    # gambit_light.invalid_point("This input point is no good.")

    # Error handling: Report a warning using gambit_light.warning.
    gambit_light.warning("Some warning.")

    # Error handling: Report an error using gambit_light.error.
    # gambit_light.error("Some error.")

    # Error handling: Error handling, alternative to using gambit_light.error: Throw an exception.
    # raise Exception("Some exception.")

    # Compute loglike
    loglike = input_dict["param_name_1"] + input_dict["param_name_2"] + input_dict["param_name_4"]

    # Save some extra outputs
    output["py_user_loglike_output_1"] = 1
    output["py_user_loglike_output_2"] = 2
    output["py_user_loglike_output_3"] = 3

    return loglike

