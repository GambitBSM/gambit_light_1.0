//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Frontend header generated by BOSS for GAMBIT backend gm2calc 1.3.0.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author The GAMBIT Collaboration
///
///  *********************************************

#include "gambit/Backends/backend_types/gm2calc_1_3_0/identification.hpp"

LOAD_LIBRARY

namespace Gambit
{
   namespace Backends
   {
      namespace gm2calc_1_3_0
      {
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell_susy_parameters MSSMNoFV_onshell_susy_parameters;
         }
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell MSSMNoFV_onshell;
         }
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell_mass_eigenstates MSSMNoFV_onshell_mass_eigenstates;
         }
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell_problems MSSMNoFV_onshell_problems;
         }
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell_physical MSSMNoFV_onshell_physical;
         }
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell_soft_parameters MSSMNoFV_onshell_soft_parameters;
         }
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::Error Error;
         }
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::ESetupError ESetupError;
         }
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::EInvalidInput EInvalidInput;
         }
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::EPhysicalProblem EPhysicalProblem;
         }
         namespace gm2calc
         {
            typedef ::gm2calc_1_3_0::gm2calc::EReadError EReadError;
         }
      }
   }
}

// Functions
BE_FUNCTION(calculate_amu_1loop, double, (const gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell&), "calculate_amu_1loop__BOSS_8", "calculate_amu_1loop")
BE_FUNCTION(calculate_amu_1loop_non_tan_beta_resummed, double, (const gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell&), "calculate_amu_1loop_non_tan_beta_resummed__BOSS_9", "calculate_amu_1loop_non_tan_beta_resummed")
BE_FUNCTION(calculate_amu_2loop, double, (const gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell&), "calculate_amu_2loop__BOSS_10", "calculate_amu_2loop")
BE_FUNCTION(calculate_amu_2loop_non_tan_beta_resummed, double, (const gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell&), "calculate_amu_2loop_non_tan_beta_resummed__BOSS_11", "calculate_amu_2loop_non_tan_beta_resummed")
BE_FUNCTION(calculate_uncertainty_amu_2loop, double, (const gm2calc_1_3_0::gm2calc::MSSMNoFV_onshell&), "calculate_uncertainty_amu_2loop__BOSS_17", "calculate_uncertainty_amu_2loop")

// Enums

// Variables

// Initialisation function (dependencies)

// Convenience functions (registration)

// Initialisation function (definition)
BE_INI_FUNCTION{} END_BE_INI_FUNCTION

// Convenience functions (definitions)

// End
#include "gambit/Backends/backend_undefs.hpp"
