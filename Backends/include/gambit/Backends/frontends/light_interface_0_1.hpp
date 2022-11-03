//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  GAMBIT_light interface library (C/C++, Fortra, Python backends)
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \author Marcin Krotkiewski
///          (marcin.krotkiewski@usit.uio.no)
///  \date 2022 Sep
///
///  *********************************************


#define BACKENDNAME light_interface
#define BACKENDLANG CXX
#define VERSION 0.1
#define SAFE_VERSION 0_1
#define REFERENCE GAMBIT:2017yxo

#include "gambit/Utils/util_types.hpp"
// #include "gambit/Core/yaml_parser.hpp"

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
BE_FUNCTION(lightLibrary_C_CXX_Fortran, void, (const std::string &, const std::string &, const std::string &, const std::string &, std::vector<std::string> &, std::vector<std::string> &), "lightLibrary_C_CXX_Fortran", "lightLibrary_C_CXX_Fortran")
BE_FUNCTION(lightLibrary_Python, void, (const std::string &, const std::string &, const std::string &, const std::string &, std::vector<std::string> &, std::vector<std::string> &), "lightLibrary_Python", "lightLibrary_Python")


/* Syntax for BE_VARIABLE:
 * BE_VARIABLE([name], [type], "[exact symbol name]", "[choose capability name]")
 * */

// Convenience functions (registration)
// BE_CONV_FUNCTION(light_interface_init, void, (), "light_interface_init")


// BE_NAMESPACE
// {
// #include <stdio.h>
// #include <errno.h>
// #include <string.h>
// #include <dlfcn.h>

//     static bool _so_initialized = false;

//     void light_interface_init() {

//         using namespace Gambit::Backends::light_interface_0_1;
//         YAML::Node root;
//         std::string user_lib;
//         std::string function_name;
//         std::string init_fun;
//         std::string lang;
//         std::vector<std::string> inputs, outputs;

//         // root = YAML::LoadFile("light_interface.yaml");
//         // YAML::Node lightRootNode = root["LightInterface"];

//         YAML::Node lightRootNode = runOptions->getNode("LightInterface");

//         for (std::size_t fi = 0; fi < lightRootNode.size(); fi++) {
//             const YAML::Node& lightNode = lightRootNode[fi];

//             if (not lightNode["function_name"].IsDefined()) {
//                 backend_error().raise(LOCAL_INFO, "ligth_interface: could not load dynamic library: 'function_name' not specified in config file");
//             }
//             function_name = lightNode["function_name"].as<std::string>();

//             if (not lightNode["user_lib"].IsDefined()) {
//                 backend_error().raise(LOCAL_INFO, "ligth_interface: could not load dynamic library: 'user_lib' not specified in config file");
//             }
//             user_lib = lightNode["user_lib"].as<std::string>();

//             if (not lightNode["init_fun"].IsDefined())
//                 init_fun = "init_like";
//             else
//                 init_fun = lightNode["init_fun"].as<std::string>();

//             if (not lightNode["lang"].IsDefined()) {
//                 backend_error().raise(LOCAL_INFO, "ligth_interface: could not load dynamic library: 'lang' not specified in config file");
//             }
//             lang = lightNode["lang"].as<std::string>();
//             if (lang != "fortran" and
//                 lang != "c" and
// #ifdef HAVE_PYBIND11
//                 lang != "python" and
// #endif
//                 lang != "c++") {
//                 backend_error().raise(LOCAL_INFO, "ligth_interface: could not load dynamic library: unsupported plugin language '" + lang + "'");
//                 continue;
//             }

//             if (not lightNode["input"].IsDefined()) {
//                 backend_error().raise(LOCAL_INFO, "ligth_interface: could not load dynamic library: 'input' not specified in config file");
//                 continue;
//             }
//             const YAML::Node& node_input = lightNode["input"];
//             for (std::size_t i = 0; i < node_input.size(); i++) {
//                 inputs.push_back(node_input[i].as<std::string>());
//             }

//             if (lightNode["output"].IsDefined()) {
//                 const YAML::Node& node_output = lightNode["output"];
//                 for (std::size_t i = 0; i < node_output.size(); i++) {
//                     outputs.push_back(node_output[i].as<std::string>());
//                 }
//             }

//             printf("-- LIGHT CONFIG for function '%s'\n", function_name.c_str());
//             printf("user_lib: %s\n", user_lib.c_str());
//             printf("init_fun: %s\n", init_fun.c_str());
//             printf("lang:     %s\n", lang.c_str());
//             printf("-- ----- ------\n");

//             if (lang == "c" or
//                 lang == "c++" or
//                 lang == "fortran")
//                 lightLibrary_C_CXX_Fortran(user_lib, init_fun, lang, function_name, inputs, outputs);

// #ifdef HAVE_PYBIND11
//             if (lang == "python")
//                 lightLibrary_Python(user_lib, init_fun, lang, function_name, inputs, outputs);
// #endif
//         }

//         // TODO: cleanup in the destructor
//         // dlclose(handle);
//     }
// }
// END_BE_NAMESPACE

// // this is called for every point, not once for backend
// BE_INI_FUNCTION 
// {
//     if(_so_initialized) return;
//     light_interface_init();
//     _so_initialized = true;
// }
// END_BE_INI_FUNCTION

// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
