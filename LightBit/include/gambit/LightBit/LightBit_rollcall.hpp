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
///  \date 2022 Sep, 2023 Apr, 2023 May
///
///  *********************************************

#ifndef __LightBit_rollcall_hpp__
#define __LightBit_rollcall_hpp__

#include "gambit/cmake/cmake_variables.hpp"
#include "gambit/Elements/shared_types.hpp"
#include "gambit/Utils/util_types.hpp"
#include "gambit/LightBit/LightBit_types.hpp"

#define MODULE LightBit
START_MODULE
  
  #define CAPABILITY initialisation
  START_CAPABILITY
    #define FUNCTION initialisation
    START_FUNCTION(bool)
    ALLOW_MODELS(UserModel)
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY input_point
  START_CAPABILITY
    #define FUNCTION input_point
    START_FUNCTION(parameter_point)
    DEPENDENCY(initialisation, bool)
    ALLOW_MODELS(UserModel)
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY input
  START_CAPABILITY
    #define FUNCTION input
    START_FUNCTION(map_str_dbl)
    DEPENDENCY(input_point, parameter_point)
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY output
  START_CAPABILITY
    #define FUNCTION output
    START_FUNCTION(map_str_dbl)
    DEPENDENCY(input_point, parameter_point)
    #undef FUNCTION
  #undef CAPABILITY

  #define CAPABILITY total_loglike
  START_CAPABILITY
    #define FUNCTION total_loglike
    START_FUNCTION(double)
    DEPENDENCY(output, map_str_dbl)
    #undef FUNCTION
  #undef CAPABILITY

#undef MODULE

#endif /* defined __LightBit_rollcall_hpp__ */
