#include <stdio.h>
#include "gambit_light_interface.h"


// User-side log-likelihood function, registered in GAMBIT by init_user_loglike below.
double user_loglike(const int n_inputs, const double *input, const int n_outputs, double *output)
{

    printf("example.c: user_loglike: Computing loglike.\n");

    // Error handling: Report an invalid point using gambit_light_invalid_point.
    // gambit_light_invalid_point("This input point is no good.");

    // Error handling: Report a warning using gambit_light_warning.
    // gambit_light_warning("Some warning.");

    // Error handling: Report an error using gambit_light_error.
    // gambit_light_error("Some error.");

    output[0] = 1;
    output[1] = 2;
    output[2] = 3;

    return input[0] + input[1] + input[2];
}

// User-side initialisation function, called by GAMBIT at init
void init_user_loglike(const char *fcn_name, gambit_light_register_loglike_fcn rf)
{
    printf("example.c: init_user_loglike: Registering loglike function.\n");
    rf(fcn_name, user_loglike);
}
