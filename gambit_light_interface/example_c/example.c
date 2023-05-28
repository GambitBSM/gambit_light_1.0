#include <stdio.h>
#include "gambit_light_interface.h"

// User-side log-likelihood function, registered in GAMBIT by init_user_loglike.
double user_loglike(const int n_inputs, const double *input, const int n_outputs, double *outputs)
{
    printf("libuser.c: user_loglike: n_inputs = %i.\n", n_inputs);
    printf("libuser.c: user_loglike: computing loglike.\n");
    if(!n_inputs) return 0.0;
    for(int i=0; i<n_outputs; i++) outputs[i] = i;

    // return input[0] + input[1];
    
    // Error handling: Report an invalid point using gambit_light_invalid_point.
    // printf("libuser.c: calling gambit_light_invalid_point.\n");
    // gambit_light_invalid_point("This input point is no good.");

    // Error handling: Report a warning using gambit_light_warning.
    // gambit_light_warning("Some warning.");

    // Error handling: Report an error using gambit_light_error.
    // gambit_light_error("Some error.");

    return input[0] + input[1] + input[2];
}

// User-side initialisation function, called by GAMBIT at init
void init_user_loglike(const char *fcn_name, gambit_light_register_loglike_fcn rf)
{
    printf("libuser.c: init_user_loglike: initialising user library.\n");
    rf(fcn_name, user_loglike);
}
