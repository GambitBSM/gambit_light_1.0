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

namespace Gambit
{
  namespace LightBit
  {
    using namespace LogTags;

    //************************************************************

    /// \name Module functions
    /// @{

    // This function will run the light_interface library 
    // and collect all the results from all the connected user libraries
    // in one map<string,double>.
    void output(std::map<std::string,double> &result)
    {
      using namespace Pipes::output;

      std::map<std::string,double> input;

      // Construct input map from GAMBIT parameter map:
      for (auto& kv : Param) 
      {
        input[kv.first] = *kv.second;
      }

      BEreq::run_light_interface(input, result);
    }


    // This function will extract the expected 'total_loglike' entry
    // from the light_interface output map.
    void total_loglike(double &result)
    {
      using namespace Pipes::total_loglike;

      // Get the output map from dependency "light_output"
      const std::map<std::string,double> &output = *Dep::output;

      result = output.at("total_loglike");
    }


    /// @}

  }

}

