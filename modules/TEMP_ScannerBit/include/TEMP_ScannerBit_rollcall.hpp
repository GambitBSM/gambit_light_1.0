//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Rollcall header for module TEMP_ScannerBit.
///
///  Compile-time registration of available 
///  observables and likelihoods, as well as their
///  dependencies.
///
///  Add to this if you want to add an observable
///  or likelihood to this module.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Ben Farmer 
///          (ben.farmer@gmail.com)
///    \date 2013 July 
///
///  *********************************************


#ifndef __TEMP_ScannerBit_rollcall_hpp__
#define __TEMP_ScannerBit_rollcall_hpp__

typedef std::map<std::string,double> parameters_map; //Cannot have commas in macro argument!

#define MODULE TEMP_ScannerBit
START_MODULE
  
  #define CAPABILITY alpha_parameters      // Parameters generated by ScannerBit!!!
  START_CAPABILITY

    #define FUNCTION generate_parameters    // Name of specific function providing the observable
    START_FUNCTION(parameters_map)          // Returns a map from strings to doubles
    #undef FUNCTION
    
  #undef CAPABILITY
 
#undef MODULE

#endif /* defined(__SUSYspecBit_rollcall_hpp__) */

