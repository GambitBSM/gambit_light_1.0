#include "common.h"

struct background
{
  /** @name - input parameters initialized by user in input module
   *  (all other quantities are computed in this module, given these parameters
   *   and the content of the 'precision' structure)
   *
   * The background cosmological parameters listed here form a parameter
   * basis which is directly usable by the background module. Nothing
   * prevents from defining the input cosmological parameters
   * differently, and to pre-process them into this format, using the input
   * module (this might require iterative calls of background_init()
   * e.g. for dark energy or decaying dark matter). */

  //@{

  double H0; /**< \f$ H_0 \f$: Hubble parameter (in fact, [\f$H_0/c\f$]) in \f$ Mpc^{-1} \f$ */

  double Omega0_g; /**< \f$ \Omega_{0 \gamma} \f$: photons */

  double T_cmb; /**< \f$ T_{cmb} \f$: current CMB temperature in Kelvins */

  double Omega0_b; /**< \f$ \Omega_{0 b} \f$: baryons */

  double Omega0_cdm; /**< \f$ \Omega_{0 cdm} \f$: cold dark matter */

  double Omega0_lambda; /**< \f$ \Omega_{0_\Lambda} \f$: cosmological constant */

  double Omega0_fld; /**< \f$ \Omega_{0 de} \f$: fluid */
  double w0_fld; /**< \f$ w0_{DE} \f$: current fluid equation of state parameter */
  double wa_fld; /**< \f$ wa_{DE} \f$: fluid equation of state parameter derivative */

  double cs2_fld; /**< \f$ c^2_{s~DE} \f$: sound speed of the fluid
		     in the frame comoving with the fluid (so, this is
		     not [delta p/delta rho] in the synchronous or
		     newtonian gauge!!!) */

  short use_ppf; /**< flag switching on PPF perturbation equations
                    instead of true fluid equations for
                    perturbations. It could have been defined inside
                    perturbation structure, but we leave it here in
                    such way to have all fld parameters grouped. */

  double c_gamma_over_c_fld; /**< ppf parameter defined in eq. (16) of 0808.3125 [astro-ph] */

  double Omega0_ur; /**< \f$ \Omega_{0 \nu r} \f$: ultra-relativistic neutrinos */

  double Omega0_dcdmdr; /**< \f$ \Omega_{0 dcdm}+\Omega_{0 dr} \f$: decaying cold dark matter (dcdm) decaying to dark radiation (dr) */

  double Gamma_dcdm; /**< \f$ \Gamma_{dcdm} \f$: decay constant for decaying cold dark matter */

  double Omega_ini_dcdm;    /**< \f$ \Omega_{ini,dcdm} \f$: rescaled initial value for dcdm density (see 1407.2418 for definitions) */

  double Omega0_scf;        /**< \f$ \Omega_{0 scf} \f$: scalar field */
  short attractor_ic_scf;   /**< whether the scalar field has attractor initial conditions */
  double phi_ini_scf;       /**< \f$ \phi(t_0) \f$: scalar field initial value */
  double phi_prime_ini_scf; /**< \f$ d\phi(t_0)/d\tau \f$: scalar field initial derivative wrt conformal time */
  double * scf_parameters;  /**< list of parameters describing the scalar field potential */
  int scf_parameters_size;  /**< size of scf_parameters */
  int scf_tuning_index;     /**< index in scf_parameters used for tuning */
  //double scf_lambda; /**< \f$ \lambda \f$ : scalar field exponential potential slope */
  //double scf_alpha;  /**< \f$ \alpha \f$ : Albrecht-Skordis polynomial slope */
  //double scf_B; /**< \f$ \alpha \f$ : Albrecht-Skordis field shift */
  //double scf_A; /**< \f$ \alpha \f$ : Albrecht-Skordis offset */

  double Omega0_k; /**< \f$ \Omega_{0_k} \f$: curvature contribution */

  int N_ncdm;                            /**< Number of distinguishable ncdm species */
  double * M_ncdm;                       /**< vector of masses of non-cold relic:
                                             dimensionless ratios m_ncdm/T_ncdm */
  double * Omega0_ncdm, Omega0_ncdm_tot; /**< Omega0_ncdm for each species and for the total Omega0_ncdm */
  double * deg_ncdm, deg_ncdm_default;   /**< vector of degeneracy parameters in factor
                                             of p-s-d: 1 for one family of neutrinos
                                             (= one neutrino plus its anti-neutrino,
                                             total g*=1+1=2, so deg = 0.5 g*); and its
					     default value */

  /* the following parameters help to define the analytical ncdm phase space distributions (p-s-d) */
  double * T_ncdm,T_ncdm_default;       /**< list of 1st parameters in
					     p-s-d of non-cold relics:
					     relative temperature
					     T_ncdm1/T_gamma; and its
					     default value */
  double * ksi_ncdm, ksi_ncdm_default;  /**< list of 2nd parameters in
					     p-s-d of non-cold relics:
					     relative chemical potential
					     ksi_ncdm1/T_ncdm1; and its
					     default value */
  double * ncdm_psd_parameters;         /**< list of parameters for specifying/modifying
                                             ncdm p.s.d.'s, to be customized for given model
                                             (could be e.g. mixing angles) */
  /* end of parameters for analytical ncdm p-s-d */

