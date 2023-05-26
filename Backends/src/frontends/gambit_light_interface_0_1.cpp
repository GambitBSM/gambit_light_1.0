//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Backend that connects GAMBIT to the
///  gambit_light_interface library
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
#include "gambit/Backends/frontends/gambit_light_interface_0_1.hpp"

BE_NAMESPACE
{
    std::vector<std::string> listed_user_pars;
    std::vector<std::string> listed_model_pars;
    std::map<std::string,std::string> model_to_user_par_names;
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

    YAML::Node userModelNode = runOptions->getNode("UserModel");
    YAML::Node userLogLikesNode = runOptions->getNode("UserLogLikes");

    // Check for unknown options or typos in the "UserLogLikes" section.
    const static std::vector<std::string> known_userloglike_options = {"lang", "user_lib", "init_fun", "input", "output"};
    for(YAML::const_iterator it = userLogLikesNode.begin(); it != userLogLikesNode.end(); ++it)
    {
        std::string loglike_name = it->first.as<std::string>();
        const YAML::Node& userLogLikesEntry = it->second;
        for(YAML::const_iterator it2 = userLogLikesEntry.begin(); it2 != userLogLikesEntry.end(); ++it2)
        {
            std::string option_name = it2->first.as<std::string>();
            if (std::find(known_userloglike_options.begin(), known_userloglike_options.end(), option_name) == known_userloglike_options.end())
            {
                backend_error().raise(LOCAL_INFO, "Error while parsing the UserLogLikes settings: Unknown option '" + option_name + "' for the loglike '" + loglike_name + "'. (Maybe a typo?)");
            }
        }
    }

    // Collect all parameter names listed in the "UserModel" section (and check for duplicates).
    for(YAML::const_iterator it = userModelNode.begin(); it != userModelNode.end(); ++it)
    {
        std::string model_par_name = it->first.as<std::string>();
        std::string user_par_name = (it->second)["name"].as<std::string>();

        // Check for duplicate entries of model_par_name ("p1", "p2", etc.)
        if (std::find(listed_model_pars.begin(), listed_model_pars.end(), model_par_name) != listed_model_pars.end())
        {
            backend_error().raise(LOCAL_INFO, "Error while parsing the UserLogLikes settings: Multiple entries for the parameter '" + model_par_name + "'.");
        }

        // Check for duplicate entries of the user-specified parameter name.
        if (std::find(listed_user_pars.begin(), listed_user_pars.end(), user_par_name) != listed_user_pars.end())
        {
            backend_error().raise(LOCAL_INFO, "Error while parsing the UserLogLikes settings: The parameter name '" + user_par_name + "' is assigned to multiple parameters.");
        }

        // This is a new parameter. Register it.
        listed_user_pars.push_back(user_par_name);
        listed_model_pars.push_back(model_par_name);
        model_to_user_par_names[model_par_name] = user_par_name;
    }

    // Check consistency and collect settings from the "UserLogLikes" section.
    for(YAML::const_iterator it = userLogLikesNode.begin(); it != userLogLikesNode.end(); ++it)
    {
        std::string loglike_name = it->first.as<std::string>();
        const YAML::Node& userLogLikesEntry = it->second;

        std::vector<std::string> inputs;
        std::vector<std::string> outputs;

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
                // if (not userModelNode[input_par_name].IsDefined())
                bool in_listed_user_pars = std::find(listed_user_pars.begin(), listed_user_pars.end(), input_par_name) != listed_user_pars.end();
                bool in_listed_model_pars = std::find(listed_model_pars.begin(), listed_model_pars.end(), input_par_name) != listed_model_pars.end();
                if(not in_listed_user_pars)
                {
                    std::string error_msg;
                    if(in_listed_model_pars)
                    {
                        error_msg = "Error while parsing the UserLogLikes settings: The parameter " 
                                    + input_par_name + " is requested as an input parameter for the "
                                    "loglike '" + loglike_name + "', but this parameter has been assigned "
                                    "a new name in the UserModel section. Use this new name in the 'input' "
                                    "section for '" + loglike_name + "', or remove the 'name' option in "
                                    "the UserModel section.";
                    }
                    else
                    {
                        error_msg = "Error while parsing the UserLogLikes settings: The parameter " 
                                    + input_par_name + " is requested as an input parameter for the "
                                    "loglike '" + loglike_name + "', but it is not found in the "
                                    " UserModel section.";
                    }
                    backend_error().raise(LOCAL_INFO, error_msg);
                }

                // Register requested input parameter
                inputs.push_back(input_par_name);
            }
        }
        else  // The current "UserLogLike" entry has no "input" node
        {
            // If there is no "input" node we assume that all listed 
            // parameters should be used as input.
            inputs.assign(listed_user_pars.begin(), listed_user_pars.end());
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
                backend_error().raise(LOCAL_INFO, "Caught runtime error while initialising the gambit_light_interface backend: " + std::string(e.what()));
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
                backend_error().raise(LOCAL_INFO, "Caught runtime error while initialising the gambit_light_interface backend: " + std::string(e.what()));
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

        // Call run_user_loglikes from the interface library. 
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
                invalid_point().raise("Caught an 'invalid point' message via the gambit_light_interface library: " + errmsg);          
            }
            else
            {
                backend_error().raise(LOCAL_INFO, "Caught a runtime error via the gambit_light_interface library: " + std::string(e.what()));
            }
        }

        // Log any warnings that we have collected
        for (const std::string& w : warnings) 
        {
            backend_warning().raise(LOCAL_INFO, w);
        }
    }

    
    vec_pair_str_str get_input_par_name_pairs()
    {
        vec_pair_str_str input_par_name_pairs;
        for (size_t i = 0; i < listed_user_pars.size(); ++i)
        {
            std::pair<std::string, std::string> p = {listed_user_pars[i], listed_model_pars[i]};
            input_par_name_pairs.push_back(p);
        }
        return input_par_name_pairs;
    }

}
END_BE_NAMESPACE
