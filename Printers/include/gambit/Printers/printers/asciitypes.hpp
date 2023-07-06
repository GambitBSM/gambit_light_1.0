//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Sequence of all types printable by the ASCII
///  printer.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2017 Mar
///
///  *********************************************

#ifndef __ASCIITYPES__
#define __ASCIITYPES__

#include "gambit/ScannerBit/printable_types.hpp"

#define ASCII_TYPES                         \
  SCANNER_PRINTABLE_TYPES                   \
  (std::string)                             \
  (triplet<double>)                         \
  (map_intpair_dbl)                         \
  (map_const_str_dbl)                       \
  (map_const_str_map_const_str_dbl)         \
  (flav_prediction)                         \

#ifdef GAMBIT_LIGHT
  #define ASCII_BACKEND_TYPES
#else
  #define ASCII_BACKEND_TYPES           \
    (DM_nucleon_couplings)              \
    (BBN_container)
#endif

#endif
