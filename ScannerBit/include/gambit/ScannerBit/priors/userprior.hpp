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
///  \date Jun 2023
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
    extern void run_user_prior(const map_str_dbl&, map_str_dbl&, vec_str&);
    extern void init_user_lib_C_CXX_Fortran(const std::string&, const std::string&, const std::string&, const std::string&, const std::vector<std::string>&, const std::vector<std::string>&);
    extern void init_user_lib_Python(const std::string&, const std::string&, const std::string&, const std::vector<std::string>&, const std::vector<std::string>&);
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
                std::string init_fun = userPriorNode["init_fun"].as<std::string>();
  
                std::vector<std::string> inputs;
                std::vector<std::string> outputs;

                if (lang == "c" or lang == "c++" or lang == "fortran")
                {
                    try
                    {
                        Gambit::gambit_light_interface::init_user_lib_C_CXX_Fortran(user_lib, init_fun, lang, "[prior]", inputs, outputs);
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
                            Gambit::gambit_light_interface::init_user_lib_Python(user_lib, init_fun, "[prior]", inputs, outputs);
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

            void transform(const std::vector<double> &unitpars, std::unordered_map<std::string, double> &outputMap) const override
            {
                std::vector<std::string> warnings;

                // Prepare input and result maps, and fill 
                // the input map with the parameter point.
                //
                // TODO: 
                //   This vector --> map translation can be 
                //   avoided if we base communication on vectors
                //   rather than on maps. 

                std::map<std::string,double> input;
                std::map<std::string,double> result;

                auto it_vec = unitpars.begin();
                for (auto it = param_names.begin(), end = param_names.end(); it != end; it++)
                {
                    input[*it] = *(it_vec++);
                }

                // Call run_user_prior from the interface library. 
                // This will fill the result map (and the 'warnings' vector).
                try
                {
                    Gambit::gambit_light_interface::run_user_prior(input, result, warnings);
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

                for (const auto& kv : result)
                {
                    outputMap[kv.first] = kv.second;
                }
            }

            std::vector<double> inverse_transform(const std::unordered_map<std::string, double> &physical) const override
            {

                Scanner::scan_error().raise(LOCAL_INFO, 
                    "The 'inverse_transform' functionality for user-supplied priors "
                    "is not yet implemented in GAMBIT-light. Please use a scanner "
                    "that does not rely on this functionality."
                );

                // TODO: Allow user to register an inverse transform function
                //       and call that here.

                // Dummy code that just returns the point without transforming it
                std::vector<double> u;
                for (const auto& n : param_names)
                {
                    u.push_back(physical.at(n));
                }
                return u;
            }

        };

        LOAD_PRIOR(userprior, UserPrior)
    }
}

#endif
