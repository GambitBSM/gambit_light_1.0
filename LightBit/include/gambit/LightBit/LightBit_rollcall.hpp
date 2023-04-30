//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Rollcall header for LightBit module.
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

#ifndef __LightBit_rollcall_hpp__
#define __LightBit_rollcall_hpp__

#include "gambit/cmake/cmake_variables.hpp"
#include "gambit/Elements/shared_types.hpp"
#include "gambit/Utils/util_types.hpp"


#define MODULE LightBit
START_MODULE

  #define CAPABILITY light_output
  START_CAPABILITY
    #define FUNCTION get_light_output
    START_FUNCTION(map_str_dbl)
    ALLOW_MODELS(GenericModel5, GenericModel10, GenericModel15, GenericModel20)
    BACKEND_REQ(run_light_interface, (light_interface), void, (const map_str_dbl&, map_str_dbl&, vec_str&))
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY light_loglike
  START_CAPABILITY
    #define FUNCTION get_light_loglike
    START_FUNCTION(double)
    DEPENDENCY(light_output, map_str_dbl)
    #undef FUNCTION
  #undef CAPABILITY

#undef MODULE

#endif /* defined __LightBit_rollcall_hpp__ */
