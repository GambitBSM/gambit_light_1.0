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
    extern void run_user_loglikes(const map_str_dbl&, map_str_dbl&, vec_str&);
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
            std::string lang;
            std::string user_lib;
            std::string init_fun;

        public:
            // Constructor
            UserPrior(const std::vector<std::string>& param, const Options& options) : BasePrior(param, param.size())
            {
                std::cerr << "DEBUG: Start UserPrior constructor" << std::endl;

                YAML::Node userPriorNode = options.getNode();
        
                lang = userPriorNode["lang"].as<std::string>();
                user_lib = userPriorNode["user_lib"].as<std::string>();
                init_fun = userPriorNode["init_fun"].as<std::string>();
  
                std::vector<std::string> inputs;
                std::vector<std::string> outputs;

                if (lang == "c" or lang == "c++" or lang == "fortran")
                {
                    try
                    {
                        Gambit::gambit_light_interface::init_user_lib_C_CXX_Fortran(user_lib, init_fun, lang, "prior", inputs, outputs);
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
                            Gambit::gambit_light_interface::init_user_lib_Python(user_lib, init_fun, "prior", inputs, outputs);
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

                std::cerr << "DEBUG: End UserPrior constructor" << std::endl;
            }

            void transform(const std::vector<double> &unitpars, std::unordered_map<std::string, double> &outputMap) const override
            {
                // _Anders
                // - Pass unitpars vector and output map to user function
                // - The user function Receive transformed parameters
                auto it_vec = unitpars.begin();
                for (auto it = param_names.begin(), end = param_names.end(); it != end; it++)
                {
                    outputMap[*it] = *(it_vec++);
                }
            }

            std::vector<double> inverse_transform(const std::unordered_map<std::string, double> &physical) const override
            {
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