  /* the following parameters help to define tabulated ncdm p-s-d passed in file */
  int * got_files;                      /**< list of flags for each species, set to true if
					     p-s-d is passed through file */
  char * ncdm_psd_files;                /**< list of filenames for tabulated p-s-d */
  /* end of parameters for tabulated ncdm p-s-d */

  //@}

  /** @name - related parameters */

  //@{

  double h; /**< reduced Hubble parameter */
  double age; /**< age in Gyears */
  double conformal_age; /**< conformal age in Mpc */
  double K; /**< \f$ K \f$: Curvature parameter \f$ K=-\Omega0_k*a_{today}^2*H_0^2\f$; */
  int sgnK; /**< K/|K|: -1, 0 or 1 */
  double * m_ncdm_in_eV; /**< list of ncdm masses in eV (inferred from M_ncdm and other parameters above) */
  double Neff; /**< so-called "effective neutrino number", computed at earliest time in interpolation table */
  double Omega0_dcdm; /**< \f$ \Omega_{0 dcdm} \f$: decaying cold dark matter */
  double Omega0_dr; /**< \f$ \Omega_{0 dr} \f$: decay radiation */


  //@}

  /** @name - other background parameters */

  //@{

  double a_today; /**< scale factor today (arbitrary and irrelevant for most purposes) */

  //@}

  /** @name - all indices for the vector of background (=bg) quantities stored in table */

  //@{

  int index_bg_a;             /**< scale factor */
  int index_bg_H;             /**< Hubble parameter in \f$Mpc^{-1}\f$ */
  int index_bg_H_prime;       /**< its derivative w.r.t. conformal time */

  /* end of vector in short format, now quantities in normal format */

  int index_bg_rho_g;         /**< photon density */
  int index_bg_rho_b;         /**< baryon density */
  int index_bg_rho_cdm;       /**< cdm density */
  int index_bg_rho_lambda;    /**< cosmological constant density */
  int index_bg_rho_fld;       /**< fluid density */
  int index_bg_w_fld;         /**< fluid equation of state */
  int index_bg_rho_ur;        /**< relativistic neutrinos/relics density */
  int index_bg_rho_dcdm;      /**< dcdm density */
  int index_bg_rho_dr;        /**< dr density */

  int index_bg_phi_scf;       /**< scalar field value */
  int index_bg_phi_prime_scf; /**< scalar field derivative wrt conformal time */
  int index_bg_V_scf;         /**< scalar field potential V */
  int index_bg_dV_scf;        /**< scalar field potential derivative V' */
  int index_bg_ddV_scf;       /**< scalar field potential second derivative V'' */
  int index_bg_rho_scf;       /**< scalar field energy density */
  int index_bg_p_scf;         /**< scalar field pressure */

  int index_bg_rho_ncdm1;     /**< density of first ncdm species (others contiguous) */
  int index_bg_p_ncdm1;       /**< pressure of first ncdm species (others contiguous) */
  int index_bg_pseudo_p_ncdm1;/**< another statistical momentum useful in ncdma approximation */

  int index_bg_Omega_r;       /**< relativistic density fraction (\f$ \Omega_{\gamma} + \Omega_{\nu r} \f$) */

  /* end of vector in normal format, now quantities in long format */

  int index_bg_rho_crit;      /**< critical density */
  int index_bg_Omega_m;       /**< non-relativistic density fraction (\f$ \Omega_b + \Omega_cdm + \Omega_{\nu nr} \f$) */
  int index_bg_conf_distance; /**< conformal distance (from us) in Mpc */
  int index_bg_ang_distance;  /**< angular diameter distance in Mpc */
  int index_bg_lum_distance;  /**< luminosity distance in Mpc */
  int index_bg_time;          /**< proper (cosmological) time in Mpc */
  int index_bg_rs;            /**< comoving sound horizon in Mpc */

  int index_bg_D;             /**< scale independent growth factor D(a) for CDM perturbations */
  int index_bg_f;             /**< corresponding velocity growth factor [dlnD]/[dln a] */

  int bg_size_short;  /**< size of background vector in the "short format" */
  int bg_size_normal; /**< size of background vector in the "normal format" */
  int bg_size;        /**< size of background vector in the "long format" */

  //@}

  /** @name - background interpolation tables */

  //@{

  int bt_size;               /**< number of lines (i.e. time-steps) in the array */
  double * tau_table;        /**< vector tau_table[index_tau] with values of \f$ \tau \f$ (conformal time) */
  double * z_table;          /**< vector z_table[index_tau] with values of \f$ z \f$ (redshift) */
  double * background_table; /**< table background_table[index_tau*pba->bg_size+pba->index_bg] with all other quantities (array of size bg_size*bt_size) **/

