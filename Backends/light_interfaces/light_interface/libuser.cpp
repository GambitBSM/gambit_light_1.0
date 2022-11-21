#include <stdexcept>
#include <iostream>
#include "light_interface.h"

// user-side likelihood function, registered in gambit by init_like
double user_like(const std::map<std::string,double>& input, std::map<std::string,double>& output)
{
    std::cout << "libuser.cpp: user_like: computing loglike." << std::endl;
    output["oname1_cpp"] = 1;
    output["oname2_cpp"] = 2;
    output["oname3_cpp"] = 3;
    return input.at("p1") + input.at("p2");
    
    // error handling: return a value denoting an invalid point
    // return light_interface_invalid_point();

    // error handling: report a string warning using light_interface_warning
    // light_interface_warning("Some warning");
    // return input.at("p1") + input.at("p2");

    // error handling: report a string error using light_interface_error
    // light_interface_error("Invalid input parameters");
    // return light_interface_invalid_point();

    // error handling: throw an exception
    // throw std::runtime_error("test error");
}

// user-side initialization function, called by gambit at init
void init_like(const char *fcn_name, light_interface_register_fcn rf)
{
    printf("libuser.cpp: init_like: initializing user library.\n");
    rf(fcn_name, (void*)user_like);
}
