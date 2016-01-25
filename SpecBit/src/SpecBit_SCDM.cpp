//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions of module SpecBit
///
///  These functions link ModelParameters to 
///  Spectrum objects in various ways (by running
///  spectrum generators, etc.)
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///    \date 2014 Sep - Dec, 2015 Jan - Mar
///  
///  \author Christopher Rogan
///          (christophersrogan@gmail.com)
///  \date 2015 Apr
///
///  \author James McKay
///           (j.mckay14@imperial.ac.uk)
///
///  \date 2015 Nov
///
///  *********************************************

#include <string>
#include <sstream>

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/Elements/spectrum.hpp"
#include "gambit/Utils/stream_overloads.hpp" // Just for more convenient output to logger
#include "gambit/Utils/util_macros.hpp"
#include "gambit/SpecBit/SpecBit_rollcall.hpp"
#include "gambit/SpecBit/SpecBit_helpers.hpp"
#include "gambit/SpecBit/QedQcdWrapper.hpp"
//#include "gambit/SpecBit/SMskeleton.hpp"
//#include "gambit/SpecBit/SCDMskeleton.hpp" does not exist
//#include "gambit/SpecBit/SCDMSpec.hpp"

#include "gambit/SpecBit/model_files_and_boxes.hpp" // #includes lots of flexiblesusy headers and defines interface classes

// Flexible SUSY stuff (should not be needed by the rest of gambit)
#include "flexiblesusy/src/ew_input.hpp"
#include "flexiblesusy/src/lowe.h" // From softsusy; used by flexiblesusy
#include "flexiblesusy/src/numerics2.hpp"
#include "flexiblesusy/src/two_loop_corrections.hpp"



// Switch for debug mode
#define SpecBit_DBUG 

namespace Gambit
{

  namespace SpecBit
  {
    using namespace LogTags;
    using namespace flexiblesusy;

    // To check if a model is currently being scanned:
    // bool Pipes::<fname>::ModelInUse(str model_name)

    /// @{ Non-Gambit convenience functions
    //  =======================================================================
    //  These are not known to Gambit, but they do basically all the real work.
    //  The Gambit module functions merely wrap the functions here and hook 
    //  them up to their dependencies, and input parameters.

