//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Declaration of all cosmology-related nuisance
///  parameter models.
///
///
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///
///  \author Janina Renk
///          (janina.renk@fysik.su.se)
///  \date 2019 June
///  *********************************************

#ifndef __CosmoNuisanceModels_hpp__
#define __CosmoNuisanceModels_hpp__

/* PLANCK NUISANCE PARAMETERS */
/// Planck - Full set of nuisance parameter for the high-l TTTEEE likelihood
#define MODEL cosmo_nuisance_Planck_TTTEEE
  START_MODEL
  DEFINEPARS(A_cib_217,cib_index,xi_sz_cib,A_sz,ps_A_100_100,ps_A_143_143,ps_A_143_217,ps_A_217_217,ksz_norm,gal545_A_100,gal545_A_143,gal545_A_143_217,gal545_A_217,galf_EE_A_100,galf_EE_A_100_143,galf_EE_A_100_217,galf_EE_A_143,galf_EE_A_143_217,galf_EE_A_217,galf_EE_index,galf_TE_A_100,galf_TE_A_100_143,galf_TE_A_100_217,galf_TE_A_143,galf_TE_A_143_217,galf_TE_A_217,galf_TE_index,calib_100T,calib_217T,calib_100P,calib_143P,calib_217P,A_pol,A_planck)
#undef MODEL

/// Planck - Full set of nuisance parameter for the high-l TT likelihood
#define MODEL cosmo_nuisance_Planck_TT
  START_MODEL
  DEFINEPARS(A_cib_217,cib_index,xi_sz_cib,A_sz,ps_A_100_100,ps_A_143_143,ps_A_143_217,ps_A_217_217,ksz_norm,gal545_A_100,gal545_A_143,gal545_A_143_217,gal545_A_217,calib_100T,calib_217T,A_planck)
#undef MODEL

/// Planck - Lite version of the likelihoods
#define MODEL cosmo_nuisance_Planck_lite
  START_MODEL
  DEFINEPARS(A_planck)
#undef MODEL

/// Supernovae -- JLA
#define MODEL cosmo_nuisance_JLA
  START_MODEL
  DEFINEPARS(alpha,beta,M,Delta_M)
#undef MODEL

/// Pantheon -> child of JLA (light curve params fitted with SaltMu2 therefore only 1 nuisance param)
/// use same model for JLA_simple likelihood
#define MODEL cosmo_nuisance_Pantheon
  #define PARENT cosmo_nuisance_JLA
    START_MODEL
    DEFINEPARS(M)
    INTERPRET_AS_PARENT_FUNCTION(cosmo_nuisance_Pantheon_to_cosmo_nuisance_JLA)
  #undef PARENT
#undef MODEL

/// nuisance params for acbar likelihood implemented in MontePython
#define MODEL cosmo_nuisance_acbar
  START_MODEL
  DEFINEPARS(A_SZ)
#undef MODEL

/// nuisance params for bicep/keck array BK14 likelihood implemented in MontePython
#define MODEL cosmo_nuisance_BK14
  START_MODEL
  DEFINEPARS(BBdust,BBsync,BBalphadust,BBbetadust,BBTdust,BBalphasync,BBbetasync,BBdustsynccorr,EEtoBB_dust,EEtoBB_sync)
#undef MODEL

/// nuisance params for bicep/keck array BK14priors likelihood implemented in MontePython
#define MODEL cosmo_nuisance_BK14priors
  #define PARENT cosmo_nuisance_BK14
    START_MODEL
    DEFINEPARS(BBbetadust,BBbetasync)
    INTERPRET_AS_PARENT_FUNCTION(cosmo_nuisance_BK14priors_to_cosmo_nuisance_BK14)
  #undef PARENT
#undef MODEL

