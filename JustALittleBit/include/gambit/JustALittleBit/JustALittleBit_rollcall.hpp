//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Rollcall header for JustALittleBit module.
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

#ifndef __JustALittleBit_rollcall_hpp__
#define __JustALittleBit_rollcall_hpp__

#include "gambit/cmake/cmake_variables.hpp"
#include "gambit/Elements/shared_types.hpp"


#define MODULE JustALittleBit
START_MODULE

  #define CAPABILITY ExternalLogLike
  START_CAPABILITY
    #define FUNCTION getExternalLogLike
    START_FUNCTION(double)
    ALLOW_MODELS(GenericModel5, GenericModel10, GenericModel15, GenericModel20)
    // BACKEND_REQ(pylike_get_loglike, (), double, (parameterMap&))
    #undef FUNCTION
  #undef CAPABILITY


#undef MODULE

#endif /* defined __JustALittleBit_rollcall_hpp__ */
