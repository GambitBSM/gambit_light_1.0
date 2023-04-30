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


#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/light_interface_0_1.hpp"

// Initialisation function. This is called for every point.
BE_INI_FUNCTION
{
    static bool _so_initialized = false;
    if(_so_initialized) return;

    std::string user_lib;
    std::string function_name;
    std::string init_fun;
    std::string lang;
    std::vector<std::string> inputs, outputs;

    YAML::Node lightRootNode = runOptions->getNode("LightInterface");

    for (std::size_t fi = 0; fi < lightRootNode.size(); fi++)
    {
        const YAML::Node& lightNode = lightRootNode[fi];

        if (not lightNode["function_name"].IsDefined())
        {
            backend_error().raise(LOCAL_INFO, "light_interface: Could not load dynamic library: 'function_name' not specified in config file.");
        }
        function_name = lightNode["function_name"].as<std::string>();

        if (not lightNode["user_lib"].IsDefined())
        {
            backend_error().raise(LOCAL_INFO, "light_interface: Could not load dynamic library: 'user_lib' not specified in config file.");
        }
        user_lib = lightNode["user_lib"].as<std::string>();

        if (not lightNode["init_fun"].IsDefined())
        {
            init_fun = "init_like";
        }
        else 
        {
            init_fun = lightNode["init_fun"].as<std::string>();
        }

        if (not lightNode["lang"].IsDefined())
        {
            backend_error().raise(LOCAL_INFO, "light_interface: Could not load dynamic library: 'lang' not specified in config file.");
        }
        lang = lightNode["lang"].as<std::string>();

        if (lang != "fortran" and 
            lang != "c" and
#ifdef HAVE_PYBIND11
            lang != "python" and
#endif
            lang != "c++")
        {
            backend_error().raise(LOCAL_INFO, "light_interface: Could not load dynamic library: unsupported plugin language '" + lang + "'.");
            continue;
        }

        if (not lightNode["input"].IsDefined())
        {
            backend_error().raise(LOCAL_INFO, "light_interface: Could not load dynamic library: 'input' not specified in config file.");
            continue;
        }
        const YAML::Node& node_input = lightNode["input"];
        for (std::size_t i = 0; i < node_input.size(); i++)
        {
            inputs.push_back(node_input[i].as<std::string>());
        }

        if (lightNode["output"].IsDefined())
        {
            const YAML::Node& node_output = lightNode["output"];
            for (std::size_t i = 0; i < node_output.size(); i++)
            {
                outputs.push_back(node_output[i].as<std::string>());
            }
        }

        logger() << "Configuration for function '" << function_name << "':" << endl;
        logger() << "  user_lib: " << user_lib << endl;
        logger() << "  init_fun: " << init_fun << endl;
        logger() << "  lang:     " << lang << EOM;

        if (lang == "c" or lang == "c++" or lang == "fortran")
        {
            lightLibrary_C_CXX_Fortran(user_lib, init_fun, lang, function_name, inputs, outputs);
        }

#ifdef HAVE_PYBIND11
        if (lang == "python")
        {
            lightLibrary_Python(user_lib, init_fun, lang, function_name, inputs, outputs);
        }
#endif
    }

    // TODO: cleanup in the destructor
    // dlclose(handle);

    _so_initialized = true;
}
END_BE_INI_FUNCTION

