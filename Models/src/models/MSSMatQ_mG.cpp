//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///
///  Translation function definitions for the
///  MSSM models with boundary conditions at
///  a generic scale Q and with a light gravitino
///
///  Contains the interpret-as-parent translation
///  functions for:
///
///  MSSM30atQ_mG   --> MSSM63atQ_mG
///  MSSM25atQ_mG   --> MSSM30atQ_mG
///  MSSM24atQ_mG   --> MSSM25atQ_mG
///  MSSM20atQ_mG   --> MSSM25atQ_mG
///  MSSM19atQ_mG   --> MSSM24atQ_mG
///  MSSM16atQ_mG   --> MSSM19atQ_mG
///  MSSM15atQ_mG   --> MSSM16atQ_mG
///  MSSM11atQ_mG   --> MSSM16atQ_mG
///  MSSM10atQ_mG   --> MSSM11atQ_mG
///  MSSM10batQ_mG  --> MSSM11atQ_mG
///  MSSM10catQ_mG  --> MSSM15atQ_mG
///  MSSM9atQ_mG    --> MSSM10atQ_mG
///  MSSM9batQ_mG   --> MSSM15atQ_mG
///  MSSM7atQ_mG    --> MSSM9atQ_mG
///
///  as well as the interpret-as-friend translation
///  functions for
///
///  MSSM19atQ_mG   --> MSSM20atQ_mG
///  MSSM9atQ_mG    --> MSSM10batQ_mG
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
///  \date 2018 Oct
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

#include "gambit/Models/models/MSSMatQ_mG.hpp"
#include "gambit/Models/models/MSSM_translation_helpers.hpp"

// Activate debug output
//#define MSSMatQ_mG_DBUG

// MSSM30atQ_mG --> MSSM63atQ_mG
#define MODEL MSSM30atQ_mG
#define PARENT MSSM63atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     MSSM30atX_to_MSSM63atX(myP, targetP);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM25atQ_mG --> MSSM30atQ_mG
#define MODEL MSSM25atQ_mG
#define PARENT MSSM30atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     MSSM25atX_to_MSSM30atX(myP, targetP);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL


// MSSM24atQ_mG --> MSSM25atQ_mG
#define MODEL MSSM24atQ_mG
#define PARENT MSSM25atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     targetP.setValues(myP);
     // Set 25th parameter (1st/2nd gen trilinear) in parent to zero.
     targetP.setValue("Ae_12",  0.0 );

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM20atQ_mG --> MSSM25atQ_mG
#define MODEL MSSM20atQ_mG
#define PARENT MSSM25atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     MSSM20atX_to_MSSM25atX(myP, targetP);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM19atQ_mG --> MSSM24atQ_mG
#define MODEL MSSM19atQ_mG
#define PARENT MSSM24atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     // Ignore that some parameters don't exist in the parent, as these are set below.
     targetP.setValues(myP,false);

     // RH squark soft masses, gen 1 and 2
     targetP.setValue("mq2_1",  myP["mq2_12"] ); // mq2_11 in MSSM63
     targetP.setValue("mq2_2",  myP["mq2_12"] ); // mq2_22   " "
     // RH slepton soft masses, gen 1 and 2
     targetP.setValue("ml2_1",  myP["ml2_12"] ); // ml2_11 in MSSM63
     targetP.setValue("ml2_2",  myP["ml2_12"] ); // ml2_22   " "
     // LH down-type squark soft masses
     targetP.setValue("md2_1",  myP["md2_12"] ); // ml2_11 in MSSM63
     targetP.setValue("md2_2",  myP["md2_12"] ); // ml2_22   " "
     // LH up-type squark soft masses
     targetP.setValue("mu2_1",  myP["mu2_12"] ); // mu2_11 in MSSM63
     targetP.setValue("mu2_2",  myP["mu2_12"] ); // mu2_22   " "
     // LH charged slepton soft masses
     targetP.setValue("me2_1",  myP["me2_12"] ); // me2_11 in MSSM63
     targetP.setValue("me2_2",  myP["me2_12"] ); // me2_22   " "

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT

// MSSM19atQ_mG --> MSSM20atQ_mG
#define FRIEND MSSM20atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,FRIEND) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_x calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(FRIEND) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in the friend model.
     targetP.setValues(myP);
     // Set 20th parameter (1st/2nd gen trilinear) in friend to zero.
     targetP.setValue("Ae_12",  0.0 );

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(FRIEND) " parameters:" << targetP << std::endl;
     #endif
  }
#undef FRIEND
#undef MODEL

// MSSM16atQ_mG --> MSSM19atQ_mG
#define MODEL MSSM16atQ_mG
#define PARENT MSSM19atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     targetP.setValues(myP);

     // LH first and second gen down-type squark, up-type squark and charged slepton soft masses
     targetP.setValue("md2_12", myP["mq2_12"]);
     targetP.setValue("mu2_12", myP["mq2_12"]);
     targetP.setValue("me2_12", myP["ml2_12"]);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM15atQ_mG --> MSSM16atQ_mG
#define MODEL MSSM15atQ_mG
#define PARENT MSSM16atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     // Ignore that some parameters don't exist in the parent, as these are set below.
     targetP.setValues(myP,false);

     // 3rd gen trilinear couplings.
     targetP.setValue("Ae_3", myP["A0"]);
     targetP.setValue("Ad_3", myP["A0"]);
     targetP.setValue("Au_3", myP["Au_3"]);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM11atQ_mG --> MSSM16atQ_mG
