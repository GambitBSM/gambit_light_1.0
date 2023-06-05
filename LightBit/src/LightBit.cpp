//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions of module LightBit
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date 2022 Sep, 2023 Apr, 2023 May
///
///  *********************************************

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/LightBit/LightBit_rollcall.hpp"
#include "gambit_light_interface.h"

namespace Gambit
{
  namespace gambit_light_interface
  {

    // Functions from the gambit_light_interface library
    extern void run_user_loglikes(const map_str_dbl&, map_str_dbl&, vec_str&);
    extern void init_user_lib_C_CXX_Fortran(const std::string&, const std::string&, const std::string&, const std::string&, const std::vector<std::string>&, const std::vector<std::string>&);
    extern void init_user_lib_Python(const std::string&, const std::string&, const std::string&, const std::vector<std::string>&, const std::vector<std::string>&);

  }
}


namespace Gambit
{
  namespace LightBit
  {
    using namespace LogTags;

    //************************************************************

    /// \name Module-level variables
    /// @{

    std::vector<std::string> listed_user_pars;
    std::vector<std::string> listed_model_pars;
    std::map<std::string,std::string> user_to_model_par_names;

    /// @}

    /// \name Module functions
    /// @{

    // The first time this function is run, it performs a series of consistency 
    // checks for the YAML file settings and extracts the information needed.
    void initialisation(bool& result)
    {
      using namespace Pipes::initialisation;

      // Perform the initialisation the first time this function is run.
      static bool initialisation_done = false;
      if (not initialisation_done)
      {

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
                    LightBit_error().raise(LOCAL_INFO,
                        "Error while parsing the UserLogLikes settings: Unknown option '" 
                        + option_name + "' for the loglike '" + loglike_name + "'. "
                        "(Maybe a typo?)"
                    );
                }
            }
        }

        // Collect all parameter names listed in the "UserModel" section (and check for duplicates).
        for(YAML::const_iterator it = userModelNode.begin(); it != userModelNode.end(); ++it)
        {
            // Get the UserModel parameter name
            std::string model_par_name = it->first.as<std::string>();

            // If there's a "name" entry for this parameter, use it.
            // If not, just use the model_par_name.
            std::string user_par_name = model_par_name;
            const YAML::Node& par_node = it->second;
            if (par_node["name"].IsDefined())
            {
                user_par_name = par_node["name"].as<std::string>();
            }

            // Check for duplicate entries of model_par_name ("p1", "p2", etc.)
            if (std::find(listed_model_pars.begin(), listed_model_pars.end(), model_par_name) != listed_model_pars.end())
            {
                LightBit_error().raise(LOCAL_INFO, 
                    "Error while parsing the UserLogLikes settings: Multiple entries " 
                    "for the parameter '" + model_par_name + "'."
                );
            }

            // Check for duplicate entries of the user-specified parameter name.
            if (std::find(listed_user_pars.begin(), listed_user_pars.end(), user_par_name) != listed_user_pars.end())
            {
                LightBit_error().raise(LOCAL_INFO, 
                    "Error while parsing the UserLogLikes settings: "
                    "The parameter name '" + user_par_name + "' is "
                    "assigned to multiple parameters."
                );
            }

            // This is a new parameter. Register it.
            listed_user_pars.push_back(user_par_name);
            listed_model_pars.push_back(model_par_name);
            user_to_model_par_names[user_par_name] = model_par_name;
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
                LightBit_error().raise(LOCAL_INFO, 
                    "Error while parsing the UserLogLikes settings: "
                    "'user_lib' not specified in config file."
                );
            }
            user_lib = userLogLikesEntry["user_lib"].as<std::string>();

            if (not userLogLikesEntry["init_fun"].IsDefined())
            {
                LightBit_error().raise(LOCAL_INFO,
                    "Error while parsing the UserLogLikes settings: "
                    "'init_fun' not specified in config file."
                );
            }
            init_fun = userLogLikesEntry["init_fun"].as<std::string>();

            if (not userLogLikesEntry["lang"].IsDefined())
            {
                LightBit_error().raise(LOCAL_INFO, 
                    "Error while parsing the UserLogLikes settings: "
                    "'lang' not specified in config file."
                );
            }
            lang = userLogLikesEntry["lang"].as<std::string>();

            // Avoid confusion with "Fortran" vs "fortran", etc. 
            std::transform(lang.begin(), lang.end(), lang.begin(), ::tolower);

            // Treat "lang: cpp" and "lang: cxx" as equivalent to "lang: c++"
            if (lang == "cpp" or lang == "cxx") { lang = "c++"; }

            if (lang != "fortran" and 
                lang != "c" and
                #ifdef HAVE_PYBIND11
                  lang != "python" and
                #endif
                lang != "c++")
            {
                LightBit_error().raise(LOCAL_INFO, 
                    "Error while parsing the UserLogLikes settings: "
                    "unsupported plugin language '" + lang + "'."
                );
            }

