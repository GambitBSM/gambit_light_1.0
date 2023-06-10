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
    namespace Priors
    {
        class UserPrior : public BasePrior
        {
        private:

        public:
            // Constructor
            UserPrior(const std::vector<std::string>& param, const Options&) : BasePrior(param, param.size())
            {
                // _Anders
                // Make connection to external code here
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
