#include <stdexcept>
#include <iostream>
#include "gambit_light.h"

// User-side log-likelihood function, registered in GAMBIT by init_user_loglike.
double user_loglike(const std::map<std::string,double>& input, std::map<std::string,double>& output)
{
    std::cout << "libuser.cpp: user_loglike: computing loglike." << std::endl;
    output["cpp_user_loglike_output_1"] = 1;
    output["cpp_user_loglike_output_2"] = 2;
    output["cpp_user_loglike_output_3"] = 3;

    // return input.at("p1") + input.at("p2");
    
    // Error handling: Return a value denoting an invalid point.
    // return gambit_light_invalid_point();

    // Error handling: Report a string warning using gambit_light_warning.
    gambit_light_warning("Some warning");
    return input.at("param_name_2") + input.at("param_name_3");

    // Error handling: Report a string error using gambit_light_error.
    // gambit_light_error("Invalid input parameters");
    // return gambit_light_invalid_point();

    // Error handling: Throw an exception.
    // throw std::runtime_error("test error");
}

// User-side initialisation function, called by GAMBIT at init.
void init_user_loglike(const char *fcn_name, gambit_light_register_loglike_fcn rf)
{
    std::cout << "libuser.cpp: init_user_loglike: initialising user library." << std::endl;
    rf(fcn_name, (void*)user_loglike);
}
