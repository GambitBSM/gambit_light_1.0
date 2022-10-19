#include<iostream>
#include<string>
#include<map>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

#include "light_interface.h"

#define LIGHT_INTERFACE_CONFIG "light_interface.cfg"
#define USER_INIT_FUNCTION "init_like"

namespace Gambit
{
    namespace Backends
    {
        namespace light_interface
        {

            // registered functions
            // NOTE: init_priority attribute is needed to make sure that the map is initialized
            // when the dlopen calls the so init function. Verified to work on GCC, intel, and clang
            std::map<std::string, user_like_fcn> user_likes __attribute__ ((init_priority (128)));

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
                for (const auto& fn : user_likes) {
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
            int light_interface_register(const char *fcn_name, const user_like_fcn user_like)
            {
                printf("light_interface: registering user function '%s', address %p\n", fcn_name, user_like);
                user_likes.insert({fcn_name, user_like});
                return 1;
            }

            // light_interface module init function (currently called ad dlload time)
            // TODO: in the dl constructor we can't use C++ objects, as they are not yet initialized
            // at this point. For example, std::cout cannot be used, so we use C stdio.
            // NOTE: BE_INI_FUNCTION function cannot be used for this, as it's called
            // for each sampled point, not only once for the module
            static void light_interface_init() __attribute__((constructor));
            void light_interface_init() {

                // read configuration file, find user-side library, call user-side init function
                FILE *fd;
                printf("light_interface: reading config file %s\n", LIGHT_INTERFACE_CONFIG);
                fd = fopen(LIGHT_INTERFACE_CONFIG, "r");
                if(!fd){
                    printf("light_interface: could not open config file: %s\n", strerror(errno));
                    return;
                }

                char *data = (char*)alloca(4096);
                data[0] = 0;
                int nread = fread(data, 1, 4095, fd);
                if(nread>0){
                    data[nread] = 0;
                    printf("light_interface: read %d bytes: %s\n", nread, data);
                } else {
                    printf("light_interface: coud not read config file: %d: %s\n", nread, strerror(errno));
                    return;
                }

                // load the init symbol from the user library (should be added to the config file)
                void *handle = dlopen(data, RTLD_LAZY);
                if(!handle){
                    printf("ligth_interface: could not load dynamic library: %s\n", dlerror());
                    return;
                }
    
                dlerror();

                char *error;
                user_init_fcn user_init_function;
                *(void**) (&user_init_function) = dlsym(handle, USER_INIT_FUNCTION);

                if ((error = dlerror()) != NULL)  {
                    printf("light_interface: could not load init function: %s\n", error);
                    return;
                }
                printf("light_interface: calling user function %p, reservation address %p\n", 
                       user_init_function, light_interface_register);
                (*user_init_function)(light_interface_register);
                
                // TODO: cleanup in the destructor
                // dlclose(handle);
            }
        }
    }
}

#ifdef DEBUG_MAIN
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

    Gambit::Backends::light_interface::run(input, output);
    std::cout << "returned loglike: " << output["loglike"] << std::endl;
}
#endif /* DEBUG_MAIN */
