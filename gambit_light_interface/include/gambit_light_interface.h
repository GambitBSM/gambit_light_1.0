#pragma once

// Report an error from C and Fortran interfaces
#ifdef __cplusplus
extern "C" 
{
#endif
    void gambit_light_invalid_point(const char*);
    void gambit_light_error(const char*);
    void gambit_light_warning(const char*);
#ifdef __cplusplus
}
#endif

// Typedefs for user-side log-likelihood and prior transform functions.
typedef double (*t_loglike_fcn_fortran)(const int, const double*, const int, double*);
typedef void (*t_prior_fcn_fortran)(const int, const double*, double*);

typedef double (*t_loglike_fcn_c)(const int, const double*, const int, double*);
typedef void (*t_prior_fcn_c)(const int, const double*, double*);

#ifdef __cplusplus
#include <vector>
#include <map>
#include <string>
typedef double (*t_loglike_fcn_cpp)(const std::vector<std::string>&, const std::vector<double>&, std::map<std::string,double>&);
typedef void (*t_prior_fcn_cpp)(const std::vector<std::string>&, const std::vector<double>&, std::vector<double>&);
#endif

#ifdef HAVE_PYBIND11
#include <pybind11/embed.h>
typedef pybind11::object* t_loglike_fcn_python;
typedef pybind11::object* t_prior_fcn_python;
#endif

// Typedefs for GAMBIT-side callbacks to register user log-likelihood and prior functions.
typedef int (*t_gambit_light_register_loglike_fcn)(const char*, void*);
typedef int (*t_gambit_light_register_prior_fcn)(void*);

// Typedefs for user-side registration functions.
typedef void (*t_fcn_loglike)(const char*, t_gambit_light_register_loglike_fcn);
typedef void (*t_fcn_prior)(t_gambit_light_register_prior_fcn);


// C++ macro for registering a user-side log-likelihood function
#ifdef __cplusplus
    #define GAMBIT_LIGHT_REGISTER_LOGLIKE(FUNC_NAME)                             \
    extern "C"                                                                   \
    void gambit_light_register_loglike_##FUNC_NAME (const char *fcn_name, t_gambit_light_register_loglike_fcn rf)  \
    {                                                                            \
        rf(fcn_name, (void*)FUNC_NAME);                                          \
    }
#endif

// C++ macro for registering a user-side prior function
#ifdef __cplusplus
    #define GAMBIT_LIGHT_REGISTER_PRIOR(FUNC_NAME)                               \
    extern "C"                                                                   \
    void gambit_light_register_prior_##FUNC_NAME (t_gambit_light_register_prior_fcn rf)  \
    {                                                                            \
        rf((void*)FUNC_NAME);                                                    \
    }
#endif
