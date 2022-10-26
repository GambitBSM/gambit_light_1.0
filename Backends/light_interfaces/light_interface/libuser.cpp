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
}

// user-side initialization function, called by gambit at init
void init_like(const char *fcn_name, light_interface_register_fcn rf)
{
    printf("libuser.cpp: init_like: initializing user library.\n");
    rf(fcn_name, (void*)user_like);
}
