#include <stdio.h>
#include "gambit_light.h"

// user-side likelihood function, registered in gambit by init_like
double user_like(const int niparams, const double *iparams, const int noparams, double *oparams)
{
    printf("libuser.c: user_like: computing loglike.\n");
    if(!niparams) return 0.0;
    for(int i=0; i<noparams; i++) oparams[i] = i;
    return iparams[0] + iparams[1];
    
    // error handling: return a value denoting an invalid point
    // return gambit_light_invalid_point();

    // error handling: report a string warning using gambit_light_warning
    // gambit_light_warning("Some warning");
    // return iparams[0] + iparams[1];

    // error handling: report a string error using gambit_light_error
    // gambit_light_error("Invalid input parameters");
    // return gambit_light_invalid_point();
}

// user-side initialization function, called by gambit at init
void init_like(const char *fcn_name, gambit_light_register_loglike_fcn rf)
{
    printf("libuser.c: init_like: initializing user library.\n");
    rf(fcn_name, user_like);
}
