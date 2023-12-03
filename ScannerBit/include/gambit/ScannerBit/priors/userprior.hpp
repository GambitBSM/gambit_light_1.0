//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  Prior class used by GAMBIT-light to connect 
///  to a user-provided prior transformation 
///  function in an external library.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
//
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date Jun, Dec 2023
///
///  *********************************************

#ifndef USERPRIOR_HPP
#define USERPRIOR_HPP

#include <algorithm>

#include "gambit/ScannerBit/priors.hpp"
#include "gambit_light_interface.h"


namespace Gambit
{
  namespace gambit_light_interface
  {
    // Functions from the gambit_light_interface library
    extern void run_user_prior(const std::vector<std::string>&, const std::vector<double>&, std::vector<double>&, std::vector<std::string>&);
    extern void init_user_lib_C_CXX_Fortran(const std::string&, const std::string&, const std::string&, const std::string&, const std::vector<std::string>&);
    extern void init_user_lib_Python(const std::string&, const std::string&, const std::string&, const std::vector<std::string>&);
  }
}


namespace Gambit
{
    namespace Priors
    {
        class UserPrior : public BasePrior
        {
        private:

        public:
            // Constructor
            UserPrior(const std::vector<std::string>& param, const Options& options) : BasePrior(param, param.size())
            {
                YAML::Node userPriorNode = options.getNode();
        
                std::string lang = userPriorNode["lang"].as<std::string>();
                std::string user_lib = userPriorNode["user_lib"].as<std::string>();
                std::string func_name = userPriorNode["func_name"].as<std::string>();
  
                std::vector<std::string> outputs = param_names;

                if (lang == "c" or lang == "c++" or lang == "fortran")
                {
                    try
                    {
                        Gambit::gambit_light_interface::init_user_lib_C_CXX_Fortran(user_lib, func_name, lang, "[prior]", outputs);
                    }
                    catch (const std::runtime_error& e)
                    {
                        Scanner::scan_error().raise(LOCAL_INFO, 
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
                            Gambit::gambit_light_interface::init_user_lib_Python(user_lib, func_name, "[prior]", outputs);
                        }
                        catch (const std::runtime_error& e)
                        {
                            // Scanner::scan_error().raise(LOCAL_INFO, err.str())
                            Scanner::scan_error().raise(LOCAL_INFO,
                                "Caught runtime error while initialising the "
                                "gambit_light_interface: " + std::string(e.what())
                            );
                        }
                    }
                #endif
            }

            double log_prior_density(const std::unordered_map<std::string, double> &) const override 
            { 
                Scanner::scan_error().raise(LOCAL_INFO, 
                    "The 'log_prior_density' functionality for user-supplied priors "
                    "is not yet implemented in GAMBIT-light. Please use a scanner "
                    "that does not rely on this functionality."
                );

                // TODO: Allow user to register a function for the log prior density
                //       and call that function here.

                // Placeholder return statement
                return 1.; 
            }

            void transform(hyper_cube_ref<double> unitpars, std::unordered_map<std::string,double>& outputMap) const override
            {
                std::vector<std::string> warnings;

                // Use names consistent with other parts of GAMBIT light
                const std::vector<std::string>& input_names = param_names;
                std::vector<double> input_vals(unitpars.size());

                // Initialise input_vals
                for (int i = 0, end = unitpars.size(); i < end; ++i)
                {
                    input_vals[i] = unitpars[i];
                }

                // Initialise output vector to 0.0;
                std::vector<double> output(input_names.size(), 0.0);

                // Call run_user_prior from the interface library. 
                // This will fill the output map (and the 'warnings' vector).
                try
                {
                    Gambit::gambit_light_interface::run_user_prior(input_names, input_vals, output, warnings);
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
                        Scanner::scan_error().raise(LOCAL_INFO, errmsg);
                    }
                    else
                    {
                        Scanner::scan_error().raise(LOCAL_INFO, "Caught an unrecognized runtime error: " + errmsg);
                    }
                }

                // Log any warnings that we have collected.
                for (const std::string& w : warnings) 
                {
                    Scanner::scan_warning().raise(LOCAL_INFO, w);
                }

                // Fill the outputMap
                for (size_t i = 0; i < input_names.size(); i++)
                {
                  outputMap[input_names[i]] = output[i];
                }
            }

            void inverse_transform(const std::unordered_map<std::string, double> &physical, hyper_cube_ref<double> unit) const override
            {
                Scanner::scan_error().raise(LOCAL_INFO, 
                    "The 'inverse_transform' functionality for user-supplied priors "
                    "is not yet implemented in GAMBIT-light. Please use a scanner "
                    "that does not rely on this functionality."
                );

                // TODO: Allow user to register an inverse transform function
                //       and call that here.

                // Placeholder code that just sets the hyper_cube_ref equal to the physical parameter point.
                for (int i = 0, end = this->size(); i < end; ++i)
                {
                    unit[i] = physical.at(param_names[i]);
                }
            }

        };

        LOAD_PRIOR(userprior, UserPrior)
    }
}

#endif
