//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  GAMBIT_light interface library for C++ codes
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date 2022 Sep
///
///  *********************************************


#define BACKENDNAME light_interface
#define BACKENDLANG CXX
#define VERSION 0.1
#define SAFE_VERSION 0_1
#define REFERENCE GAMBIT:2017yxo

#include "gambit/Utils/util_types.hpp"
#include "gambit/Core/yaml_parser.hpp"

/* The following macro loads the library using dlopen
 * when this header file is included somewhere. */

LOAD_LIBRARY

/* Next we use macros BE_VARIABLE and BE_FUNCTION to load pointers
 * (using dlsym) to the variables and functions within the library.
 *
 * The macros also create functors that wrap the library variables and functions.
 * These are used by the Core for dependency resolution and to set up a suitable
 * interface to the library functions/variables at module level. */

/* Syntax for BE_FUNCTION:
 * BE_FUNCTION([choose function name], [type], [arguement types], "[exact symbol name]", "[choose capability name]")
 */

BE_FUNCTION(run, void, (const map_str_dbl &, map_str_dbl &), "run", "run_light_interface")
BE_FUNCTION(light_interface_register, int, (const char *, const void *), "light_interface_register", "light_interface_register")

/* Syntax for BE_VARIABLE:
 * BE_VARIABLE([name], [type], "[exact symbol name]", "[choose capability name]")
 * */

BE_NAMESPACE
{
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

    static bool _so_initialized = false;

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
} 
END_BE_NAMESPACE

// this is called for every point, not once for backend
BE_INI_FUNCTION 
{
    if(_so_initialized) return;
    light_interface_init();
    _so_initialized = true;
}
END_BE_INI_FUNCTION

// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
