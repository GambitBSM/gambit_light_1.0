#pragma once

// report an error from C and Fortran interfaces
#ifdef __cplusplus
extern "C" {
#endif
    double gambit_light_invalid_point();
    void gambit_light_error(const char *);
    void gambit_light_warning(const char *);
#ifdef __cplusplus
}
#endif

// user-side likelyhood function
typedef double (*user_like_fcn_fortran)(const int, const double *, const int, double *);
typedef double (*user_like_fcn_c)(const int, const double *, const int, double *);

#ifdef __cplusplus
#include <map>
#include <string>
typedef double (*user_like_fcn_cpp)(const std::map<std::string,double>&, std::map<std::string,double>&);
#endif

#ifdef HAVE_PYBIND11
#include <pybind11/embed.h>
typedef pybind11::object *user_like_fcn_python;
#endif

// gambit-side callback to register user likelyhood functions
typedef int (*gambit_light_register_loglike_fcn)(const char *, void *);

// user-side library initialization function
typedef void (*user_init_fcn)(const char *, gambit_light_register_loglike_fcn);

