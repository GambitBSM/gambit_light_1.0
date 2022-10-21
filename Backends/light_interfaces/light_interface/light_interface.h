#pragma once

// user-side likelyhood function
typedef double (*user_like_fcn)(const int, const double *, double *);

// gambit-side function to register user likelyhood functions
typedef int (*light_interface_register_fcn)(const char *, const void *);

// user-side library initialization function
typedef void (*user_init_fcn)(light_interface_register_fcn);

