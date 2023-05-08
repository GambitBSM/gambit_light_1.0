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
///  \date 2022, 2023 Apr, 2023 May
///  \author Marcin Krotkiewski
///          (marcin.krotkiewski@usit.uio.no)
///  \date 2022 Sep
///
///  *********************************************

#include "gambit/Backends/frontend_macros.hpp"
#include "gambit/Backends/frontends/light_interface_0_1.hpp"

#include <set>

BE_NAMESPACE
{
    set_str input_par_set;
}
END_BE_NAMESPACE

BE_INI_FUNCTION
{
    // Only perform the initialization the first time this function is run.
    static bool _so_initialised = false;
    if(_so_initialised) return;

    std::string user_lib;
    std::string loglike_name;
    std::string init_fun;
    std::string lang;
    std::vector<std::string> all_outputs;
    std::set<std::string> listed_usermodel_pars;

    YAML::Node userModelNode = runOptions->getNode("UserModel");
    YAML::Node userLogLikesNode = runOptions->getNode("UserLogLikes");

    // First collect all parameter names listed in the "UserModel" section
    for(YAML::const_iterator it = userModelNode.begin(); it != userModelNode.end(); ++it)
    {
        std::string usermodel_par_name = it->first.as<str>();
        listed_usermodel_pars.insert(usermodel_par_name);
    }

    // Now loop over the entries in "UserLogLikes" and perform a set of checks
    for (std::size_t fi = 0; fi < userLogLikesNode.size(); fi++)
    {
        std::vector<std::string> inputs;
        std::vector<std::string> outputs;

        const YAML::Node& userLogLikesEntry = userLogLikesNode[fi];

        if (not userLogLikesEntry["loglike_name"].IsDefined())
        {
            backend_error().raise(LOCAL_INFO, "Error while parsing the UserLogLikes settings: 'loglike_name' not specified in config file.");
        }
        loglike_name = userLogLikesEntry["loglike_name"].as<std::string>();

        if (not userLogLikesEntry["user_lib"].IsDefined())
        {
            backend_error().raise(LOCAL_INFO, "Error while parsing the UserLogLikes settings: 'user_lib' not specified in config file.");
        }
        user_lib = userLogLikesEntry["user_lib"].as<std::string>();

        if (not userLogLikesEntry["init_fun"].IsDefined())
        {
            backend_error().raise(LOCAL_INFO, "Error while parsing the UserLogLikes settings: 'init_fun' not specified in config file.");
        }
        init_fun = userLogLikesEntry["init_fun"].as<std::string>();

        if (not userLogLikesEntry["lang"].IsDefined())
        {
            backend_error().raise(LOCAL_INFO, "Error while parsing the UserLogLikes settings: 'lang' not specified in config file.");
        }
        lang = userLogLikesEntry["lang"].as<std::string>();

        if (lang != "fortran" and 
            lang != "c" and
#ifdef HAVE_PYBIND11
            lang != "python" and
#endif
            lang != "c++")
        {
            backend_error().raise(LOCAL_INFO, "Error while parsing the UserLogLikes settings: unsupported plugin language '" + lang + "'.");
            continue;
        }

        if (userLogLikesEntry["input"].IsDefined())
        {
            const YAML::Node& inputNode = userLogLikesEntry["input"];
            for (std::size_t i = 0; i < inputNode.size(); i++)
            {
                std::string input_par_name = inputNode[i].as<std::string>();

                // Check that all parameter names listed under "input" in the "UserLogLikes" 
                // section are also specified in the "UserModel" section.
                if (not userModelNode[input_par_name].IsDefined())
                {
                    str error_msg = "Error while parsing the UserLogLikes settings: "
                                    "The parameter " + input_par_name + " is requested as an input parameter, "
                                    "but it is not listed in the UserModel section.";
                    backend_error().raise(LOCAL_INFO, error_msg);
                }

                // Register requested input parameter
                inputs.push_back(input_par_name);
                input_par_set.insert(input_par_name);
            }
        }
        else  // The current "UserLogLike" entry has no "input" node
        {
            // If there is no "input" node we assume that all parameters
            // from "UserModel" should be used as input.
            inputs.assign(listed_usermodel_pars.begin(), listed_usermodel_pars.end());
            input_par_set.insert(listed_usermodel_pars.begin(), listed_usermodel_pars.end());
        }

        if (userLogLikesEntry["output"].IsDefined())
        {
            const YAML::Node& node_output = userLogLikesEntry["output"];
            for (std::size_t i = 0; i < node_output.size(); i++)
            {
                std::string output_name = node_output[i].as<std::string>();
                // Throw error if there already exists an output with the same output_name
                if (std::find(all_outputs.begin(), all_outputs.end(), output_name) != all_outputs.end())
                {
                    backend_error().raise(LOCAL_INFO, "Error while parsing the UserLogLikes settings: multiple outputs with the same name '" + output_name + "'. Each output must be assigned a unique name.");
                }
                outputs.push_back(output_name);
                all_outputs.push_back(output_name);
            }
        }

        logger() << "Configuration for the loglike '" << loglike_name << "':" << endl;
        logger() << "  user_lib: " << user_lib << endl;
        logger() << "  init_fun: " << init_fun << endl;
        logger() << "  lang:     " << lang << EOM;

        if (lang == "c" or lang == "c++" or lang == "fortran")
        {
            try
            {
                init_user_lib_C_CXX_Fortran(user_lib, init_fun, lang, loglike_name, inputs, outputs);
            }
            catch (const std::runtime_error& e)
            {
                backend_error().raise(LOCAL_INFO, "Caught runtime error while initialising the light_interface backend: " + std::string(e.what()));
            }
        }

#ifdef HAVE_PYBIND11
        if (lang == "python")
        {
            try
            {
                init_user_lib_Python(user_lib, init_fun, lang, loglike_name, inputs, outputs);
            }
            catch (const std::runtime_error& e)
            {
                backend_error().raise(LOCAL_INFO, "Caught runtime error while initialising the light_interface backend: " + std::string(e.what()));
            }
        }
#endif
    }

    _so_initialised = true;
}
END_BE_INI_FUNCTION



// Convenience functions
BE_NAMESPACE
{
    void run_light_interface(const map_str_dbl& input, map_str_dbl& result)
    {

        std::vector<std::string> warnings;

        // Call run_user_loglikes from the light_interface library. 
        // This will fill the result map (and the warnings vector).
        try
        {
            run_user_loglikes(input, result, warnings);
        }
        catch (const std::runtime_error& e)
        {
            std::string errmsg(e.what());
            std::string errmsg_lowercase = Utils::strtolower(errmsg);
            if (errmsg_lowercase.substr(0,13) == "invalid point")
            {
                invalid_point().raise("Caught an 'invalid point' message via the light_interface library: " + errmsg);          
            }
            else
            {
                backend_error().raise(LOCAL_INFO, "Caught a runtime error via the light_interface library: " + std::string(e.what()));
            }
        }

        // Log any warnings that we have collected
        for (const std::string& w : warnings) 
        {
            backend_warning().raise(LOCAL_INFO, w);
        }
    }

    
    set_str get_input_par_set()
    {
        return input_par_set;
    }

}
END_BE_NAMESPACE
