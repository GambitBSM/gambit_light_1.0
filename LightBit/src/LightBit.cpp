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
    void get_light_output(std::map<std::string,double> &result)
    {
      using namespace Pipes::get_light_output;

      std::map<std::string,double> input;
      std::vector<std::string> warnings;

      // Construct input map from GAMBIT parameter map:
      for (auto& kv : Param) 
      {
        input[kv.first] = *kv.second;
      }

      // Call the cpp interface library, which will fill the result map
      cout << endl;
      cout << "get_light_output: Will now call light_interface." << endl;
      try
      {
        BEreq::run_light_interface(input, result, warnings);
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
          LightBit_error().raise(LOCAL_INFO, "Caught a runtime error via the light_interface library: " + std::string(e.what()));
        }
      }

      // Log any warnings that we have collected
      for (const std::string& w : warnings) 
      {
        LightBit_warning().raise(LOCAL_INFO, w);
      }

      // Print the output map:
      cout << "get_light_output: Got output:";
      for (auto& kv : result) 
      {
        cout << "  " << kv.first << ":" << kv.second;
      }
      cout << endl;
    }


    // This function will extract the expected 'loglike' entry
    // from the light_interface output map.
    void get_light_loglike(double &result)
    {
      using namespace Pipes::get_light_loglike;

      // Get the output map from dependency "light_output"
      const std::map<std::string,double> &output = *Dep::light_output;

      // Check that the output map has a "loglike" entry, 
      // and return this as the result.
      if (output.count("loglike") == 0)
      {
        LightBit_error().raise(LOCAL_INFO, "Missing loglike entry in output map.");
      }

      double loglike = output.at("loglike");
      cout << "get_light_loglike: Will return result: " << loglike << endl;

      result = loglike;
    }


    /// @}

  }

}
