#pragma once

// user-side likelyhood function
typedef double (*user_like_fcn_fortran)(const int, const double *, const int, double *);
typedef double (*user_like_fcn_c)(const int, const double *, const int, double *);

#ifdef __cplusplus
#include <map>
#include <string>
typedef double (*user_like_fcn_cpp)(const std::map<std::string,double>&, std::map<std::string,double>&);
#endif

// gambit-side function to register user likelyhood functions
typedef int (*light_interface_register_fcn)(const char *, void *);

// user-side library initialization function
typedef void (*user_init_fcn)(const char *, light_interface_register_fcn);