    /// Compute an MSSM spectrum using flexiblesusy
    // In GAMBIT there are THREE flexiblesusy MSSM spectrum generators currently in
    // use, for each of three possible boundary condition types:
    //   - GUT scale input
    //   - Electroweak symmetry breaking scale input
    //   - Intermediate scale Q input
    // These each require slightly different setup, but once that is done the rest
    // of the code required to run them is the same; this is what is contained in
    // the below template function.
    // MI for Model Interface, as defined in model_files_and_boxes.hpp 
//    template <class MI> 
//    const Spectrum* run_FS_spectrum_generator
//        ( const typename MI::InputParameters& input
//        , const SMInputs& sminputs
//        , const Options& runOptions
//        , const std::map<str, safe_ptr<double> >& input_Param
//        )
//    {
//      // SoftSUSY object used to set quark and lepton masses and gauge
//      // couplings in QEDxQCD effective theory
//      // Will be initialised by default using values in lowe.h, which we will
//      // override next. 
//      softsusy::QedQcd oneset;
//
//      // Fill QedQcd object with SMInputs values
//      setup_QedQcd(oneset,sminputs);
//
//      // Run everything to Mz
//      oneset.toMz();
// 
//      // Create spectrum generator object
//      typename MI::SpectrumGenerator spectrum_generator;
//
//      // Spectrum generator settings
//      // Default options copied from flexiblesusy/src/spectrum_generator_settings.hpp
//      //
//      // | enum                             | possible values              | default value   |
//      // |----------------------------------|------------------------------|-----------------|
//      // | precision                        | any positive double          | 1.0e-4          |
//      // | max_iterations                   | any positive double          | 0 (= automatic) |
//      // | algorithm                        | 0 (two-scale) or 1 (lattice) | 0 (= two-scale) |
//      // | calculate_sm_masses              | 0 (no) or 1 (yes)            | 0 (= no)        |
//      // | pole_mass_loop_order             | 0, 1, 2                      | 2 (= 2-loop)    |
//      // | ewsb_loop_order                  | 0, 1, 2                      | 2 (= 2-loop)    |
//      // | beta_loop_order                  | 0, 1, 2                      | 2 (= 2-loop)    |
//      // | threshold_corrections_loop_order | 0, 1                         | 1 (= 1-loop)    |
//      // | higgs_2loop_correction_at_as     | 0, 1                         | 1 (= enabled)   |
//      // | higgs_2loop_correction_ab_as     | 0, 1                         | 1 (= enabled)   |
//      // | higgs_2loop_correction_at_at     | 0, 1                         | 1 (= enabled)   |
//      // | higgs_2loop_correction_atau_atau | 0, 1                         | 1 (= enabled)   |
//
//     
//      #define SPECGEN_SET(NAME,TYPE,DEFAULTVAL) \
//         CAT_2(spectrum_generator.set_, NAME) BOOST_PP_LPAREN() runOptions.getValueOrDef<TYPE> \
//               BOOST_PP_LPAREN() DEFAULTVAL BOOST_PP_COMMA() STRINGIFY(NAME) \
//               BOOST_PP_RPAREN() BOOST_PP_RPAREN()
//      // Ugly I know. It expands to:
//      // spectrum_generator.set_NAME(runOptions.getValueOrDef<TYPE>(DEFAULTVAL,"NAME"))
//
//      // For debugging only; check expansions
//      // #ifdef SpecBit_DBUG
//      //    #define ECHO(COMMAND) std::cout << SAFE_STRINGIFY(COMMAND) << std::endl
//      //    ECHO(  SPECGEN_SET(precision_goal,                 double, 1.0e-4)  );
//      //    #undef ECHO
//      // #endif
//
//      SPECGEN_SET(precision_goal,                    double, 1.0e-4);
//      SPECGEN_SET(max_iterations,                    double, 0 );
//      SPECGEN_SET(calculate_sm_masses,               bool, false );
//      SPECGEN_SET(pole_mass_loop_order,              int, 2 );
//      SPECGEN_SET(ewsb_loop_order,                   int, 2 );
//      SPECGEN_SET(beta_loop_order,                   int, 2 );
//      SPECGEN_SET(threshold_corrections_loop_order,  int, 1 );
//
//      #undef SPECGEN_SET
//
//      // Higgs loop corrections are a little different... sort them out now     
//      Two_loop_corrections two_loop_settings;
//
//      // alpha_t alpha_s
//      // alpha_b alpha_s
//      // alpha_t^2 + alpha_t alpha_b + alpha_b^2
//      // alpha_tau^2
//      two_loop_settings.higgs_at_as
//         = runOptions.getValueOrDef<bool>(true,"use_higgs_2loop_at_as");
//      two_loop_settings.higgs_ab_as
//         = runOptions.getValueOrDef<bool>(true,"use_higgs_2loop_ab_as");
//      two_loop_settings.higgs_at_at
//         = runOptions.getValueOrDef<bool>(true,"use_higgs_2loop_at_at");
//      two_loop_settings.higgs_atau_atau
//         = runOptions.getValueOrDef<bool>(true,"use_higgs_2loop_atau_atau");
//
//      spectrum_generator.set_two_loop_corrections(two_loop_settings);
//
//      // Generate spectrum
//      spectrum_generator.run(oneset, input);
//   
//      // Extract report on problems...
//   //  const typename MI::Problems& problems = spectrum_generator.get_problems();
//     
//      // Create Model_interface to carry the input and results, and know
//      // how to access the flexiblesusy routines.
//      // Note: Output of spectrum_generator.get_model() returns type, e.g. CMSSM.
//      // Need to convert it to type CMSSM_slha (which alters some conventions of
//      // parameters into SLHA format)
//      MI model_interface(spectrum_generator,oneset,input);
//
//      // Create SubSpectrum object to wrap flexiblesusy data
//      // THIS IS STATIC so that it lives on once we leave this module function. We 
//      // therefore cannot run the same spectrum generator twice in the same loop and 
//      // maintain the spectrum resulting from both. But we should never want to do 
//      // this.
//      // A pointer to this object is what gets turned into a SubSpectrum pointer and
//      // passed around Gambit.
//      //
//      // This object will COPY the interface data members into itself, so it is now the 
//      // one-stop-shop for all spectrum information, including the model interface object.
//      SCDMSpec<MI> SCDMspec(model_interface, "FlexibleSUSY", "1.1.0");
//
//      // Add extra information about the scales used to the wrapper object
//      // (last parameter turns the 'safety' check for the override setter off, which allows
//      //  us to set parameters that don't previously exist)
//      
//      SCDMspec.runningpars().set_override(Par::mass1,spectrum_generator.get_high_scale(),"high_scale",false);
//      SCDMspec.runningpars().set_override(Par::mass1,spectrum_generator.get_susy_scale(),"susy_scale",false);
//      SCDMspec.runningpars().set_override(Par::mass1,spectrum_generator.get_low_scale(), "low_scale", false);
//
//      // Create a second SubSpectrum object to wrap the qedqcd object used to initialise the spectrum generator
//      // Attach the sminputs object as well, so that SM pole masses can be passed on (these aren't easily
//      // extracted from the QedQcd object, so use the values that we put into it.)
//      QedQcdWrapper qedqcdspec(oneset,sminputs);
//      
//      
//      
//      std::ostringstream warnings;
//      const Problems<SCDM_info::NUMBER_OF_PARTICLES>& problems= spectrum_generator.get_problems();
//      const bool error = problems.have_problem();
//      problems.print_warnings(warnings);
//      if (error==1)
//      {
//      // check for errors
//      std::ostringstream problems_str;
//      problems.print_problems(problems_str);
//      cout<< FORMAT_SPINFO(4,problems_str.str()) << endl;
//      }
//
//
//// below is from MSSMSpec, I have done slightly different version above
//
////      if( runOptions.getValue<bool>("invalid_point_fatal") and problems.have_problem() )
////      {
////         ///TODO: Need to tell gambit that the spectrum is not viable somehow. For now
////         // just die.
////         std::ostringstream errmsg;
////         errmsg << "A serious problem was encountered during spectrum generation!; ";
////         errmsg << "Message from FlexibleSUSY below:" << std::endl;
////         problems.print_problems(errmsg); 
////         problems.print_warnings(errmsg); 
////         SpecBit_error().raise(LOCAL_INFO,errmsg.str());  
////      }  
////
////      // Write SLHA file (for debugging purposes...)
////      #ifdef SpecBit_DBUG
////         typename MI::SlhaIo slha_io;
////         slha_io.set_spinfo(problems);
////         slha_io.set_sminputs(oneset);
////         slha_io.set_minpar(input);
////         slha_io.set_extpar(input);
////         slha_io.set_spectrum(SCDMspec.model_interface.model);
////         slha_io.write_to_file("SpecBit/initial_SCDM_spectrum.slha");
////      #endif
//
//      // Package pointer to QedQcd SubSpectrum object along with pointer to SCDM SubSpectrum object,
//      // and SMInputs struct.
//      // Return pointer to this package.
//      static Spectrum matched_spectra;
//      matched_spectra = Spectrum(qedqcdspec,SCDMspec,sminputs,&input_Param);
//      return &matched_spectra;
//    }