/// nuisance params for CFHTLenS tomographic weak lensing likelihood implemented in MontePython
#define MODEL cosmo_nuisance_CFHTLens_correlation
  START_MODEL
  // parameter name in MP likelihood: epsilon renamed to epsilon_CFHT, have to translate back before passing to MP
  // (we can not have several parameters with the same name in GAMBIT)
  DEFINEPARS(epsilon_CFHT)
#undef MODEL

/// nuisance params for euclid lensing likelihood implemented in MontePython
#define MODEL cosmo_nuisance_euclid_lensing
  START_MODEL
  // parameter name in MP likelihood: epsilon renamed to epsilon_euclid, have to translate back before passing to MP
  // (we can not have several parameters with the same name in GAMBIT)
  DEFINEPARS(epsilon_euclid)
#undef MODEL

/// nuisance params for euclid galaxy clustering likelihood implemented in MontePython
#define MODEL cosmo_nuisance_euclid_pk
  START_MODEL
  // actual parameter names: beta_0^Euclid,beta_1^Euclid but macros don't like ^ -> have to take care of renaming when
  // filling data.mcmc_parameters dictionary for MontePython (TODO)
  // parameter name in MP likelihood: sigma_euclid renamed to sigma_NL_euclid, have to translate back before passing to MP
  // (we can not have several parameters with the same name in GAMBIT)
  DEFINEPARS(sigma_NL_euclid,beta_0Euclid,beta_1Euclid,P_shot)
#undef MODEL

/// euclid pk nuisance parameters without shot noise parameter P_shot -> child of 
/// cosmo_nuisance_euclid_pk
#define MODEL cosmo_nuisance_euclid_pk_noShot
  #define PARENT cosmo_nuisance_euclid_pk
    START_MODEL
    DEFINEPARS(sigma_NL_euclid,beta_0Euclid,beta_1Euclid)
    INTERPRET_AS_PARENT_FUNCTION(cosmo_nuisance_euclid_pk_noShot_to_cosmo_nuisance_euclid_pk)
  #undef PARENT
#undef MODEL

/// nuisance params for tomographic ISW likelihood implemented in MontePython
#define MODEL cosmo_nuisance_ISW
  START_MODEL
  DEFINEPARS(A_ISW,b0_sdss,b1_sdss,b2_sdss,b3_sdss,b4_sdss,b0_qso,b1_qso,b2_qso,b0_mpz,b1_mpz,b2_mpz,b0_wisc,b1_wisc,b2_wisc,b0_nvss)
#undef MODEL

/// nuisance params for KiDS weak lensing likelihood implemented in MontePython
#define MODEL cosmo_nuisance_kids450_qe_likelihood_public
  START_MODEL
  DEFINEPARS(m_corr,A_IA,exp_IA,A_bary,A_noise_z1,A_noise_z2,A_noise_z3)
  // D_z1,D_z2,D_z3 params were not used in public analysis, don't think they are taken into account in the public
  // likelihood calculation either
#undef MODEL

/// nuisance params for Lyalpha likelihood Lya_abg implemented in MontePython
// commented out because we currently don't support this likelihood due to 
// issues in the implementation (not following MP guidelines so it does not 
// work through the GAMBIT interface). If you fix these issues you can uncomment
// the lines below to have the model for the nuisance parameters available
//#define MODEL cosmo_nuisance_Lya_abg
//  START_MODEL
//  DEFINEPARS(T0a,T0s,gamma_a,gamma_s,Fz1,Fz2,Fz3,Fz4,F_UV)
//#undef MODEL

/// contains nuisance params from ska1_IM_band1,ska1_IM_band2 and ska1_pk likelihoods implemented in MontePython
/// use this model for a combination of a 'band' and the pk likelihood
#define MODEL cosmo_nuisance_ska1
  START_MODEL
  // actual parameter names: beta_0^IM,beta_1^IM,beta_0^SKA1,beta_1^SKA1,beta_0^SKA2,beta_1^SKA2 but macros don't like ^ -> have to take care of renaming when
  // filling data.mcmc_parameters dictionary for MontePython (TODO)
  // parameter name in MP likelihood: epsilon,sigma_NL renamed to epsilon_ska & sigma_NL_ska, have to translate back before passing to MP
  // (we can not have several parameters with the same name in GAMBIT)
  DEFINEPARS(sigma_NL_ska,beta_0IM,beta_1IM,Omega_HI0,alpha_HI,beta_0SKA1,beta_1SKA1)
