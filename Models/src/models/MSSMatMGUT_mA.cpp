//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///
///  Translation function definitions for the
///  MSSM models with boundary conditions at
///  scale MGUT and with A pole mass and mu
///  as explicit input parameters instead of mHu2 and mHd2.
///
///  Contains the interpret-as-parent translation
///  functions for:
///
///  MSSM63atMGUT_mA  --> MSSM63atMGUT
///  MSSM30atMGUT_mA  --> MSSM63atMGUT_mA
///  MSSM25atMGUT_mA  --> MSSM30atMGUT_mA
///  MSSM20atMGUT_mA  --> MSSM25atMGUT_mA
///
///  As well as the interpret-as-friend translation
///  functions for:
///
///  MSSM30atMGUT_mA  --> MSSM30atMGUT
///  MSSM20atMGUT_mA  --> MSSM20atMGUT
///
///  *********************************************
///
///  Authors
///  =======
///
///  (add name and date if you modify)
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2015 Sep
///  \date 2017 Aug
///
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2017 Sep, Oct
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@kit.edu)
///  \date 2022 Sept
///
///  *********************************************

#include "gambit/Models/model_macros.hpp"
#include "gambit/Models/model_helpers.hpp"
#include "gambit/Logs/logger.hpp"
#include "gambit/Utils/numerical_constants.hpp"
#include "gambit/Elements/sminputs.hpp"
#include "gambit/Elements/spectrum.hpp"

#include "gambit/Models/models/MSSMatMGUT_mA.hpp"
#include "gambit/Models/models/MSSM_translation_helpers.hpp"

// Activate debug output
//#define MSSMatMGUT_mA_DBUG

// MSSM63atMGUT_mA --> MSSM63atMGUTA
#define MODEL MSSM63atMGUT_mA
#define PARENT MSSM63atMGUT
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     USE_MODEL_PIPE(PARENT)
     const SubSpectrum& HE = Dep::unimproved_MSSM_spectrum->get_HE();
     MSSM_mA_to_MSSM_mhud(myP, targetP, HE);

     // Done
     #ifdef MSSMatMGUT_mA_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM30atMGUT_mA --> MSSM63atMGUT_mA
#define MODEL MSSM30atMGUT_mA
#define PARENT MSSM63atMGUT_mA
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     MSSM30atX_to_MSSM63atX(myP, targetP);

     // Done
     #ifdef MSSMatMGUT_mA_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT

// MSSM30atMGUT_mA --> MSSM30atMGUT
#define FRIEND MSSM30atMGUT
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,FRIEND) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_X calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(FRIEND) "."<<LogTags::info<<EOM;

     USE_MODEL_PIPE(FRIEND) // Need the pipe for the TARGET model
     const SubSpectrum& HE = Dep::unimproved_MSSM_spectrum->get_HE();
     MSSM_mA_to_MSSM_mhud(myP, targetP, HE);

     // Done
     #ifdef MSSMatMGUT_mA_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(FRIEND) " parameters:" << targetP << std::endl;
     #endif
  }
#undef FRIEND
#undef MODEL

// MSSM25atMGUT_mA --> MSSM30atMGUT_mA
#define MODEL MSSM25atMGUT_mA
#define PARENT MSSM30atMGUT_mA
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     MSSM25atX_to_MSSM30atX(myP, targetP);

     // Done
     #ifdef MSSMatMGUT_mA_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM20atMGUT_mA --> MSSM25atMGUT_mA
#define MODEL MSSM20atMGUT_mA
#define PARENT MSSM25atMGUT_mA
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     MSSM20atX_to_MSSM25atX(myP, targetP);

     // Done
     #ifdef MSSMatMGUT_mA_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT

// MSSM20atMGUT_mA --> MSSM20atMGUT
#define FRIEND MSSM20atMGUT
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,FRIEND) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_X calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(FRIEND) "."<<LogTags::info<<EOM;

     USE_MODEL_PIPE(FRIEND) // Need the pipe for the TARGET model
     const SubSpectrum& HE = Dep::unimproved_MSSM_spectrum->get_HE();
     MSSM_mA_to_MSSM_mhud(myP, targetP, HE);

     // Done
     #ifdef MSSMatMGUT_mA_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(FRIEND) " parameters:" << targetP << std::endl;
     #endif
  }
#undef FRIEND
#undef MODEL

