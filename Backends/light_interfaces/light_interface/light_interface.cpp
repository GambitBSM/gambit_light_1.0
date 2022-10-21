#include<iostream>
#include<string>
#include<map>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

// TODO: this is boule-defined in light_interface_0_1.hpp
// once fihished, we should probably move the contents of light_interface.cpp to light_interface_0_1.cpp
#include "light_interface.h"

#ifdef DEBUG_MAIN
#include "yaml-cpp/yaml.h"
#endif

// registered functions
namespace Gambit {
    namespace Backends {
        namespace light_interface_0_1 {
            // NOTE: init_priority attribute is needed to make sure that the map is initialized
            // when the dlopen calls the so init function. Verified to work on GCC, intel, and clang
            std::map<std::string, user_like_fcn> user_likes __attribute__ ((init_priority (128)));
        }
    }
}


extern "C"
void run(const std::map<std::string,double>& input, std::map<std::string,double>& output)
{
    std::cout << "light_interface: run" << std::endl;

    int nparams = input.size(), iparam;
    double *iparams = (double*)malloc(sizeof(double)*nparams);
    double *oparams = (double*)malloc(sizeof(double)*nparams);
    double loglike;

    // translate the input to a C array
    iparam = 0;
    for (const auto& kv : input) {
        iparams[iparam++] = kv.second;
    }

    // call user functions
    // TODO: what to do when there are multiple functions?
    for (const auto& fn : Gambit::Backends::light_interface_0_1::user_likes) {
        loglike = fn.second(input.size(), iparams, oparams);
    }

    // translate the output back to a map
    iparam = 0;
    for (auto& kv : input) {
        output[kv.first] = oparams[iparam++];
    }
                
    // cleanup
    free(iparams);
    free(oparams);
                
    // Add the expected "loglike" entry
    output["loglike"] = loglike;

}


// user-callable function to register the user likelyhood functions
extern "C"
int light_interface_register(const char *fcn_name, const void *arg)
{
    // TODO: remove this cast with proper GAMBIT type definitions
    const user_like_fcn user_like = (const user_like_fcn)(arg);
    printf("light_interface: registering user function '%s', address %p\n", fcn_name, user_like);
    Gambit::Backends::light_interface_0_1::user_likes.insert({fcn_name, user_like});
    return 1;
}


// light_interface module init function (currently called ad dlload time)
// TODO: in the dl constructor we can't use C++ objects, as they are not yet initialized
// at this point. For example, std::cout cannot be used, so we use C stdio.
// NOTE: BE_INI_FUNCTION function cannot be used for this, as it's called
// for each sampled point, not only once for the module
#ifdef DEBUG_MAIN
static void light_interface_init() __attribute__((constructor));
void light_interface_init() {

    YAML::Node root;
    std::string user_lib;
    std::string init_fun;
    std::string lang;

    root = YAML::LoadFile("light_interface.yaml");
    YAML::Node lightNode = root["LightInterface"];
    
    if (not lightNode["user_lib"].IsDefined()) {
        printf("ligth_interface: could not load dynamic library: 'user_lib' not specified in config file\n");
        return;
    }
    user_lib = lightNode["user_lib"].as<std::string>();

    if (not lightNode["init_fun"].IsDefined())
        init_fun = "init_like";
    else
        init_fun = lightNode["init_fun"].as<std::string>();

    if (not lightNode["lang"].IsDefined()) {
        printf("ligth_interface: could not load dynamic library: 'lang' not specified in config file\n");
        return;
    }
    lang = lightNode["lang"].as<std::string>();

    printf("-- LIGHT CONFIG\n");
    printf("user_lib: %s\n", user_lib.c_str());
    printf("init_fun: %s\n", init_fun.c_str());
    printf("lang:     %s\n", lang.c_str());
    printf("-- ----- ------\n");

    // load the init symbol from the user library (should be added to the config file)
    void *handle = dlopen(user_lib.c_str(), RTLD_LAZY);
    if(!handle){
        printf("ligth_interface: could not load dynamic library: %s\n", dlerror());
        return;
    }
    
    dlerror();

    char *error;
    void (*user_init_function)(void *);
    *(void**) (&user_init_function) = dlsym(handle, init_fun.c_str());

    if ((error = dlerror()) != NULL)  {
        printf("light_interface: could not load init function: %s\n", error);
        return;
    }

    // call user init function
    (*user_init_function)((void*)light_interface_register);
                
    // TODO: cleanup in the destructor
    // dlclose(handle);
}

int main()
{
    std::map<std::string,double> input;
    std::map<std::string,double> output;

    input.insert({"p1", 1});
    input.insert({"p2", 2});
    input.insert({"p3", 3});

    output.insert({"p1", 0});
    output.insert({"p2", 0});
    output.insert({"p3", 0});

    run(input, output);
    std::cout << "returned loglike: " << output["loglike"] << std::endl;
}
#endif /* DEBUG_MAIN */
