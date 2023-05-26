#include <stdio.h>
#include "gambit_light_interface.h"

// User-side log-likelihood function, registered in GAMBIT by init_user_loglike.
double user_loglike(const int niparams, const double *iparams, const int noparams, double *oparams)
{
    printf("libuser.c: user_loglike: niparams = %i.\n", niparams);
    printf("libuser.c: user_loglike: computing loglike.\n");
    if(!niparams) return 0.0;
    for(int i=0; i<noparams; i++) oparams[i] = i;

    // return iparams[0] + iparams[1];
    
    // Error handling: Report an invalid point using gambit_light_invalid_point.
    // printf("libuser.c: calling gambit_light_invalid_point.\n");
    // gambit_light_invalid_point("This input point is no good.");

    // Error handling: Report a warning using gambit_light_warning.
    gambit_light_warning("Some warning.");

    // Error handling: Report an error using gambit_light_error.
    // gambit_light_error("Some error.");

    return iparams[0] + iparams[1] + iparams[2];
}

// User-side initialisation function, called by GAMBIT at init
void init_user_loglike(const char *fcn_name, gambit_light_register_loglike_fcn rf)
{
    printf("libuser.c: init_user_loglike: initialising user library.\n");
    rf(fcn_name, user_loglike);
}