    template <class T>
    void fill_SCDM_input(T& input, const std::map<str, safe_ptr<double> >& Param )
    {
      double mH2 = *Param.at("mH2");
      double mS2 = *Param.at("mS2");
      double lambda_hs = *Param.at("lambda_hS");
      
      
      double lambda_s= *Param.at("lambda_S");
      
      input.Lambda3Input=lambda_s;
      input.HiggsIN=-mH2;//-pow(mH,2)/2;
      input.mS2Input=mS2;//pow(mS,2)-lambda_hs*15;
      input.Lambda2Input=lambda_hs;
      input.Lambda3Input=lambda_s;

      input.QEWSB=173.15;  // scale where EWSB conditions are applied
    }
//
//    void get_SCDM_spectrum(const Spectrum* &result)
//    {
//      using namespace softsusy;
//      namespace myPipe = Pipes::get_SCDM_spectrum;
//      const SMInputs& sminputs = *myPipe::Dep::SMINPUTS;
//      SCDM_input_parameters input;
//      fill_SCDM_input(input,myPipe::Param);
//      input.Qin=173.15;
//      result = run_FS_spectrum_generator<SCDM_interface<ALGORITHM1>>(input,sminputs,*myPipe::runOptions,myPipe::Param);
//    }


//
//
    bool check_perturb_func(double scale,SCDM_input_parameters input,const SMInputs& sminputs)   //const Spectrum* fullspectrum)
    {
    using namespace flexiblesusy;
    using namespace softsusy;
    using namespace Gambit;
    using namespace SpecBit;
    //SMInputs sminputs = fullspectrum->get_SMInputs();
    softsusy::QedQcd oneset;
    setup_QedQcd(oneset,sminputs);
    oneset.toMz();
    
    typename SCDM_interface<Two_scale>::SpectrumGenerator spectrum_generator;
    input.Qin=scale;
    spectrum_generator.run(oneset, input);
    
    std::ostringstream warnings;
    const Problems<SCDM_info::NUMBER_OF_PARTICLES>& problems= spectrum_generator.get_problems();
    bool error = problems.have_problem();
    problems.print_warnings(warnings);
    if (error==1)
    {
    // check for errors
    std::ostringstream problems_str;
    problems.print_problems(problems_str);
    cout<< FORMAT_SPINFO(4,problems_str.str()) << endl;
    }
    return error;
    
    }
    


