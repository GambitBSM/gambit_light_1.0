#include <stdexcept>
#include <iostream>
#include "gambit_light_interface.h"


// User-side log-likelihood function, registered in GAMBIT by init_user_loglike below.
double user_loglike(const std::map<std::string,double>& input, std::map<std::string,double>& output)
{

    std::cout << "example.cpp: user_loglike: Computing loglike." << std::endl;

    // Error handling: Report an invalid point using gambit_light_invalid_point.
    // gambit_light_invalid_point("This input point is no good.");

    // Error handling: Report a string warning using gambit_light_warning.
    // gambit_light_warning("Some warning.");

    // Error handling: Report an error using gambit_light_error.
    // gambit_light_error("Some error.");

    // Error handling, alternative to using gambit_light_error: Throw a runtime_error.
    // throw std::runtime_error("Some runtime_error.");

    output["cpp_user_loglike_output_1"] = 1;
    output["cpp_user_loglike_output_2"] = 2;
    output["cpp_user_loglike_output_3"] = 3;

    return input.at("param_name_2") + input.at("param_name_3");
}


// User-side initialisation function, called by GAMBIT.
extern "C"
void init_user_loglike(const char *fcn_name, gambit_light_register_loglike_fcn rf)
{
    std::cout << "example.cpp: init_user_loglike: Registering loglike function." << std::endl;
    rf(fcn_name, (void*)user_loglike);
}
