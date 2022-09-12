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

    void getExternalLogLike(double &result)
    {
        using namespace Pipes::getExternalLogLike;

        // Just use the first few parameters as a dummy log-likelihood result
        double p1 = *Param["p1"];
        double p2 = *Param["p2"];
        double loglike = p1 + p2;

        cout << "getExternalLogLike: Will return result: " << loglike << endl;

        result = loglike;

        // // Grab parameter values from the Param map (type map<str,double*>)
        // // and populate a map<str,double> that we pass to pylike 
        // std::map<std::string,double> pars; 
        // for (auto& kv : Param) 
        // {
        //     pars[kv.first] = *kv.second;
        // }

        // // Call the get_loglike function in pylike and store the result
        // result = BEreq::pylike_get_loglike(pars);
    }


    /// @}

  }

}

