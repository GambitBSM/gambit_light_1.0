#include <stdio.h>
#include "light_interface.h"

// user-side likelihood function, registered in gambit by init_like
double user_like(const int nparams, const double *iparams, double *oparams)
{
    printf("libuser.c: user_like: computing loglike.\n");
    if(!nparams) return 0.0;

    for(int pi=0; pi<nparams; pi++){
        oparams[pi] = iparams[pi] + 1;
    }

    return iparams[0] + iparams[11];
}

// user-side initialization function, called by gambit at init
void init_like(light_interface_register_fcn rf)
{
    printf("libuser.c: init_like: initializing user library.\n");
    rf("cuser_like", user_like);
}