  //@}

  /** @name - table of their second derivatives, used for spline interpolation */

  //@{

  double * d2tau_dz2_table; /**< vector d2tau_dz2_table[index_tau] with values of \f$ d^2 \tau / dz^2 \f$ (conformal time) */
  double * d2background_dtau2_table; /**< table d2background_dtau2_table[index_tau*pba->bg_size+pba->index_bg] with values of \f$ d^2 b_i / d\tau^2 \f$ (conformal time) */

  //@}


  /** @name - all indices for the vector of background quantities to be integrated (=bi)
   *
   * Most background quantities can be immediately inferred from the
   * scale factor. Only few of them require an integration with
   * respect to conformal time (in the minimal case, only one quantity needs to
   * be integrated with time: the scale factor, using the Friedmann
   * equation). These indices refer to the vector of
   * quantities to be integrated with time.
   * {B} quantities are needed by background_functions() while {C} quantities are not.
   */

  //@{

  int index_bi_a;       /**< {B} scale factor */
  int index_bi_rho_dcdm;/**< {B} dcdm density */
  int index_bi_rho_dr;  /**< {B} dr density */
  int index_bi_rho_fld; /**< {B} fluid density */
  int index_bi_phi_scf;       /**< {B} scalar field value */
  int index_bi_phi_prime_scf; /**< {B} scalar field derivative wrt conformal time */

  int index_bi_time;    /**< {C} proper (cosmological) time in Mpc */
  int index_bi_rs;      /**< {C} sound horizon */
  int index_bi_tau;     /**< {C} conformal time in Mpc */
  int index_bi_D;       /**< {C} scale independent growth factor D(a) for CDM perturbations. */
  int index_bi_D_prime; /**< {C} D satisfies \f$ [D''(\tau)=-aHD'(\tau)+3/2 a^2 \rho_M D(\tau) \f$ */

  int bi_B_size;        /**< Number of {B} parameters */
  int bi_size;          /**< Number of {B}+{C} parameters */

  //@}

  /** @name - flags describing the absence or presence of cosmological
      ingredients
      *
      * having one of these flag set to zero allows to skip the
      * corresponding contributions, instead of adding null contributions.
      */


  //@{

  short has_cdm;       /**< presence of cold dark matter? */
  short has_dcdm;      /**< presence of decaying cold dark matter? */
  short has_dr;        /**< presence of relativistic decay radiation? */
  short has_scf;       /**< presence of a scalar field? */
  short has_ncdm;      /**< presence of non-cold dark matter? */
  short has_lambda;    /**< presence of cosmological constant? */
  short has_fld;       /**< presence of fluid with constant w and cs2? */
  short has_ur;        /**< presence of ultra-relativistic neutrinos/relics? */
  short has_curvature; /**< presence of global spatial curvature? */

  //@}

  /**
   *@name - arrays related to sampling and integration of ncdm phase space distributions
   */


  //@{
  int * ncdm_quadrature_strategy; /**< Vector of integers according to quadrature strategy. */
  int * ncdm_input_q_size; /**< Vector of numbers of q bins */
  double * ncdm_qmax;   /**< Vector of maximum value of q */
  double ** q_ncdm_bg;  /**< Pointers to vectors of background sampling in q */
  double ** w_ncdm_bg;  /**< Pointers to vectors of corresponding quadrature weights w */
  double ** q_ncdm;     /**< Pointers to vectors of perturbation sampling in q */
  double ** w_ncdm;     /**< Pointers to vectors of corresponding quadrature weights w */
  double ** dlnf0_dlnq_ncdm; /**< Pointers to vectors of logarithmic derivatives of p-s-d */
  int * q_size_ncdm_bg; /**< Size of the q_ncdm_bg arrays */
  int * q_size_ncdm;    /**< Size of the q_ncdm arrays */
  double * factor_ncdm; /**< List of normalization factors for calculating energy density etc.*/

  //@}

  /**
   *@name - some flags needed for calling background functions
   */

  //@{

  short short_info;  /**< flag for calling background_at_eta and return little information */
  short normal_info; /**< flag for calling background_at_eta and return medium information */
  short long_info;   /**< flag for calling background_at_eta and return all information */

  short inter_normal;  /**< flag for calling background_at_eta and find position in interpolation table normally */
  short inter_closeby; /**< flag for calling background_at_eta and find position in interpolation table starting from previous position in previous call */

  //@}

  /** @name - technical parameters */

  //@{

  short shooting_failed;  /**< flag is set to true if shooting failed. */

  ErrorMsg shooting_error; /**< Error message from shooting failed. */

  short background_verbose; /**< flag regulating the amount of information sent to standard output (none if set to zero) */

  ErrorMsg error_message; /**< zone for writing error messages */

  //@}
};