            if (userLogLikesEntry["input"].IsDefined())
            {
                const YAML::Node& inputNode = userLogLikesEntry["input"];
                for (std::size_t i = 0; i < inputNode.size(); i++)
                {
                    std::string input_par_name = inputNode[i].as<std::string>();

                    // Check for duplicate parameter names in the "input" section.
                    bool in_inputs = std::find(inputs.begin(), inputs.end(), input_par_name) != inputs.end();
                    if(in_inputs)
                    {
                        LightBit_error().raise(LOCAL_INFO,
                            "Error while parsing the UserLogLikes settings: The parameter name '" 
                            + input_par_name + "' appears multiple times in the input section for "
                            "the loglike '" + loglike_name + "'."
                        );
                    }

                    // Check that all parameter names listed under "input" in the "UserLogLikes" 
                    // section are also specified in the "UserModel" section.
                    // if (not userModelNode[input_par_name].IsDefined())
                    bool in_listed_user_pars = std::find(listed_user_pars.begin(), listed_user_pars.end(), input_par_name) != listed_user_pars.end();
                    bool in_listed_model_pars = std::find(listed_model_pars.begin(), listed_model_pars.end(), input_par_name) != listed_model_pars.end();
                    if(not in_listed_user_pars)
                    {
                        if(in_listed_model_pars)
                        {
                            LightBit_error().raise(LOCAL_INFO,
                                "Error while parsing the UserLogLikes settings: The parameter '" 
                                + input_par_name + "' is requested as an input parameter for the "
                                "loglike '" + loglike_name + "', but this parameter has been assigned "
                                "a new name in the UserModel section. Use this new name in the 'input' "
                                "section for '" + loglike_name + "', or remove the 'name' option in "
                                "the UserModel section."
                            );
                        }
                        else
                        {
                            LightBit_error().raise(LOCAL_INFO,
                                "Error while parsing the UserLogLikes settings: The parameter '" 
                                + input_par_name + "' is requested as an input parameter for the "
                                "loglike '" + loglike_name + "', but it is not found in the "
                                "UserModel section."
                            );
                        }
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
                        LightBit_error().raise(LOCAL_INFO, 
                            "Error while parsing the UserLogLikes settings: multiple outputs "
                            "with the same name '" + output_name + "'. Each output must be " 
                            "assigned a unique name."
                        );
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
                    Gambit::gambit_light_interface::init_user_lib_C_CXX_Fortran(user_lib, init_fun, lang, loglike_name, inputs, outputs);
                }
                catch (const std::runtime_error& e)
                {
                    LightBit_error().raise(LOCAL_INFO, 
                        "Caught runtime error while initialising the "
                        "gambit_light_interface: " + std::string(e.what())
                    );
                }
            }

            #ifdef HAVE_PYBIND11
              if (lang == "python")
              {
                  try
                  {
                      Gambit::gambit_light_interface::init_user_lib_Python(user_lib, init_fun, loglike_name, inputs, outputs);
                  }
                  catch (const std::runtime_error& e)
                  {
                      LightBit_error().raise(LOCAL_INFO,
                        "Caught runtime error while initialising the "
                        "gambit_light_interface: " + std::string(e.what())
                    );
                  }
              }
            #endif
        }

        initialisation_done = true;
      }  // End initialisation 

      result = initialisation_done;
    }


    // This function constructs the map of named input parameters that 
    // the 'output' function below passes on to the gambit_light_interface library.
    void input(std::map<std::string,double>& result)
    {
      using namespace Pipes::input;

      // Only the first time this function is run: 
      // Construct a map with pointers to the parameter values in Param
      static std::map<std::string,const double*> param_pointer_map;
      static bool first = true;

      if (first)
      {
        // For each parameter, add a pointer in param_pointer_map that points
        // to the corresponding parameter value in Param
        for (const std::string& user_par_name: listed_user_pars)
        {
          param_pointer_map[user_par_name] = Param.at(user_to_model_par_names[user_par_name]).operator->();
        }

        first = false;
      }

      // Every time this function is run: 
      // Fill the result map using the param_pointer_map
      for (auto& kv: param_pointer_map) 
      {
        result[kv.first] = *kv.second;
      }
    }


    // This function will run the gambit_light_interface library 
    // and collect all the results from all the connected user libraries
    // in one map<string,double>.
    void output(map_str_dbl& result)
    {
      using namespace Pipes::output;

      const map_str_dbl& input = *Dep::input;
      std::vector<std::string> warnings;

      // Call run_user_loglikes from the interface library. 
      // This will fill the result map (and the 'warnings' vector).
      try
      {
        Gambit::gambit_light_interface::run_user_loglikes(input, result, warnings);
      }
      catch (const std::runtime_error& e)
      {
        std::string errmsg(e.what());

        if (errmsg.substr(0,9) == "[invalid]")
        {
          errmsg.erase(0,9);
          invalid_point().raise(errmsg);
        }
        else if (errmsg.substr(0,7) == "[fatal]")
        {
          errmsg.erase(0,7);
          LightBit_error().raise(LOCAL_INFO, errmsg);
        }
        else
        {
          LightBit_error().raise(LOCAL_INFO, "Caught an unrecognized runtime error: " + errmsg);
        }
      }

      // Log any warnings that we have collected.
      for (const std::string& w : warnings) 
      {
        LightBit_warning().raise(LOCAL_INFO, w);
      }

    }


    // This function will extract the expected 'total_loglike' entry
    // from the gambit_light_interface output map.
    void total_loglike(double& result)
    {
      using namespace Pipes::total_loglike;

      // Get the output map from dependency "light_output"
      const std::map<std::string,double> &output = *Dep::output;

      result = output.at("total_loglike");
    }


    /// @}

  }

}

