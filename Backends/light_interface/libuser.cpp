#include <stdexcept>
#include <iostream>
#include "gambit_light.h"

// user-side log-likelihood function, registered in gambit by init_user_loglike
double user_loglike(const std::map<std::string,double>& input, std::map<std::string,double>& output)
{
    std::cout << "libuser.cpp: user_loglike: computing loglike." << std::endl;
    output["oname1_cpp"] = 1;
    output["oname2_cpp"] = 2;
    output["oname3_cpp"] = 3;

    // return input.at("p1") + input.at("p2");
    
    // error handling: return a value denoting an invalid point
    // return gambit_light_invalid_point();

    // error handling: report a string warning using gambit_light_warning
    gambit_light_warning("Some warning");
    return input.at("p1") + input.at("p2");

    // error handling: report a string error using gambit_light_error
    // gambit_light_error("Invalid input parameters");
    // return gambit_light_invalid_point();

    // error handling: throw an exception
    // throw std::runtime_error("test error");
}

// user-side initialisation function, called by gambit at init
void init_user_loglike(const char *fcn_name, gambit_light_register_loglike_fcn rf)
{
    std::cout << "libuser.cpp: init_user_loglike: initialising user library." << std::endl;
    rf(fcn_name, (void*)user_loglike);
}
