//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  GAMBIT_light interface library (C/C++, Fortra, Python backends)
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date 2022, 2023 Apr
///  \author Marcin Krotkiewski
///          (marcin.krotkiewski@usit.uio.no)
///  \date 2022 Sep
///
///  *********************************************


#define BACKENDNAME light_interface
#define BACKENDLANG CXX
#define VERSION 0.1
#define SAFE_VERSION 0_1
#define REFERENCE GAMBIT:2017yxo

#include "gambit/Utils/util_types.hpp"

/* The following macro loads the library using dlopen
 * when this header file is included somewhere. */

LOAD_LIBRARY

/* Next we use macros BE_VARIABLE and BE_FUNCTION to load pointers
 * (using dlsym) to the variables and functions within the library.
 *
 * The macros also create functors that wrap the library variables and functions.
 * These are used by the Core for dependency resolution and to set up a suitable
 * interface to the library functions/variables at module level. */

/* Syntax for BE_FUNCTION:
 * BE_FUNCTION([choose function name], [type], [arguement types], "[exact symbol name]", "[choose capability name]")
 */

BE_FUNCTION(run_user_loglikes, void, (const map_str_dbl&, map_str_dbl&, vec_str&), "run_user_loglikes", "run_user_loglikes")
BE_FUNCTION(init_user_lib_C_CXX_Fortran, void, (const std::string &, const std::string &, const std::string &, const std::string &, const std::vector<std::string> &, const std::vector<std::string> &), "init_user_lib_C_CXX_Fortran", "init_user_lib_C_CXX_Fortran")
BE_FUNCTION(init_user_lib_Python, void, (const std::string &, const std::string &, const std::string &, const std::string &, const std::vector<std::string> &, const std::vector<std::string> &), "init_user_lib_Python", "init_user_lib_Python")

BE_CONV_FUNCTION(run_light_interface, void, (const map_str_dbl&, map_str_dbl&), "run_light_interface")
BE_CONV_FUNCTION(get_input_par_name_pairs, vec_pair_str_str, (), "get_input_par_name_pairs")

/* Syntax for BE_VARIABLE:
 * BE_VARIABLE([name], [type], "[exact symbol name]", "[choose capability name]")
 * */


// Undefine macros to avoid conflict with other backends
#include "gambit/Backends/backend_undefs.hpp"