#define MODEL MSSM11atQ_mG
#define PARENT MSSM16atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     // Ignore that some parameters don't exist in the parent, as these are set below.
     targetP.setValues(myP,false);

     // Sfermion mass matrix entries.
     targetP.setValue("mq2_12", myP["mq2"]);
     targetP.setValue("mq2_3",  myP["mq2"]);
     targetP.setValue("mu2_3",  myP["mq2"]);
     targetP.setValue("md2_3",  myP["mq2"]);
     targetP.setValue("ml2_12", myP["ml2"]);
     targetP.setValue("ml2_3",  myP["ml2"]);
     targetP.setValue("me2_3",  myP["ml2"]);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM10atQ_mG --> MSSM11atQ_mG
#define MODEL MSSM10atQ_mG
#define PARENT MSSM11atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     targetP.setValues(myP);

     // Charged slepton trilinear coupling
     targetP.setValue("Ae_3", 0.0);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM10batQ_mG --> MSSM11atQ_mG
#define MODEL MSSM10batQ_mG
#define PARENT MSSM11atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     // Ignore that some parameters don't exist in the parent, as these are set below.
     targetP.setValues(myP,false);

     // Sfermion mass matrix entries.
     targetP.setValue("mq2", myP["mf2"]);
     targetP.setValue("ml2", myP["mf2"]);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM10catQ_mG --> MSSM15atQ_mG
#define MODEL MSSM10catQ_mG
#define PARENT MSSM15atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     // Ignore that some parameters don't exist in the parent, as these are set below.
     targetP.setValues(myP,false);

     // Sfermion masses
     set_many_to_one(targetP, initVector<str>("mu2_3", "md2_3"), myP["mq2_3"]);
     set_many_to_one(targetP, initVector<str>("ml2_12", "ml2_3", "me2_3"), myP["ml2"]);

     // 3rd gen up-type trilinear coupling.
     targetP.setValue("Au_3", myP["A0"]);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL

// MSSM9atQ_mG --> MSSM10atQ_mG
#define MODEL MSSM9atQ_mG
#define PARENT MSSM10atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     // Ignore that some parameters don't exist in the parent, as these are set below.
     targetP.setValues(myP,false);

     // Sfermion mass matrix entries.
     targetP.setValue("mq2", myP["mf2"]);
     targetP.setValue("ml2", myP["mf2"]);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT

// MSSM9atQ_mG --> MSSM10batQ_mG
#define FRIEND MSSM10batQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,FRIEND) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_x calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(FRIEND) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     // Ignore that some parameters don't exist in the parent, as these are set below.
     targetP.setValues(myP,false);

     // Charged slepton trilinear coupling
     targetP.setValue("Ae_3", 0.0);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(FRIEND) " parameters:" << targetP << std::endl;
     #endif
  }
#undef FRIEND
#undef MODEL

// MSSM9batQ_mG --> MSSM15atQ_mG
#define MODEL MSSM9batQ_mG
#define PARENT MSSM15atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     // Ignore that some parameters don't exist in the parent, as these are set below.
     targetP.setValues(myP,false);

     // most Sfermion soft masses squared
     set_many_to_one(targetP, initVector<str>("md2_3", "me2_3", "ml2_12", "ml2_3", "mq2_12"), myP["msf2"]);
     // stop soft masses squared
     targetP.setValue("mu2_3", myP["mq2_3"]);
     // set all trilinear coupling except Au_3 to zero
     targetP.setValue("A0", 0.0);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL


/// MSSM7atQ_mG --> MSSM9atQ_mG
#define MODEL MSSM7atQ_mG
#define PARENT MSSM9atQ_mG
  void MODEL_NAMESPACE::CAT_3(MODEL,_to_,PARENT) (const ModelParameters &myP, ModelParameters &targetP)
  {
     USE_MODEL_PIPE(PARENT)

     logger()<<"Running interpret_as_parent calculations for " STRINGIFY(MODEL) " --> " STRINGIFY(PARENT) "."<<LogTags::info<<EOM;

     // Send all parameter values upstream to matching parameters in parent.
     // Ignore that some parameters don't exist in the parent, as these are set below.
     targetP.setValues(myP,false);

     // Gaugino masses
     double mz = Dep::SMINPUTS->mZ;
     double am1 = Dep::SMINPUTS->alphainv;
     /// At tree level
     /// 0.25* ( sin(2\theta_W) )^2 = pi / (root2*mz*mz*am1*Dep::SMINPUTS->GF)
     /// solve quadratic eqn for (sin \theta_W)^2 = sin2thetaW_tree
     double sin2thetaW_tree = 0.5 - sqrt(0.25 - pi / (root2*mz*mz*am1*Dep::SMINPUTS->GF));
     targetP.setValue("M1", myP["M2"] * 5.0/3.0 * sin2thetaW_tree / (1.0-sin2thetaW_tree));
     targetP.setValue("M3", myP["M2"] * Dep::SMINPUTS->alphaS * am1 * sin2thetaW_tree);

     // Done
     #ifdef MSSMatQ_mG_DBUG
       std::cout << STRINGIFY(MODEL) " parameters:" << myP << std::endl;
       std::cout << STRINGIFY(PARENT) " parameters:" << targetP << std::endl;
     #endif
  }
#undef PARENT
#undef MODEL
