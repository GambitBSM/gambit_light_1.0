#pragma once

// Report an error from C and Fortran interfaces
#ifdef __cplusplus
extern "C" 
{
#endif
    void gambit_light_invalid_point(const char *);
    void gambit_light_error(const char *);
    void gambit_light_warning(const char *);
#ifdef __cplusplus
}
#endif

// Typedefs for user-side log-likelihood and prior transform functions.
typedef double (*user_loglike_fcn_fortran)(const int, const double *, const int, double *);
typedef void (*user_prior_fcn_fortran)(const int, const double *, const int, double *);

typedef double (*user_loglike_fcn_c)(const int, const double *, const int, double *);
typedef void (*user_prior_fcn_c)(const int, const double *, const int, double *);

#ifdef __cplusplus
#include <map>
#include <string>
typedef double (*user_loglike_fcn_cpp)(const std::map<std::string,double>&, std::map<std::string,double>&);
typedef void (*user_prior_fcn_cpp)(const std::map<std::string,double>&, std::map<std::string,double>&);
#endif

#ifdef HAVE_PYBIND11
#include <pybind11/embed.h>
typedef pybind11::object *user_loglike_fcn_python;
typedef pybind11::object *user_prior_fcn_python;
#endif

// Typedefs for GAMBIT-side callbacks to register user log-likelihood and prior functions.
typedef int (*gambit_light_register_loglike_fcn)(const char *, void *);
typedef int (*gambit_light_register_prior_fcn)(void *);

// Typedefs for user-side library initialisation functions.
typedef void (*user_init_fcn_loglike)(const char *, gambit_light_register_loglike_fcn);
typedef void (*user_init_fcn_prior)(gambit_light_register_prior_fcn);

