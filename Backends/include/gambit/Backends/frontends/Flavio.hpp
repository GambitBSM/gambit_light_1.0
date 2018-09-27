//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Example of how to use the macros in
///  'backend_macros.hpp' to set up a frontend for
///  a Python library.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Marcin Chrzaszcz
///          (mchrzasz@cern.ch)
///  \date 2018 Sep
///
///  *********************************************


#define BACKENDNAME Flavio
#define BACKENDLANG Python        
#define VERSION 0.27.3

/* The following macro imports the modudle in the Python interpreter              
 * when this header file is included somewhere. */



LOAD_LIBRARY

BE_FUNCTION(sm_prediction, double, (string), "sm_prediction", "sm_prediction")