    void check_perturb_simple(bool &error)
    {
    using namespace flexiblesusy;
    using namespace softsusy;
    namespace myPipe = Pipes::check_perturb_simple;
    using namespace Gambit;
    using namespace SpecBit;
    const Options& runOptions=*myPipe::runOptions;
    double scale = runOptions.getValueOrDef<double>(1.22e19,"set_high_scale");
    cout<< "checking perturbative up to high scale = " << scale << endl;
    const SMInputs& sminputs = *myPipe::Dep::SMINPUTS;
    
   // const Spectrum* fullspectrum = *myPipe::Dep::SingletDM_spectrum;


    SCDM_input_parameters input;
    fill_SCDM_input(input,myPipe::Param);
    
    
    error=check_perturb_func(scale,input,sminputs);
    }
    
    
    
    void check_perturb_to_min_lambda(bool &error)
    {
    using namespace flexiblesusy;
    using namespace softsusy;
    namespace myPipe = Pipes::check_perturb_to_min_lambda;
    using namespace Gambit;
    using namespace SpecBit;
    ddpair age = *myPipe::Dep::vacuum_stability;
    double scale=std::get<1>(age);
    cout<< "checking perturbative up to high scale (of minimum lambda) = " << scale << endl;
    const SMInputs& sminputs = *myPipe::Dep::SMINPUTS;
    
   // const Spectrum* fullspectrum = *myPipe::Dep::SingletDM_spectrum;
   // SCDM_input_parameters input;
   // fill_SCDM_input(input,myPipe::Param);

   // const Spectrum* fullspectrum = *myPipe::Dep::SingletDM_spectrum;


    SCDM_input_parameters input;
    fill_SCDM_input(input,myPipe::Param);

    
//>>>>>>> DarkBit_development:SpecBit/src/SpecBit_SSDM.cpp
    
    error=check_perturb_func(scale,input,sminputs);
    }



    
    
    void shift(double &a, double &b, double &c, const double d)
    {
        a=b;
        b=c;
        c=d;
    }


