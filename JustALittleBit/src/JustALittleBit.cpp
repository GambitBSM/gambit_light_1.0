//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions of module JustALittleBit
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date 2022 Sep
///
///  *********************************************

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/JustALittleBit/JustALittleBit_rollcall.hpp"

namespace Gambit
{

  namespace JustALittleBit
  {
    using namespace LogTags;

    //************************************************************

    /// \name Module functions
    /// @{

    void get_dummy_loglike(double &result)
    {
      using namespace Pipes::get_dummy_loglike;

      // Just use the first few parameters as a dummy log-likelihood result
      double p1 = *Param["p1"];
      double p2 = *Param["p2"];
      double loglike = p1 + p2;

      cout << endl;
      cout << "get_dummy_loglike: Will return result: " << loglike << endl;

      result = loglike;
    }



    void get_cpp_loglike(double &result)
    {
      using namespace Pipes::get_cpp_loglike;

      std::map<std::string,double> input;
      std::map<std::string,double> output;

      // Construct input map from GAMBIT parameter map:
      for (auto& kv : Param) 
      {
        input[kv.first] = *kv.second;
      }

      // Call the cpp interface library, which will fill the result map
      cout << endl;
      cout << "get_cpp_loglike: Will now call light_cpp_interface.";
      BEreq::run_light_cpp_interface(input, output);

      // Print the output map:
      cout << "get_cpp_loglike: Got output:";
      for (auto& kv : output) 
      {
        cout << "  " << kv.first << ":" << kv.second;
      }
      cout << endl;

      // Check that the output map has a "loglike" entry, 
      // and return this as the result.
      if (output.count("loglike") == 0)
      {
        JustALittleBit_error().raise(LOCAL_INFO, "Missing loglike entry in output map.");
      }

      double loglike = output.at("loglike");
      cout << "get_cpp_loglike: Will return result: " << loglike << endl;

      result = loglike;
    }

    /// @}

  }

}

