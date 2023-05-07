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

    void input(std::map<std::string,double> &result)
    {
      using namespace Pipes::input;

      // Only the first time this function is run: 
      // Construct a map with pointers to the parameter values in Param
      static std::map<std::string,const double*> param_pointer_map;
      static bool first = true;
      if (first)
      {
        // Get the names of input parameters in use
        std::set<std::string> input_par_set = BEreq::get_input_par_set();

        // For each parameter, get the pointer from Param in param_pointer_map
        for (const std::string& par_name: input_par_set) 
        {
          param_pointer_map[par_name] = Param.at(par_name).operator->();
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


    // This function will run the light_interface library 
    // and collect all the results from all the connected user libraries
    // in one map<string,double>.
    void output(std::map<std::string,double> &result)
    {
      using namespace Pipes::output;

      BEreq::run_light_interface(*Dep::input, result);
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

