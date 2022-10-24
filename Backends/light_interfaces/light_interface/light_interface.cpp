#include<iostream>
#include<string>
#include<map>
#include <vector>

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

            typedef enum {
                LANG_FORTRAN,
                LANG_CPP,
                LANG_C
            } t_user_like_fcn_language;

            typedef struct {
                t_user_like_fcn_language lang;
                union {
                    void *typeless_ptr;
                    user_like_fcn_fortran fortran;
                    user_like_fcn_cpp cpp;
                    user_like_fcn_c c;
                } fcn;
                std::vector<std::string> inputs;
                std::vector<std::string> outputs;
            } t_user_like_desc;

            // NOTE: init_priority attribute is needed to make sure that the map is initialized
            // when the dlopen calls the so init function. Verified to work on GCC, intel, and clang
            std::map<std::string, t_user_like_desc> user_likes __attribute__ ((init_priority (128)));

            double call_user_function(const std::map<std::string,double>& input, std::map<std::string,double>& output,
                                      const t_user_like_desc &desc)
            {
                if(desc.lang == LANG_FORTRAN ||
                   desc.lang == LANG_C){
                    double *iparams = (double*)alloca(sizeof(double)*desc.inputs.size());
                    double *oparams = (double*)alloca(sizeof(double)*desc.outputs.size());
                    double retval;

                    // translate the input to a C array
                    int iparam = 0;
                    for (auto& pname : desc.inputs) {
                        iparams[iparam++] = input.at(pname);
                    }

                    if(desc.lang == LANG_FORTRAN) retval = desc.fcn.fortran(desc.inputs.size(), iparams, desc.outputs.size(), oparams);
                    if(desc.lang == LANG_C) retval = desc.fcn.c(desc.inputs.size(), iparams, desc.outputs.size(), oparams);

                    // translate the input to a C array
                    iparam = 0;
                    for (auto& pname : desc.outputs) {
                        output[pname] = oparams[iparam++];
                    }

                    return retval;
                }

                if(desc.lang == LANG_CPP)
                    return desc.fcn.cpp(input, output);

                // unknown language
                return 0.0;
            }
        }
    }
}


extern "C"
void run(const std::map<std::string,double>& input, std::map<std::string,double>& output)
{
    std::cout << "light_interface: run" << std::endl;

    double loglike;

    // call user functions
    // TODO:
    //  - what to do when there are multiple functions?
    //  - what if there are multiple backend languages? which is first? does it matter
    using namespace Gambit::Backends::light_interface_0_1;
    for (const auto& fn : user_likes){
        loglike += call_user_function(input, output, fn.second);
    }

    // Add the expected "loglike" entry
    output["loglike"] = loglike;
}


// user-callable function to register the user likelyhood functions
extern "C"
int light_interface_register(const char *fcn_name, void *fcn)
{
    using namespace Gambit::Backends::light_interface_0_1;
    t_user_like_desc desc;
    desc.fcn.typeless_ptr = fcn;
    user_likes.insert({fcn_name, desc});
    printf("light_interface: registering function '%s'\n", fcn_name);
    return 0;
}


// light_interface module init function (currently called ad dlload time)
// TODO: in the dl constructor we can't use C++ objects, as they are not yet initialized
// at this point. For example, std::cout cannot be used, so we use C stdio.
// NOTE: BE_INI_FUNCTION function cannot be used for this, as it's called
// for each sampled point, not only once for the module
#ifdef DEBUG_MAIN
static void light_interface_init() __attribute__((constructor));
void light_interface_init() {

    using namespace Gambit::Backends::light_interface_0_1;
    YAML::Node root;
    std::string user_lib;
    std::string function_name;
    std::string init_fun;
    std::string lang;
    std::vector<std::string> inputs, outputs;

    root = YAML::LoadFile("light_interface.yaml");
    YAML::Node lightRootNode = root["LightInterface"];

    for (std::size_t fi = 0; fi < lightRootNode.size(); fi++) {
        const YAML::Node& lightNode = lightRootNode[fi];

        if (not lightNode["function_name"].IsDefined()) {
            printf("ligth_interface: could not load dynamic library: 'function_name' not specified in config file\n");
            continue;
        }
        function_name = lightNode["function_name"].as<std::string>();

        if (not lightNode["user_lib"].IsDefined()) {
            printf("ligth_interface: could not load dynamic library: 'user_lib' not specified in config file\n");
            continue;
        }
        user_lib = lightNode["user_lib"].as<std::string>();

        if (not lightNode["init_fun"].IsDefined())
            init_fun = "init_like";
        else
            init_fun = lightNode["init_fun"].as<std::string>();

        if (not lightNode["lang"].IsDefined()) {
            printf("ligth_interface: could not load dynamic library: 'lang' not specified in config file\n");
            continue;
        }
        lang = lightNode["lang"].as<std::string>();
        if (lang != "fortran" and lang != "c" and lang != "c++") {
            printf("ligth_interface: could not load dynamic library: unsupported plugin language '%s'\n", lang.c_str());
            continue;
        }

        if (not lightNode["input"].IsDefined()) {
            printf("ligth_interface: could not load dynamic library: 'input' not specified in config file\n");
            continue;
        }
        const YAML::Node& node_input = lightNode["input"];
        for (std::size_t i = 0; i < node_input.size(); i++) {
            inputs.push_back(node_input[i].as<std::string>());
        }

        if (lightNode["output"].IsDefined()) {
            const YAML::Node& node_output = lightNode["output"];
            for (std::size_t i = 0; i < node_output.size(); i++) {
                outputs.push_back(node_output[i].as<std::string>());
            }
        }

        printf("-- LIGHT CONFIG for function '%s'\n", function_name.c_str());
        printf("user_lib: %s\n", user_lib.c_str());
        printf("init_fun: %s\n", init_fun.c_str());
        printf("lang:     %s\n", lang.c_str());
        printf("-- ----- ------\n");

        // load the init symbol from the user library (should be added to the config file)
        void *handle = dlopen(user_lib.c_str(), RTLD_LAZY);
        if(!handle){
            printf("ligth_interface: could not load dynamic library: %s\n", dlerror());
            continue;
        }

        dlerror();

        char *error;
        user_init_fcn user_init_function;
        // void (*user_init_function)(void *);
        *(void**) (&user_init_function) = dlsym(handle, init_fun.c_str());

        if ((error = dlerror()) != NULL)  {
            printf("light_interface: could not load init function: %s\n", error);
            continue;
        }

        // call user init function
        (*user_init_function)(function_name.c_str(), light_interface_register);

        if(user_likes.find(function_name) != user_likes.end()) {

            // add parameter and output information to user function description
            t_user_like_desc &desc = user_likes[function_name];

            if (lang == "fortran")  desc.lang = LANG_FORTRAN;
            else if (lang == "c")   desc.lang = LANG_C;
            else if (lang == "c++") desc.lang = LANG_CPP;

            desc.inputs = std::move(inputs);
            desc.outputs = std::move(outputs);
        } else {
            printf("light_interface: expected function '%s' nas not been registered, skipping.\n", function_name.c_str());
        }
    }

    // TODO: cleanup in the destructor
    // dlclose(handle);
}

int main()
{
    std::map<std::string,double> input;
    std::map<std::string,double> output;

    input.insert({"p1", 1});
    input.insert({"p2", 1});
    input.insert({"p3", 1});
    input.insert({"p4", 1});
    input.insert({"p5", 1});
    input.insert({"p6", 1});

    run(input, output);
    std::cout << "returned loglike: " << output["loglike"] << std::endl;
}
#endif /* DEBUG_MAIN */