#undef MODEL

/// child of cosmo_nuisance_ska1 model containing only parameter for ska1_IM_band1 and ka1_IM_band1 likelihoods implemented in MontePython
#define MODEL cosmo_nuisance_ska1_IM_band
  #define PARENT cosmo_nuisance_ska1
    START_MODEL
    // actual parameter names: beta_0^IM,beta_1^IM,beta_0^SKA1,beta_1^SKA1,beta_0^SKA2,beta_1^SKA2 but macros don't like ^ -> have to take care of renaming when
    // filling data.mcmc_parameters dictionary for MontePython
    // parameter name in MP likelihood: epsilon,sigma_NL renamed to epsilon_ska & sigma_NL_ska, have to translate back before passing to MP
    // (we can not have several parameters with the same name in GAMBIT)
    DEFINEPARS(sigma_NL_ska,beta_0IM,beta_1IM,Omega_HI0,alpha_HI)
    INTERPRET_AS_PARENT_FUNCTION(cosmo_nuisance_ska1_IM_band_to_cosmo_nuisance_ska1)
  #undef PARENT
#undef MODEL

/// child of cosmo_nuisance_ska1_IM_band model containing only parameter for ska1_IM_band1 and ka1_IM_band1 likelihood implemented in MontePython
/// excluding nuisance parameters Omega_HI0 and alphaHI (energy density and redshift dependence of neutral hydrogen)
#define MODEL cosmo_nuisance_ska1_IM_band_noHI
  #define PARENT cosmo_nuisance_ska1_IM_band
    START_MODEL
    DEFINEPARS(sigma_NL_ska,beta_0IM,beta_1IM)
    INTERPRET_AS_PARENT_FUNCTION(cosmo_nuisance_ska1_IM_band_noHI_to_cosmo_nuisance_ska1_IM_band)
  #undef PARENT
#undef MODEL

/// child of cosmo_nuisance_ska model containing only parameter for ska_pk likelihood implemented in MontePython
#define MODEL cosmo_nuisance_ska1_pk
  #define PARENT cosmo_nuisance_ska1
    START_MODEL
    DEFINEPARS(sigma_NL_ska,beta_0SKA1,beta_1SKA1)
    INTERPRET_AS_PARENT_FUNCTION(cosmo_nuisance_ska1_pk_to_cosmo_nuisance_ska1)
  #undef PARENT
#undef MODEL

/// nuisance parameters for ska1_lensing and ska2_lensing likelihoods implemented in MontePython
#define MODEL cosmo_nuisance_ska_lensing
  START_MODEL
  DEFINEPARS(epsilon_ska)
#undef MODEL

/// model containing only parameter for ska2_pk likelihood implemented in MontePython
#define MODEL cosmo_nuisance_ska2_pk
  START_MODEL
  DEFINEPARS(sigma_NL_ska,beta_0SKA2,beta_1SKA2)
#undef MODEL

/// nuisance params for spt and spt_2500 likelihood implemented in MontePython
#define MODEL cosmo_nuisance_spt
  START_MODEL
  DEFINEPARS(SPT_SZ,SPT_PS,SPT_CL)
#undef MODEL

/// add new model holding cosmological nuisance parameters
#define MODEL cosmo_nuisance_dummy
  START_MODEL
  DEFINEPARS(nuisance_param_1,nuisance_param_2,nuisance_param_3,nuisance_param_4,nuisance_param_5)
  DEFINEPARS(nuisance_param_6,nuisance_param_7,nuisance_param_8,nuisance_param_9,nuisance_param_10)
#undef MODEL


#endif