    void find_min_lambda(std::pair<double, double>& age_pair)
    {
      using namespace flexiblesusy;
      using namespace softsusy;
      namespace myPipe = Pipes::find_min_lambda;//get_SingletDM_spectrum;
      using namespace Gambit;
      using namespace SpecBit;
      //const SMInputs& sminputs = *myPipe::Dep::SMINPUTS;

      const Spectrum* fullspectrum = *myPipe::Dep::SingletDM_spectrum;
      const SubSpectrum* spec = fullspectrum->get_HE(); // SCDMSpec SubSpectrum object
     
      cout<<"Scalar pole mass:" << endl;
      cout<<spec->phys().get(Par::Pole_Mass,"S")  <<endl;
      cout<<"Higgs pole mass:" << endl;
      cout<<spec->phys().get(Par::Pole_Mass,"h0")  <<endl;

      //SMInputs sminputs = fullspectrum->get_SMInputs();
      std::unique_ptr<SubSpectrum> SM = fullspectrum->clone_HE(); // COPIES Spectrum object
      //std::unique_ptr<SubSpectrum> oneset = fullspectrum->clone_LE();
  
      SM -> RunToScale(MZ);
      //double LamZ =SM->runningpars().get(Par::dimensionless,"Lambda1");
      //


      double u_1=1;
      double u_2=10;
      double u_3=20;
      double lambda_1,lambda_2,lambda_3;

      // fit parabola (in log space) to 3 trial points and use this to estimate the minimum, zooming in on the region of interest
      for (int i=1;i<3;i++)
      {
      SM -> RunToScale(pow(10,u_1));
      lambda_1 =SM->runningpars().get(Par::dimensionless,"Lambda1");
      SM -> RunToScale(pow(10,u_2));
      lambda_2 =SM->runningpars().get(Par::dimensionless,"Lambda1");
      SM -> RunToScale(pow(10,u_3));
      lambda_3 =SM->runningpars().get(Par::dimensionless,"Lambda1");
      double min_u= (lambda_1*(pow(u_2,2)-pow(u_3,2))  - lambda_2*(pow(u_1,2)-pow(u_3,2)) + lambda_3*(pow(u_1,2)-pow(u_2,2)));
      min_u=(min_u/( lambda_1*(u_2-u_3)+ lambda_2*(u_3-u_1)  +lambda_3*(u_1-u_2)))/2;
      u_1=min_u-2/(pow(float(i),0.01));
      u_2=min_u;
      u_3=min_u+2/(pow(float(i),0.01));
      SM = fullspectrum->clone_HE(); // clone the original spectrum incase the above calcuations have run into a non-perturbative scale and thus the spectrum is no longer reliable
      }
      // run downhill minimization routine to find exact minimum
      double ax=pow(10,u_1);
      double bx=pow(10,u_2);
      double cx=pow(10,u_3);

      int ITMAX=100;
      double tol=0.0001;
      const double CGOLD=0.3819660;
      const double ZEPS=numeric_limits<double>::epsilon()*1.0e-3;
      double d=0.0,etemp,fu,fv,fw,fx;
      double p,q,r,tol1,tol2,u,v,w,x,xm;
      double e=0.0;
      double a=(ax < cx ? ax : cx);
      double b=(ax > cx ? ax : cx);
      x=w=v=bx;
      SM -> RunToScale(x);
      fw=fv=fx =SM->runningpars().get(Par::dimensionless,"Lambda1");
      
      
      for (int iter=0;iter<ITMAX;iter++)
      {                            //  Main program loop.
          xm=0.5*(a+b);
          tol2=2.0*(tol1=tol*abs(x)+ZEPS);
          if (abs(x-xm) <= (tol2-0.5*(b-a)))
          {                      //Test for done here.
              break;
          }
          if (abs(e) > tol1)
          {                       //Construct a trial parabolic fit.
              r=(x-w)*(fx-fv);
              q=(x-v)*(fx-fw);
              p=(x-v)*q-(x-w)*r;
              q=2.0*(q-r);
              if (q > 0.0) p = -p;
              q=abs(q);
              etemp=e;
              e=d;
              if (abs(p) >= abs(0.5*q*etemp) || p <= q*(a-x)
                  || p >= q*(b-x))
                  d=CGOLD*(e=(x >= xm ? a-x : b-x));
              else
              {
                  d=p/q;
                  u=x+d;
                  if (u-a < tol2 || b-u < tol2)
                      d=SIGN(tol1,xm-x);
              }
          }
          else
          {
              d=CGOLD*(e=(x >= xm ? a-x : b-x));
          }
          u=(abs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
        
          SM -> RunToScale(u);
          fu =SM->runningpars().get(Par::dimensionless,"Lambda1");
        
        
        
          if (fu <= fx)
          {
              if(u >= x) a=x; else b=x;
              shift(v,w,x,u);
              shift(fv,fw,fx,fu);
          }
          else
          {
              if (u < x) a=u; else b=u;
              if (fu <= fw || w == x)
              {
                  v=w;
                  w=u;
                  fv=fw;
                  fw=fu;
              } else if (fu <= fv || v == x || v == w)
              {
                  v=u;
                  fv=fu;
              }
          }
      }
      cout<< "minimum value of quartic coupling is   "<< fu << " at " << u <<" GeV"<<endl;

      double lambda_min=fu;
      double lifetime,LB;
      if (lambda_min<0)
      {
        LB=u;
        double p=exp(4*140-2600/(abs(lambda_min)/0.01))*pow(LB/(1.2e19),4);
        cout<< "tunnelling rate is approximately  " << p << endl;
     
        if (p>1)
        {
            cout<< "vacuum is unstable" << endl;
            //stability=2; // unstable
        }
        else
        {
            cout<< "vacuum is metastable" << endl;
            //stability=1; // metastable
        }
       
        lifetime=1/(exp(3*140-2600/(abs(lambda_min)/0.01))*pow(1/(1.2e19),3)*pow(LB,4));
      }
      else
      {
        LB=1.22e19;
        lifetime=1e99;
        //stability=0; // stabe
      }
      age_pair = std::make_pair (lifetime,LB);
    
     
    }


    void get_expected_lifetime(double &lifetime)
    {
      namespace myPipe = Pipes::get_expected_lifetime;//
      using namespace Gambit;
      ddpair age = *myPipe::Dep::vacuum_stability;
      lifetime=std::get<0>(age);
    }
    
//    void default_scale(std::pair<double, double>& age_pair)
//    {
//      namespace myPipe = Pipes::default_scale;//
//      using namespace Gambit;
//      age_pair = std::make_pair (0,1.22e19);
//    }


    void get_likelihood(double &result)
    {
      namespace myPipe = Pipes::get_likelihood;//
      using namespace Gambit;
      ddpair age = *myPipe::Dep::vacuum_stability;
      result=log10(  ( 1 / ( std::get<0>(age) ) ) * exp(140) * (1/ (1.2e19) )   );
     result=1;
    }
    

  } // end namespace SpecBit
} // end namespace Gambit

