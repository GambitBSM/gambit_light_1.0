//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Functions of ColliderBit event loop.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Abram Krislock
///          (a.m.b.krislock@fys.uio.no)
///
///  \author Aldo Saavedra
///
///  \author Andy Buckley
///
///  \author Chris Rogan
///          (crogan@cern.ch)
///  \date 2014 Aug
///  \date 2015 May
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2015 Jul
///  \date 2018 Jan
///
///  \author Anders Kvellestad
///          (anders.kvellestad@fys.uio.no)
///  \date   2017 March
///  \date   2018 Jan
///
///  *********************************************

#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <numeric>
#include <sstream>
#include <vector>

#include "gambit/Elements/gambit_module_headers.hpp"
#include "gambit/ColliderBit/MC_convergence.hpp"
#include "gambit/ColliderBit/ColliderBit_rollcall.hpp"
#include "gambit/ColliderBit/analyses/BaseAnalysis.hpp"

#include "Eigen/Eigenvalues"
#include "HEPUtils/FastJet.h"

// #define COLLIDERBIT_DEBUG

namespace Gambit
{
  namespace ColliderBit
  {


    /// **************************************************
    /// Non-rollcalled functions and module-wide variables
    /// **************************************************


    #ifdef COLLIDERBIT_DEBUG
    inline str debug_prefix()
    {
      std::stringstream ss;
      ss << "DEBUG: OMP thread " << omp_get_thread_num() << ":  ";
      return ss.str();
    }
    #endif



    /// Module-wide variables
    /// @{

    /// @TODO: Get rid of some of these variables by restructuring the code a bit

    /// Special iteration labels for the loop controlled by operateLHCLoop
    enum specialIterations { BASE_INIT = -1,
                             COLLIDER_INIT = -2,
                             START_SUBPROCESS = -3,
                             COLLECT_CONVERGENCE_DATA = -4,
                             CHECK_CONVERGENCE = -5,
                             END_SUBPROCESS = -6,
                             COLLIDER_FINALIZE = -7,
                             BASE_FINALIZE = -8};

    /// Pythia stuff
    std::vector<str> pythiaNames;
    std::vector<str>::const_iterator iterPythiaNames;
    std::map< str,std::map<str,int> > colliderInfo;
    unsigned int indexPythiaNames;
    bool eventsGenerated;
    int nFailedEvents;
    int maxFailedEvents;
    int seedBase;

    /// Analysis stuff
    bool useBuckFastATLASDetector;
    bool haveUsedBuckFastATLASDetector;

    bool useBuckFastATLASnoeffDetector;
    bool haveUsedBuckFastATLASnoeffDetector;

    bool useBuckFastCMSDetector;
    bool haveUsedBuckFastCMSDetector;

    bool useBuckFastCMSnoeffDetector;
    bool haveUsedBuckFastCMSnoeffDetector;

    #ifndef EXCLUDE_DELPHES
    bool useDelphesDetector;
    std::vector<std::string> analysisNamesDet;
    bool haveUsedDelphesDetector;
    #endif

    bool useBuckFastIdentityDetector;
    bool haveUsedBuckFastIdentityDetector;

    /// @}

    // *************************************************
    // Rollcalled functions properly hooked up to Gambit
    // *************************************************
    // *** Loop Managers ***


    void MC_ConvergenceSettings_from_YAML(convergence_settings& result)
    {
      using namespace Pipes::MC_ConvergenceSettings_from_YAML;

      static bool first = true;
      if (first)
      {
        result.min_nEvents = runOptions->getValue<std::vector<int> >("min_nEvents");
        result.max_nEvents = runOptions->getValue<std::vector<int> >("max_nEvents");
        result.target_stat = runOptions->getValue<std::vector<double> >("target_fractional_uncert");
        result.stop_at_sys = runOptions->getValueOrDef<bool>(true, "halt_when_systematic_dominated");
        result.all_SR_must_converge = runOptions->getValueOrDef<bool>(true, "all_SR_must_converge");
        result.stoppingres = runOptions->getValueOrDef<std::vector<int> >(std::vector<int>(result.target_stat.size(), 200), "events_between_convergence_checks");
        if (result.min_nEvents.size() != result.max_nEvents.size() or result.min_nEvents.size() != result.target_stat.size())
        {
          str errmsg;
          errmsg  = "The options 'min_nEvents', 'max_nEvents' and 'target_fractional_uncert' for the function 'MC_ConvergenceSettings_from_YAML' must have\n";
          errmsg += "the same number of entries. Correct your settings and try again.";
          ColliderBit_error().raise(LOCAL_INFO, errmsg);
        }
        for (unsigned int i = 0; i != result.min_nEvents.size(); ++i)
        {
          if (result.min_nEvents[i] > result.max_nEvents[i])
           ColliderBit_error().raise(LOCAL_INFO,"One or more min_nEvents is greater than corresponding max_nEvents. Please correct yur YAML file.");
        }
        first = false;
      }
    }

    /// @note: Much of the loop below designed for splitting up the subprocesses to be generated.

    void operateLHCLoop()
    {
      using namespace Pipes::operateLHCLoop;
      static std::streambuf *coutbuf = std::cout.rdbuf(); // save cout buffer for running the loop quietly

      #ifdef COLLIDERBIT_DEBUG
      cout << debug_prefix() << endl;
      cout << debug_prefix() << "~~~~ New point! ~~~~" << endl;
      #endif

      //
      // Clear global containers and variables
      //
      pythiaNames.clear();
      iterPythiaNames = pythiaNames.cbegin();
      indexPythiaNames = 0;
      colliderInfo.clear();

      // - Pythia random number seed base will be set in the loop over colliders below.
      seedBase = 0;
      // - Set eventsGenerated to true once event generation is underway.
      eventsGenerated = false;
      // - Keep track of the number of failed events
      nFailedEvents = 0;

      useBuckFastATLASDetector = false;
      useBuckFastATLASnoeffDetector = false;
      useBuckFastCMSDetector = false;
      useBuckFastCMSnoeffDetector = false;
      useBuckFastIdentityDetector = false;
      #ifndef EXCLUDE_DELPHES
      useDelphesDetector = false;
      #endif

      haveUsedBuckFastATLASDetector = false;
      haveUsedBuckFastATLASnoeffDetector = false;
      haveUsedBuckFastCMSDetector = false;
      haveUsedBuckFastCMSnoeffDetector = false;
      haveUsedBuckFastIdentityDetector = false;
      #ifndef EXCLUDE_DELPHES
      haveUsedDelphesDetector = false;
      #endif

      // Retrieve run options from the YAML file (or standalone code)
      pythiaNames = runOptions->getValue<std::vector<str> >("pythiaNames");
      maxFailedEvents = runOptions->getValueOrDef<int>(1, "maxFailedEvents");
      // Allow the user to specify the Pythia seed base (for debugging). If the default value -1
      // is used, a new seed is generated for every new Pythia configuration and parameter point.
      int yaml_seedBase = runOptions->getValueOrDef<int>(-1, "pythiaSeedBase");

      // Check that length of pythiaNames and nEvents agree!
      if (pythiaNames.size() != Dep::MC_ConvergenceSettings->min_nEvents.size())
      {
        str errmsg;
        errmsg  = "The option 'pythiaNames' for the function 'operateLHCLoop' must have\n";
        errmsg += "the same number of entries as those in the MC_ConvergenceSettings dependency.\nCorrect your settings and try again.";
        ColliderBit_error().raise(LOCAL_INFO, errmsg);
      }

      // Should we silence stdout during the loop?
      bool silenceLoop = runOptions->getValueOrDef<bool>(true, "silenceLoop");
      if (silenceLoop) std::cout.rdbuf(0);

      // Do the base-level initialisation
      Loop::executeIteration(BASE_INIT);

      // For every collider requested in the yaml file:
      for (iterPythiaNames = pythiaNames.cbegin(); iterPythiaNames != pythiaNames.cend(); ++iterPythiaNames)
      {

        // Update the global index indexPythiaNames
        indexPythiaNames = iterPythiaNames - pythiaNames.cbegin();

        // Update the global Pythia seedBase.
        // The Pythia random number seed will be this, plus the thread number.
        if (yaml_seedBase == -1) { seedBase = int(Random::draw() * 899990000); }
        else { seedBase = yaml_seedBase; }

        // Store some collider info
        colliderInfo[*iterPythiaNames]["seed_base"] = seedBase;
        colliderInfo[*iterPythiaNames]["final_event_count"] = 0;  // Will be updated later

        // Get the minimum and maximum number of events to run for this collider, and the convergence step
        int min_nEvents = Dep::MC_ConvergenceSettings->min_nEvents[indexPythiaNames];
        int max_nEvents = Dep::MC_ConvergenceSettings->max_nEvents[indexPythiaNames];
        int stoppingres = Dep::MC_ConvergenceSettings->stoppingres[indexPythiaNames];

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "operateLHCLoop: Current collider is " << *iterPythiaNames << " with index " << indexPythiaNames << endl;
        #endif

        piped_invalid_point.check();
        Loop::reset();
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "operateLHCLoop: Will execute COLLIDER_INIT" << endl;
        #endif
        Loop::executeIteration(COLLIDER_INIT);

        // Any problem during COLLIDER_INIT step?
        piped_warnings.check(ColliderBit_warning());
        piped_errors.check(ColliderBit_error());

        //
        // OMP parallelized sections begin here
        //
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "operateLHCLoop: Will execute START_SUBPROCESS";
        #endif
        int currentEvent = 0;
        #pragma omp parallel
        {
          Loop::executeIteration(START_SUBPROCESS);
        }
        // Any problems during the START_SUBPROCESS step?
        piped_warnings.check(ColliderBit_warning());
        piped_errors.check(ColliderBit_error());

        // Convergence loop
        while(currentEvent < max_nEvents and not *Loop::done)
        {
          int eventCountBetweenConvergenceChecks = 0;

          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "Starting main event loop.  Will do " << stoppingres << " events before testing convergence." << endl;
          #endif

          // Main event loop
          #pragma omp parallel
          {
            while(eventCountBetweenConvergenceChecks < stoppingres and
                  currentEvent < max_nEvents and
                  not *Loop::done and
                  not piped_errors.inquire() and
                  nFailedEvents <= maxFailedEvents)
            {
              if (!eventsGenerated) eventsGenerated = true;
              try
              {
                Loop::executeIteration(currentEvent);
                currentEvent++;
                eventCountBetweenConvergenceChecks++;
              }
              catch (std::domain_error& e)
              {
                cout << "\n   Continuing to the next event...\n\n";
              }
            }
          }
          // Any problems during the main event loop?
          piped_warnings.check(ColliderBit_warning());
          piped_errors.check(ColliderBit_error());

          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "Did " << eventCountBetweenConvergenceChecks << " events of " << currentEvent << " simulated so far." << endl;
          #endif

          // Break convergence loop if too many events fail
          if(nFailedEvents > maxFailedEvents) break;

          // Don't bother with convergence stuff if we haven't passed the minimum number of events yet
          if (currentEvent >= min_nEvents)
          {
            #pragma omp parallel
            {
              Loop::executeIteration(COLLECT_CONVERGENCE_DATA);
            }
            // Any problems during the COLLECT_CONVERGENCE_DATA step?
            piped_warnings.check(ColliderBit_warning());
            piped_errors.check(ColliderBit_error());

            Loop::executeIteration(CHECK_CONVERGENCE);
            // Any problems during the CHECK_CONVERGENCE step?
            piped_warnings.check(ColliderBit_warning());
            piped_errors.check(ColliderBit_error());
          }
        }

        // Store the number of generated events
        colliderInfo[*iterPythiaNames]["final_event_count"] = currentEvent;  // Will be updated later

        // Break collider loop if too many events have failed
        if(nFailedEvents > maxFailedEvents)
        {
          logger() << LogTags::debug << "Too many failed events during event generation." << EOM;
          break;
        }

        #pragma omp parallel
        {
          Loop::executeIteration(END_SUBPROCESS);
        }
        // Any problems during the END_SUBPROCESS step?
        piped_warnings.check(ColliderBit_warning());
        piped_errors.check(ColliderBit_error());

        //
        // OMP parallelized sections end here
        //

        Loop::executeIteration(COLLIDER_FINALIZE);
      }

      // Nicely thank the loop for being quiet, and restore everyone's vocal cords
      if (silenceLoop) std::cout.rdbuf(coutbuf);

      // Check for exceptions
      piped_invalid_point.check();

      Loop::executeIteration(BASE_FINALIZE);
    }



    // *** Hard Scattering Collider Simulators ***

    void getPythia(SpecializablePythia &result)
    {
      using namespace Pipes::getPythia;

      static str pythia_doc_path;
      static str default_doc_path;
      static bool pythia_doc_path_needs_setting = true;
      static std::vector<str> pythiaCommonOptions;
      static SLHAstruct slha;
      static SLHAstruct spectrum;
      static std::vector<double> xsec_vetos;

      if (*Loop::iteration == BASE_INIT)
      {
        // Setup the Pythia documentation path
        if (pythia_doc_path_needs_setting)
        {
          default_doc_path = GAMBIT_DIR "/Backends/installed/Pythia/" +
            Backends::backendInfo().default_version("Pythia") +
            "/share/Pythia8/xmldoc/";
          pythia_doc_path = runOptions->getValueOrDef<str>(default_doc_path, "Pythia_doc_path");
          // Print the Pythia banner once.
          result.banner(pythia_doc_path);
          pythia_doc_path_needs_setting = false;
        }

        // SLHAea object constructed from dependencies on the spectrum and decays.
        slha.clear();
        spectrum.clear();
        slha = Dep::decay_rates->getSLHAea(2);
        if (ModelInUse("MSSM63atQ") or ModelInUse("MSSM63atMGUT"))
        {
          // MSSM-specific.  SLHAea in SLHA2 format, please.
          spectrum = Dep::MSSM_spectrum->getSLHAea(2);
          SLHAea::Block block("MODSEL");
          block.push_back("BLOCK MODSEL              # Model selection");
          SLHAea::Line line;
          line << 1 << 0 << "# General MSSM";
          block.push_back(line);
          slha.insert(slha.begin(), spectrum.begin(), spectrum.end());
          slha.push_front(block);
        }
        else
        {
          ColliderBit_error().raise(LOCAL_INFO, "No spectrum object available for this model.");
        }

        // Read xsec veto values and store in static variable 'xsec_vetos'
        std::vector<double> default_xsec_vetos(pythiaNames.size(), 0.0);
        xsec_vetos = runOptions->getValueOrDef<std::vector<double> >(default_xsec_vetos, "xsec_vetos");
        CHECK_EQUAL_VECTOR_LENGTH(xsec_vetos, pythiaNames)
      }

      else if (*Loop::iteration == COLLIDER_INIT)
      {
        // Collect Pythia options that are common across all OMP threads
        pythiaCommonOptions.clear();

        // By default we tell Pythia to be quiet. (Can be overridden from yaml settings)
        pythiaCommonOptions.push_back("Print:quiet = on");
        pythiaCommonOptions.push_back("SLHA:verbose = 0");

        // Get options from yaml file. If the SpecializablePythia specialization is hard-coded, okay with no options.
        if (runOptions->hasKey(*iterPythiaNames))
        {
          std::vector<str> addPythiaOptions = runOptions->getValue<std::vector<str>>(*iterPythiaNames);
          pythiaCommonOptions.insert(pythiaCommonOptions.end(), addPythiaOptions.begin(), addPythiaOptions.end());
        }

        // We need showProcesses for the xsec veto.
        pythiaCommonOptions.push_back("Init:showProcesses = on");

        // We need "SLHA:file = slhaea" for the SLHAea interface.
        pythiaCommonOptions.push_back("SLHA:file = slhaea");
      }

      else if (*Loop::iteration == START_SUBPROCESS)
      {
        // Variables needed for the xsec veto
        std::stringstream processLevelOutput;
        str _junk, readline;
        int code, nxsec;
        double xsec, totalxsec;

        // Each thread needs an independent Pythia instance at the start
        // of each event generation loop.
        // Thus, the actual Pythia initialization is
        // *after* COLLIDER_INIT, within omp parallel.

        result.clear();

        // Get the Pythia options that are common across all OMP threads ('pythiaCommonOptions')
        // and then add the thread-specific seed
        std::vector<str> pythiaOptions = pythiaCommonOptions;
        pythiaOptions.push_back("Random:seed = " + std::to_string(seedBase + omp_get_thread_num()));

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "getPythia: My Pythia seed is: " << std::to_string(seedBase + omp_get_thread_num()) << endl;
        #endif

        result.resetSpecialization(*iterPythiaNames);

        try
        {
          result.init(pythia_doc_path, pythiaOptions, &slha, processLevelOutput);
        }
        catch (SpecializablePythia::InitializationError &e)
        {
          // Append new seed to override the previous one
          int newSeedBase = int(Random::draw() * 899990000.);
          pythiaOptions.push_back("Random:seed = " + std::to_string(newSeedBase));
          try
          {
            result.init(pythia_doc_path, pythiaOptions, &slha, processLevelOutput);
          }
          catch (SpecializablePythia::InitializationError &e)
          {
            #ifdef COLLIDERBIT_DEBUG
            cout << debug_prefix() << "SpecializablePythia::InitializationError caught in getPythia. Will discard this point." << endl;
            #endif
            piped_invalid_point.request("Bad point: Pythia can't initialize");
            Loop::wrapup();
            return;
          }
        }

        // Should we apply the xsec veto and skip event generation?

        // - Get the xsec veto value for the current collider
        double totalxsec_fb_veto = xsec_vetos[indexPythiaNames];

        // - Get the upper limt xsec as estimated by Pythia
        code = -1;
        nxsec = 0;
        totalxsec = 0.;
        while(true)
        {
          std::getline(processLevelOutput, readline);
          std::istringstream issPtr(readline);
          issPtr.seekg(47, issPtr.beg);
          issPtr >> code;
          if (!issPtr.good() && nxsec > 0) break;
          issPtr >> _junk >> xsec;
          if (issPtr.good())
          {
            totalxsec += xsec;
            nxsec++;
          }
        }

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "totalxsec [fb] = " << totalxsec * 1e12 << ", veto limit [fb] = " << totalxsec_fb_veto << endl;
        #endif

        // - Check for NaN xsed
        if (Utils::isnan(totalxsec))
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "Got NaN cross-section estimate from Pythia." << endl;
          #endif
          piped_invalid_point.request("Got NaN cross-section estimate from Pythia.");
          Loop::wrapup();
          return;
        }

        // - Wrap up loop if veto applies
        if (totalxsec * 1e12 < totalxsec_fb_veto)
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "Cross-section veto applies. Will now call Loop::wrapup() to skip event generation for this collider." << endl;
          #endif
          Loop::wrapup();
        }
      }
    }



    void getPythiaFileReader(SpecializablePythia &result)
    {
      using namespace Pipes::getPythiaFileReader;

      static std::vector<str> filenames;
      static str default_doc_path;
      static str pythia_doc_path;
      static std::vector<str> pythiaCommonOptions;
      static bool pythia_doc_path_needs_setting = true;
      static unsigned int fileCounter = 0;
      static std::vector<double> xsec_vetos;

      if (*Loop::iteration == BASE_INIT)
      {
        // Setup the Pythia documentation path
        if (pythia_doc_path_needs_setting)
        {
          default_doc_path = GAMBIT_DIR "/Backends/installed/Pythia/" +
            Backends::backendInfo().default_version("Pythia") +
            "/share/Pythia8/xmldoc/";
          pythia_doc_path = runOptions->getValueOrDef<str>(default_doc_path, "Pythia_doc_path");
          // Print the Pythia banner once.
          result.banner(pythia_doc_path);
          pythia_doc_path_needs_setting = false;
        }

        // Get SLHA file(s)
        filenames = runOptions->getValue<std::vector<str> >("SLHA_filenames");
        if (filenames.empty())
        {
          str errmsg = "No SLHA files are listed for ColliderBit function getPythiaFileReader.\n";
          errmsg    += "Please correct the option 'SLHA_filenames' or use getPythia instead.";
          ColliderBit_error().raise(LOCAL_INFO, errmsg);
        }

        if (filenames.size() <= fileCounter) invalid_point().raise("No more SLHA files. My work is done.");

        // Read xsec veto values and store in static variable 'xsec_vetos'
        std::vector<double> default_xsec_vetos(pythiaNames.size(), 0.0);
        xsec_vetos = runOptions->getValueOrDef<std::vector<double> >(default_xsec_vetos, "xsec_vetos");
        CHECK_EQUAL_VECTOR_LENGTH(xsec_vetos, pythiaNames)
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        // Collect Pythia options that are common across all OMP threads
        pythiaCommonOptions.clear();

        // By default we tell Pythia to be quiet. (Can be overridden from yaml settings)
        pythiaCommonOptions.push_back("Print:quiet = on");
        pythiaCommonOptions.push_back("SLHA:verbose = 0");

        // Get options from yaml file. If the SpecializablePythia specialization is hard-coded, okay with no options.
        if (runOptions->hasKey(*iterPythiaNames))
        {
          std::vector<str> addPythiaOptions = runOptions->getValue<std::vector<str>>(*iterPythiaNames);
          pythiaCommonOptions.insert(pythiaCommonOptions.end(), addPythiaOptions.begin(), addPythiaOptions.end());
        }

        // We need showProcesses for the xsec veto.
        pythiaCommonOptions.push_back("Init:showProcesses = on");

        // We need to control "SLHA:file" for the SLHA interface.
        pythiaCommonOptions.push_back("SLHA:file = " + filenames.at(fileCounter));
      }

      if (*Loop::iteration == START_SUBPROCESS)
      {
        // variables for xsec veto
        std::stringstream processLevelOutput;
        str _junk, readline;
        int code, nxsec;
        double xsec, totalxsec;

        // Each thread needs an independent Pythia instance at the start
        // of each event generation loop.
        // Thus, the actual Pythia initialization is
        // *after* COLLIDER_INIT, within omp parallel.

        result.clear();

        if (omp_get_thread_num() == 0) logger() << "Reading SLHA file: " << filenames.at(fileCounter) << EOM;

        // Get the Pythia options that are common across all OMP threads ('pythiaCommonOptions')
        // and then add the thread-specific seed
        std::vector<str> pythiaOptions = pythiaCommonOptions;
        pythiaOptions.push_back("Random:seed = " + std::to_string(seedBase + omp_get_thread_num()));

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "getPythiaFileReader: My Pythia seed is: " << std::to_string(seedBase + omp_get_thread_num()) << endl;
        #endif

        result.resetSpecialization(*iterPythiaNames);

        try
        {
          result.init(pythia_doc_path, pythiaOptions, processLevelOutput);
        }
        catch (SpecializablePythia::InitializationError &e)
        {
          // Append new seed to override the previous one
          int newSeedBase = int(Random::draw() * 899990000.);
          pythiaOptions.push_back("Random:seed = " + std::to_string(newSeedBase));
          try
          {
            result.init(pythia_doc_path, pythiaOptions, processLevelOutput);
          }
          catch (SpecializablePythia::InitializationError &e)
          {
            piped_invalid_point.request("Bad point: Pythia can't initialize");
            Loop::wrapup();
            return;
          }
        }

        // Should we apply the xsec veto and skip event generation?

        // - Get the xsec veto value for the current collider
        double totalxsec_fb_veto = xsec_vetos[indexPythiaNames];

        // - Get the upper limt xsec as estimated by Pythia
        code = -1;
        nxsec = 0;
        totalxsec = 0.;
        while(true)
        {
          std::getline(processLevelOutput, readline);
          std::istringstream issPtr(readline);
          issPtr.seekg(47, issPtr.beg);
          issPtr >> code;
          if (!issPtr.good() && nxsec > 0) break;
          issPtr >> _junk >> xsec;
          if (issPtr.good())
          {
            totalxsec += xsec;
            nxsec++;
          }
        }

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "totalxsec [fb] = " << totalxsec * 1e12 << ", veto limit [fb] = " << totalxsec_fb_veto << endl;
        #endif

        // - Wrap up loop if veto applies
        if (totalxsec * 1e12 < totalxsec_fb_veto)
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "Cross-section veto applies. Will now call Loop::wrapup() to skip event generation for this collider." << endl;
          #endif
          Loop::wrapup();
        }

      }

      if (*Loop::iteration == BASE_FINALIZE) fileCounter++;

    }


    // *** Detector Simulators ***

    #ifndef EXCLUDE_DELPHES
    void getDelphes(DelphesVanilla &result)
    {
      using namespace Pipes::getDelphes;
      static std::vector<bool> useDetector;
      static std::vector<str> delphesConfigFiles;

      if (*Loop::iteration == BASE_INIT)
      {
        // Read useDetector option
        std::vector<bool> default_useDetector(pythiaNames.size(), false);  // Delphes is switched off by default
        useDetector = runOptions->getValueOrDef<std::vector<bool> >(default_useDetector, "useDetector");
        CHECK_EQUAL_VECTOR_LENGTH(useDetector,pythiaNames)

        // Return if all elements in useDetector are false
        if (std::find(useDetector.begin(), useDetector.end(), true) == useDetector.end()) return;

        // Read delphesConfigFiles option
        delphesConfigFiles = runOptions->getValue<std::vector<str> >("delphesConfigFiles");
        CHECK_EQUAL_VECTOR_LENGTH(delphesConfigFiles,pythiaNames)

        // Delphes is not threadsafe (depends on ROOT). Raise error if OMP_NUM_THREADS>1.
        if(omp_get_max_threads()>1 and std::find(useDetector.begin(), useDetector.end(), true) != useDetector.end())
        {
          str errmsg = "Delphes is not threadsafe and cannot be used with OMP_NUM_THREADS>1.\n";
          errmsg    += "Either set OMP_NUM_THREADS=1 or switch to a threadsafe detector simulator, e.g. BuckFast.";
          ColliderBit_error().raise(LOCAL_INFO, errmsg);
        }

        return;
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        result.clear();

        // Get useDetector setting for the current collider
        useDelphesDetector = useDetector[indexPythiaNames];
        if (!useDelphesDetector) return;
        else haveUsedDelphesDetector = true;

        // Setup new Delphes for the current collider
        std::vector<str> delphesOptions;
        delphesOptions.push_back(delphesConfigFiles[indexPythiaNames]);

        try
        {
          result.init(delphesOptions);
        }
        catch (std::runtime_error& e)
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "DelphesVanilla::InitializationError caught in getDelphes. Will raise ColliderBit_error." << endl;
          #endif
          str errmsg = "getDelphes caught the following runtime error: ";
          errmsg    += e.what();
          piped_errors.request(LOCAL_INFO, errmsg);
        }
      }
    }
    #endif // not defined EXCLUDE_DELPHES



    void getBuckFastATLAS(BuckFastSmearATLAS &result)
    {
      using namespace Pipes::getBuckFastATLAS;
      static std::vector<bool> useDetector;
      static std::vector<bool> partonOnly;
      static std::vector<double> antiktR;

      if (*Loop::iteration == BASE_INIT)
      {
        // Read options
        std::vector<bool> default_useDetector(pythiaNames.size(), true);  // BuckFastATLAS is switched on by default
        useDetector = runOptions->getValueOrDef<std::vector<bool> >(default_useDetector, "useDetector");
        CHECK_EQUAL_VECTOR_LENGTH(useDetector,pythiaNames)

        std::vector<bool> default_partonOnly(pythiaNames.size(), false);
        partonOnly = runOptions->getValueOrDef<std::vector<bool> >(default_partonOnly, "partonOnly");
        CHECK_EQUAL_VECTOR_LENGTH(partonOnly,pythiaNames)

        std::vector<double> default_antiktR(pythiaNames.size(), 0.4);
        antiktR = runOptions->getValueOrDef<std::vector<double> >(default_antiktR, "antiktR");
        CHECK_EQUAL_VECTOR_LENGTH(antiktR,pythiaNames)

        return;
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        // Get useDetector setting for the current collider
        useBuckFastATLASDetector = useDetector[indexPythiaNames];
        if (useBuckFastATLASDetector)
          haveUsedBuckFastATLASDetector = true;

        return;
      }

      if (*Loop::iteration == START_SUBPROCESS and useBuckFastATLASDetector)
      {
        // Each thread gets its own BuckFastSmearATLAS.
        // Thus, their initialization is *after* COLLIDER_INIT, within omp parallel.
        result.init(partonOnly[indexPythiaNames], antiktR[indexPythiaNames]);

        return;
      }

    }


    void getBuckFastATLASnoeff(BuckFastSmearATLASnoeff &result)
    {
      using namespace Pipes::getBuckFastATLASnoeff;
      static std::vector<bool> useDetector;
      static std::vector<bool> partonOnly;
      static std::vector<double> antiktR;

      if (*Loop::iteration == BASE_INIT)
      {
        // Read options
        std::vector<bool> default_useDetector(pythiaNames.size(), false);  // BuckFastATLASnoeff is switched off by default
        useDetector = runOptions->getValueOrDef<std::vector<bool> >(default_useDetector, "useDetector");
        CHECK_EQUAL_VECTOR_LENGTH(useDetector,pythiaNames)

        std::vector<bool> default_partonOnly(pythiaNames.size(), false);
        partonOnly = runOptions->getValueOrDef<std::vector<bool> >(default_partonOnly, "partonOnly");
        CHECK_EQUAL_VECTOR_LENGTH(partonOnly,pythiaNames)

        std::vector<double> default_antiktR(pythiaNames.size(), 0.4);
        antiktR = runOptions->getValueOrDef<std::vector<double> >(default_antiktR, "antiktR");
        CHECK_EQUAL_VECTOR_LENGTH(antiktR,pythiaNames)

        return;
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        // Get useDetector setting for the current collider
        useBuckFastATLASnoeffDetector = useDetector[indexPythiaNames];
        if (useBuckFastATLASnoeffDetector)
          haveUsedBuckFastATLASnoeffDetector = true;

        return;
      }

      if (*Loop::iteration == START_SUBPROCESS and useBuckFastATLASnoeffDetector)
      {
        // Each thread gets its own BuckFastSmearATLASnoeff.
        // Thus, their initialization is *after* COLLIDER_INIT, within omp parallel.
        result.init(partonOnly[indexPythiaNames], antiktR[indexPythiaNames]);

        return;
      }

    }


    void getBuckFastCMS(BuckFastSmearCMS &result)
    {
      using namespace Pipes::getBuckFastCMS;
      static std::vector<bool> useDetector;
      static std::vector<bool> partonOnly;
      static std::vector<double> antiktR;

      if (*Loop::iteration == BASE_INIT)
      {
        // Read options
        std::vector<bool> default_useDetector(pythiaNames.size(), true);  // BuckFastCMS is switched on by default
        useDetector = runOptions->getValueOrDef<std::vector<bool> >(default_useDetector, "useDetector");
        CHECK_EQUAL_VECTOR_LENGTH(useDetector,pythiaNames)

        std::vector<bool> default_partonOnly(pythiaNames.size(), false);
        partonOnly = runOptions->getValueOrDef<std::vector<bool> >(default_partonOnly, "partonOnly");
        CHECK_EQUAL_VECTOR_LENGTH(partonOnly,pythiaNames)

        std::vector<double> default_antiktR(pythiaNames.size(), 0.4);
        antiktR = runOptions->getValueOrDef<std::vector<double> >(default_antiktR, "antiktR");
        CHECK_EQUAL_VECTOR_LENGTH(antiktR,pythiaNames)

        return;
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        // Get useDetector setting for the current collider
        useBuckFastCMSDetector = useDetector[indexPythiaNames];
        if (useBuckFastCMSDetector) haveUsedBuckFastCMSDetector = true;
        return;
      }

      if (*Loop::iteration == START_SUBPROCESS and useBuckFastCMSDetector)
      {
        // Each thread gets its own BuckFastSmearCMS.
        // Thus, their initialization is *after* COLLIDER_INIT, within omp parallel.
        result.init(partonOnly[indexPythiaNames], antiktR[indexPythiaNames]);
        return;
      }

    }


    void getBuckFastCMSnoeff(BuckFastSmearCMSnoeff &result)
    {
      using namespace Pipes::getBuckFastCMSnoeff;
      static std::vector<bool> useDetector;
      static std::vector<bool> partonOnly;
      static std::vector<double> antiktR;

      if (*Loop::iteration == BASE_INIT)
      {
        // Read options
        std::vector<bool> default_useDetector(pythiaNames.size(), false);  // BuckFastCMSnoeff is switched off by default
        useDetector = runOptions->getValueOrDef<std::vector<bool> >(default_useDetector, "useDetector");
        CHECK_EQUAL_VECTOR_LENGTH(useDetector,pythiaNames)

        std::vector<bool> default_partonOnly(pythiaNames.size(), false);
        partonOnly = runOptions->getValueOrDef<std::vector<bool> >(default_partonOnly, "partonOnly");
        CHECK_EQUAL_VECTOR_LENGTH(partonOnly,pythiaNames)

        std::vector<double> default_antiktR(pythiaNames.size(), 0.4);
        antiktR = runOptions->getValueOrDef<std::vector<double> >(default_antiktR, "antiktR");
        CHECK_EQUAL_VECTOR_LENGTH(antiktR,pythiaNames)

        return;
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        // Get useDetector setting for the current collider
        useBuckFastCMSnoeffDetector = useDetector[indexPythiaNames];
        if (useBuckFastCMSnoeffDetector) haveUsedBuckFastCMSnoeffDetector = true;
        return;
      }

      if (*Loop::iteration == START_SUBPROCESS and useBuckFastCMSnoeffDetector)
      {
        // Each thread gets its own BuckFastSmearCMSnoeff.
        // Thus, their initialization is *after* COLLIDER_INIT, within omp parallel.
        result.init(partonOnly[indexPythiaNames], antiktR[indexPythiaNames]);
        return;
      }

    }


    void getBuckFastIdentity(Gambit::ColliderBit::BuckFastIdentity &result)
    {
      using namespace Pipes::getBuckFastIdentity;
      static std::vector<bool> useDetector;
      static std::vector<bool> partonOnly;
      static std::vector<double> antiktR;

      if (*Loop::iteration == BASE_INIT)
      {
        // Read options
        std::vector<bool> default_useDetector(pythiaNames.size(), false);  // BuckFastIdentity is switched off by default
        useDetector = runOptions->getValueOrDef<std::vector<bool> >(default_useDetector, "useDetector");
        CHECK_EQUAL_VECTOR_LENGTH(useDetector,pythiaNames)

        std::vector<bool> default_partonOnly(pythiaNames.size(), false);
        partonOnly = runOptions->getValueOrDef<std::vector<bool> >(default_partonOnly, "partonOnly");
        CHECK_EQUAL_VECTOR_LENGTH(partonOnly,pythiaNames)

        std::vector<double> default_antiktR(pythiaNames.size(), 0.4);
        antiktR = runOptions->getValueOrDef<std::vector<double> >(default_antiktR, "antiktR");
        CHECK_EQUAL_VECTOR_LENGTH(antiktR,pythiaNames)

        return;
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        // Get useDetector setting for the current collider
        useBuckFastIdentityDetector = useDetector[indexPythiaNames];
        if (useBuckFastIdentityDetector) haveUsedBuckFastIdentityDetector = true;
        return;
      }

      if (*Loop::iteration == START_SUBPROCESS and useBuckFastIdentityDetector)
      {
        // Each thread gets its own BuckFastSmearIdentity.
        // Thus, their initialization is *after* COLLIDER_INIT, within omp parallel.
        result.init(partonOnly[indexPythiaNames], antiktR[indexPythiaNames]);
        return;
      }
    }



    // *** Initialization for analyses ***


    #ifndef EXCLUDE_DELPHES
    void getDetAnalysisContainer(HEPUtilsAnalysisContainer& result)
    {
      using namespace Pipes::getDetAnalysisContainer;
      static std::vector<std::vector<str> > analyses;
      static bool first = true;

      if (*Loop::iteration == BASE_INIT)
      {
        // Only run this once
        if (first)
        {
          // Read analysis names from the yaml file
          std::vector<std::vector<str> > default_analyses;  // The default is empty lists of analyses
          analyses = runOptions->getValueOrDef<std::vector<std::vector<str> > >(default_analyses, "analyses");
          first = false;
        }
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        if (!useDelphesDetector) return;

        // Check that there are some analyses to run if the detector is switched on
        if (analyses[indexPythiaNames].empty() and useDelphesDetector)
        {
          str errmsg = "The option 'useDetector' for function 'getDelphes' is set to true\n";
          errmsg    += "for the collider '";
          errmsg    += *iterPythiaNames;
          errmsg    += "', but the corresponding list of analyses\n";
          errmsg    += "(in option 'analyses' for function 'getDetAnalysisContainer') is empty.\n";
          errmsg    += "Please correct your settings.\n";
          ColliderBit_error().raise(LOCAL_INFO, errmsg);
        }

        return;
      }

      if (!useDelphesDetector) return;

      if (*Loop::iteration == START_SUBPROCESS)
      {
        result.register_thread("DetAnalysisContainer");
        result.set_current_collider(*iterPythiaNames);

        if (!result.has_analyses()) result.init(analyses[indexPythiaNames]); 
        else result.reset();

        return;
      }

      if (*Loop::iteration == END_SUBPROCESS && eventsGenerated && nFailedEvents <= maxFailedEvents)
      {
        const double xs_fb = Dep::HardScatteringSim->xsec_pb() * 1000.;
        const double xserr_fb = Dep::HardScatteringSim->xsecErr_pb() * 1000.;
        result.add_xsec(xs_fb, xserr_fb);

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "xs_fb = " << xs_fb << " +/- " << xserr_fb << endl;
        #endif
        return;
      }

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        result.collect_and_add_signal();
        result.collect_and_improve_xsec();
        result.scale();        
        return;
      }
    }
    #endif // not defined EXCLUDE_DELPHES



    void getATLASAnalysisContainer(HEPUtilsAnalysisContainer& result)
    {
      using namespace Pipes::getATLASAnalysisContainer;
      static std::vector<std::vector<str> > analyses;
      static bool first = true;

      if (*Loop::iteration == BASE_INIT)
      {
        // Only run this once
        if (first)
        {
          // Read analysis names from the yaml file
          std::vector<std::vector<str> > default_analyses;  // The default is empty lists of analyses
          analyses = runOptions->getValueOrDef<std::vector<std::vector<str> > >(default_analyses, "analyses");
          first = false;
        }
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        if (!useBuckFastATLASDetector) return;

        // Check that there are some analyses to run if the detector is switched on
        if (analyses[indexPythiaNames].empty() and useBuckFastATLASDetector)
        {
          str errmsg = "The option 'useDetector' for function 'getBuckFastATLAS' is set to true\n";
          errmsg    += "for the collider '";
          errmsg    += *iterPythiaNames;
          errmsg    += "', but the corresponding list of analyses\n";
          errmsg    += "(in option 'analyses' for function 'getATLASAnalysisContainer') is empty.\n";
          errmsg    += "Please correct your settings.\n";
          ColliderBit_error().raise(LOCAL_INFO, errmsg);
        }

        return;
      }

      if (!useBuckFastATLASDetector) return;

      if (*Loop::iteration == START_SUBPROCESS)
      {
        // cout << "DEBUG: thread " << my_thread << ": getATLASAnalysisContainer: Begin START_SUBPROCESS, indexPythiaNames = " << indexPythiaNames  << endl;

        // Register analysis container
        // cout << "DEBUG: thread " << my_thread << ": getATLASAnalysisContainer: Will run result.register_thread " << endl;
        result.register_thread("ATLASAnalysisContainer");
        // cout << "DEBUG: thread " << my_thread << ": getATLASAnalysisContainer: ...done" << endl;

        // Set current collider
        // cout << "DEBUG: thread " << my_thread << ": getATLASAnalysisContainer: Will run result.set_current_collider " << endl;
        result.set_current_collider(*iterPythiaNames);
        // cout << "DEBUG: thread " << my_thread << ": getATLASAnalysisContainer: ...done" << endl;

        // Initialize analysis container or reset all the contained analyses
        // cout << "DEBUG: thread " << my_thread << ": getATLASAnalysisContainer: Will run get_current_analyses_map" << endl;
        if (!result.has_analyses()) result.init(analyses[indexPythiaNames]); 
        else result.reset();
        // cout << "DEBUG: thread " << my_thread << ": getATLASAnalysisContainer: ...done " << endl;

        // #ifdef COLLIDERBIT_DEBUG
        // if (my_thread == 0)
        // {
        //   for (auto& apair : result.get_current_analyses_map())
        //   {
        //     cout << debug_prefix() << "The run with " << *iterPythiaNames << " will include the analysis " << apair.first << endl;
        //   }
        // }
        // #endif

        // cout << "DEBUG: thread " << my_thread << ": getATLASAnalysisContainer: End START_SUBPROCESS "  << endl;
        return;
      }

      if (*Loop::iteration == END_SUBPROCESS && eventsGenerated && nFailedEvents <= maxFailedEvents)
      {
        const double xs_fb = Dep::HardScatteringSim->xsec_pb() * 1000.;
        const double xserr_fb = Dep::HardScatteringSim->xsecErr_pb() * 1000.;
        result.add_xsec(xs_fb, xserr_fb);

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "xs_fb = " << xs_fb << " +/- " << xserr_fb << endl;
        #endif
        return;
      }

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        result.collect_and_add_signal();
        result.collect_and_improve_xsec();
        result.scale();        
        return;
      }

    }



    void getATLASnoeffAnalysisContainer(HEPUtilsAnalysisContainer& result)
    {
      using namespace Pipes::getATLASnoeffAnalysisContainer;
      static std::vector<std::vector<str> > analyses;
      static bool first = true;

      if (*Loop::iteration == BASE_INIT)
      {
        // Only run this once
        if (first)
        {
          // Read analysis names from the yaml file
          std::vector<std::vector<str> > default_analyses;  // The default is empty lists of analyses
          analyses = runOptions->getValueOrDef<std::vector<std::vector<str> > >(default_analyses, "analyses");
          first = false;
        }
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        if (!useBuckFastATLASnoeffDetector) return;

        // Check that there are some analyses to run if the detector is switched on
        if (analyses[indexPythiaNames].empty() and useBuckFastATLASnoeffDetector)
        {
          str errmsg = "The option 'useDetector' for function 'getBuckFastATLASnoeff' is set to true\n";
          errmsg    += "for the collider '";
          errmsg    += *iterPythiaNames;
          errmsg    += "', but the corresponding list of analyses\n";
          errmsg    += "(in option 'analyses' for function 'getATLASnoeffAnalysisContainer') is empty.\n";
          errmsg    += "Please correct your settings.\n";
          ColliderBit_error().raise(LOCAL_INFO, errmsg);
        }

        return;
      }

      if (!useBuckFastATLASnoeffDetector) return;

      if (*Loop::iteration == START_SUBPROCESS)
      {
        // cout << "DEBUG: thread " << my_thread << ": getATLASnoeffAnalysisContainer: Begin START_SUBPROCESS, indexPythiaNames = " << indexPythiaNames  << endl;

        // Register analysis container
        // cout << "DEBUG: thread " << my_thread << ": getATLASnoeffAnalysisContainer: Will run result.register_thread " << endl;
        result.register_thread("ATLASnoeffAnalysisContainer");
        // cout << "DEBUG: thread " << my_thread << ": getATLASnoeffAnalysisContainer: ...done" << endl;

        // Set current collider
        // cout << "DEBUG: thread " << my_thread << ": getATLASnoeffAnalysisContainer: Will run result.set_current_collider " << endl;
        result.set_current_collider(*iterPythiaNames);
        // cout << "DEBUG: thread " << my_thread << ": getATLASnoeffAnalysisContainer: ...done" << endl;

        // Initialize analysis container or reset all the contained analyses
        // cout << "DEBUG: thread " << my_thread << ": getATLASnoeffAnalysisContainer: Will run get_current_analyses_map" << endl;
        if (!result.has_analyses()) result.init(analyses[indexPythiaNames]); 
        else result.reset();
        // cout << "DEBUG: thread " << my_thread << ": getATLASnoeffAnalysisContainer: ...done " << endl;

        // #ifdef COLLIDERBIT_DEBUG
        // if (my_thread == 0)
        // {
        //   for (auto& apair : result.get_current_analyses_map())
        //   {
        //     cout << debug_prefix() << "The run with " << *iterPythiaNames << " will include the analysis " << apair.first << endl;
        //   }
        // }
        // #endif

        // cout << "DEBUG: thread " << my_thread << ": getATLASnoeffAnalysisContainer: End START_SUBPROCESS "  << endl;
        return;
      }

      if (*Loop::iteration == END_SUBPROCESS && eventsGenerated && nFailedEvents <= maxFailedEvents)
      {
        const double xs_fb = Dep::HardScatteringSim->xsec_pb() * 1000.;
        const double xserr_fb = Dep::HardScatteringSim->xsecErr_pb() * 1000.;
        result.add_xsec(xs_fb, xserr_fb);

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "xs_fb = " << xs_fb << " +/- " << xserr_fb << endl;
        #endif
        return;
      }

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        result.collect_and_add_signal();
        result.collect_and_improve_xsec();
        result.scale();        
        return;
      }

    }



    void getCMSAnalysisContainer(HEPUtilsAnalysisContainer& result)
    {
      using namespace Pipes::getCMSAnalysisContainer;
      static std::vector<std::vector<str> > analyses;
      static bool first = true;

      if (*Loop::iteration == BASE_INIT)
      {
        // Only run this once
        if (first)
        {
          // Read analysis names from the yaml file
          std::vector<std::vector<str> > default_analyses;  // The default is empty lists of analyses
          analyses = runOptions->getValueOrDef<std::vector<std::vector<str> > >(default_analyses, "analyses");
          first = false;
        }
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        if (!useBuckFastCMSDetector) return;

        // Check that there are some analyses to run if the detector is switched on
        if (analyses[indexPythiaNames].empty() and useBuckFastCMSDetector)
        {
          str errmsg = "The option 'useDetector' for function 'getBuckFastCMS' is set to true\n";
          errmsg    += "for the collider '";
          errmsg    += *iterPythiaNames;
          errmsg    += "', but the corresponding list of analyses\n";
          errmsg    += "(in option 'analyses' for function 'getCMSAnalysisContainer') is empty.\n";
          errmsg    += "Please correct your settings.\n";
          ColliderBit_error().raise(LOCAL_INFO, errmsg);
        }

        return;
      }

      if (!useBuckFastCMSDetector) return;

      if (*Loop::iteration == START_SUBPROCESS)
      {
        // Register analysis container
        result.register_thread("CMSAnalysisContainer");

        // Set current collider
        result.set_current_collider(*iterPythiaNames);

        // Initialize analysis container or reset all the contained analyses
        if (!result.has_analyses()) result.init(analyses[indexPythiaNames]); 
        else result.reset();

        return;
      }

      if (*Loop::iteration == END_SUBPROCESS && eventsGenerated && nFailedEvents <= maxFailedEvents)
      {
        const double xs_fb = Dep::HardScatteringSim->xsec_pb() * 1000.;
        const double xserr_fb = Dep::HardScatteringSim->xsecErr_pb() * 1000.;
        result.add_xsec(xs_fb, xserr_fb);

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "xs_fb = " << xs_fb << " +/- " << xserr_fb << endl;
        #endif
        return;
      }

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        result.collect_and_add_signal();
        result.collect_and_improve_xsec();
        result.scale();        
        return;
      }

    }


    void getCMSnoeffAnalysisContainer(HEPUtilsAnalysisContainer& result)
    {
      using namespace Pipes::getCMSnoeffAnalysisContainer;
      static std::vector<std::vector<str> > analyses;
      static bool first = true;

      if (*Loop::iteration == BASE_INIT)
      {
        // Only run this once
        if (first)
        {
          // Read analysis names from the yaml file
          std::vector<std::vector<str> > default_analyses;  // The default is empty lists of analyses
          analyses = runOptions->getValueOrDef<std::vector<std::vector<str> > >(default_analyses, "analyses");
          first = false;
        }
      }

      if (*Loop::iteration == COLLIDER_INIT)
      {
        if (!useBuckFastCMSnoeffDetector) return;

        // Check that there are some analyses to run if the detector is switched on
        if (analyses[indexPythiaNames].empty() and useBuckFastCMSnoeffDetector)
        {
          str errmsg = "The option 'useDetector' for function 'getBuckFastCMSnoeff' is set to true\n";
          errmsg    += "for the collider '";
          errmsg    += *iterPythiaNames;
          errmsg    += "', but the corresponding list of analyses\n";
          errmsg    += "(in option 'analyses' for function 'getCMSnoeffAnalysisContainer') is empty.\n";
          errmsg    += "Please correct your settings.\n";
          ColliderBit_error().raise(LOCAL_INFO, errmsg);
        }

        return;
      }

      if (!useBuckFastCMSnoeffDetector) return;

      if (*Loop::iteration == START_SUBPROCESS)
      {
        // Register analysis container
        result.register_thread("CMSnoeffAnalysisContainer");

        // Set current collider
        result.set_current_collider(*iterPythiaNames);

        // Initialize analysis container or reset all the contained analyses
        if (!result.has_analyses()) result.init(analyses[indexPythiaNames]); 
        else result.reset();

        return;
      }

      if (*Loop::iteration == END_SUBPROCESS && eventsGenerated && nFailedEvents <= maxFailedEvents)
      {
        const double xs_fb = Dep::HardScatteringSim->xsec_pb() * 1000.;
        const double xserr_fb = Dep::HardScatteringSim->xsecErr_pb() * 1000.;
        result.add_xsec(xs_fb, xserr_fb);

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "xs_fb = " << xs_fb << " +/- " << xserr_fb << endl;
        #endif
        return;
      }

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        result.collect_and_add_signal();
        result.collect_and_improve_xsec();
        result.scale();        
        return;
      }

    }


    void getIdentityAnalysisContainer(HEPUtilsAnalysisContainer& result)
    {
      using namespace Pipes::getIdentityAnalysisContainer;
      static std::vector<std::vector<str> > analyses;
      static bool first = true;

      if (*Loop::iteration == BASE_INIT)
      {
        // Only run this once
        if (first)
        {
          // Read analysis names from the yaml file
          std::vector<std::vector<str> > default_analyses;  // The default is empty lists of analyses
          analyses = runOptions->getValueOrDef<std::vector<std::vector<str> > >(default_analyses, "analyses");
          first = false;
        }
      }


      if (*Loop::iteration == COLLIDER_INIT)
      {
        if (!useBuckFastIdentityDetector) return;

        // Check that there are some analyses to run if the detector is switched on
        if (analyses[indexPythiaNames].empty() and useBuckFastIdentityDetector)
        {
          str errmsg = "The option 'useDetector' for function 'getBuckFastIdentity' is set to true\n";
          errmsg    += "for the collider '";
          errmsg    += *iterPythiaNames;
          errmsg    += "', but the corresponding list of analyses\n";
          errmsg    += "(in option 'analyses' for function 'getIdentityAnalysisContainer') is empty.\n";
          errmsg    += "Please correct your settings.\n";
          ColliderBit_error().raise(LOCAL_INFO, errmsg);
        }

        return;
      }

      if (!useBuckFastIdentityDetector) return;

      if (*Loop::iteration == START_SUBPROCESS)
      {
        // Register analysis container
        result.register_thread("IdentityAnalysisContainer");

        // Set current collider
        result.set_current_collider(*iterPythiaNames);

        // Initialize analysis container or reset all the contained analyses
        if (!result.has_analyses()) result.init(analyses[indexPythiaNames]); 
        else result.reset();

        return;
      }

      if (*Loop::iteration == END_SUBPROCESS && eventsGenerated && nFailedEvents <= maxFailedEvents)
      {
        const double xs_fb = Dep::HardScatteringSim->xsec_pb() * 1000.;
        const double xserr_fb = Dep::HardScatteringSim->xsecErr_pb() * 1000.;
        result.add_xsec(xs_fb, xserr_fb);

        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "xs_fb = " << xs_fb << " +/- " << xserr_fb << endl;
        #endif
        return;
      }

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        result.collect_and_add_signal();
        result.collect_and_improve_xsec();
        result.scale();        
        return;
      }

    }


    // *** Hard Scattering Event Generators ***

    void generatePythia8Event(Pythia8::Event& result)
    {
      using namespace Pipes::generatePythia8Event;

      if (*Loop::iteration <= BASE_INIT) return;
      result.clear();

      while(nFailedEvents <= maxFailedEvents)
      {
        try
        {
          Dep::HardScatteringSim->nextEvent(result);
          break;
        }
        catch (SpecializablePythia::EventGenerationError& e)
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "SpecializablePythia::EventGenerationError caught in generatePythia8Event. Check the ColliderBit log for event details." << endl;
          #endif
          #pragma omp critical (pythia_event_failure)
          {
            // Update global counter
            nFailedEvents += 1;
            // Store Pythia event record in the logs
            std::stringstream ss;
            result.list(ss, 1);
            logger() << LogTags::debug << "SpecializablePythia::EventGenerationError error caught in generatePythia8Event. Pythia record for event that failed:\n" << ss.str() << EOM;
          }
        }
      }
      // Wrap up event loop if too many events fail.
      if(nFailedEvents > maxFailedEvents)
      {
        Loop::wrapup();
        return;
      }

    }



    // *** Standard Event Format Functions ***

    #ifndef EXCLUDE_DELPHES
    void reconstructDelphesEvent(HEPUtils::Event& result)
    {
      using namespace Pipes::reconstructDelphesEvent;
      if (*Loop::iteration <= BASE_INIT or !useDelphesDetector) return;
      result.clear();

      #pragma omp critical (Delphes)
      {
        try
        {
          (*Dep::DetectorSim).processEvent(*Dep::HardScatteringEvent, result);
        }
        catch (std::runtime_error& e)
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "DelphesVanilla::ProcessEventError caught in reconstructDelphesEvent." << endl;
          #endif

          // Store Pythia event record in the logs
          std::stringstream ss;
          Dep::HardScatteringEvent->list(ss, 1);
          logger() << LogTags::debug << "DelphesVanilla::ProcessEventError caught in reconstructDelphesEvent. Pythia record for event that failed:\n" << ss.str() << EOM;

          str errmsg = "Bad point: reconstructDelphesEvent caught the following runtime error: ";
          errmsg    += e.what();
          piped_invalid_point.request(errmsg);
          Loop::wrapup();
        }
      }
    }
    #endif // not defined EXCLUDE_DELPHES


    void smearEventATLAS(HEPUtils::Event& result)
    {
      using namespace Pipes::smearEventATLAS;
      if (*Loop::iteration <= BASE_INIT or !useBuckFastATLASDetector) return;
      result.clear();

      // Get the next event from Pythia8, convert to HEPUtils::Event, and smear it
      try
      {
        (*Dep::SimpleSmearingSim).processEvent(*Dep::HardScatteringEvent, result);
      }
      catch (Gambit::exception& e)
      {
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "Gambit::exception caught during event conversion in smearEventATLAS. Check the ColliderBit log for details." << endl;
        #endif
        #pragma omp critical (event_conversion_error)
        {
          // Store Pythia event record in the logs
          std::stringstream ss;
          Dep::HardScatteringEvent->list(ss, 1);
          logger() << LogTags::debug << "Gambit::exception error caught in smearEventATLAS. Pythia record for event that failed:\n" << ss.str() << EOM;
        }
        str errmsg = "Bad point: smearEventATLAS caught the following runtime error: ";
        errmsg    += e.what();
        piped_invalid_point.request(errmsg);
        Loop::wrapup();
        return;
      }
    }


    void smearEventATLASnoeff(HEPUtils::Event& result)
    {
      using namespace Pipes::smearEventATLASnoeff;
      if (*Loop::iteration <= BASE_INIT or !useBuckFastATLASnoeffDetector) return;
      result.clear();

      // Get the next event from Pythia8, convert to HEPUtils::Event, and smear it
      try
      {
        (*Dep::SimpleSmearingSim).processEvent(*Dep::HardScatteringEvent, result);
      }
      catch (Gambit::exception& e)
      {
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "Gambit::exception caught during event conversion in smearEventATLASnoeff. Check the ColliderBit log for details." << endl;
        #endif
        #pragma omp critical (event_conversion_error)
        {
          // Store Pythia event record in the logs
          std::stringstream ss;
          Dep::HardScatteringEvent->list(ss, 1);
          logger() << LogTags::debug << "Gambit::exception error caught in smearEventATLASnoeff. Pythia record for event that failed:\n" << ss.str() << EOM;
        }
        str errmsg = "Bad point: smearEventATLASnoeff caught the following runtime error: ";
        errmsg    += e.what();
        piped_invalid_point.request(errmsg);
        Loop::wrapup();
        return;
      }
    }


    void smearEventCMS(HEPUtils::Event& result)
    {
      using namespace Pipes::smearEventCMS;
      if (*Loop::iteration <= BASE_INIT or !useBuckFastCMSDetector) return;
      result.clear();

      // Get the next event from Pythia8, convert to HEPUtils::Event, and smear it
      try
      {
        (*Dep::SimpleSmearingSim).processEvent(*Dep::HardScatteringEvent, result);
      }
      catch (Gambit::exception& e)
      {
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "Gambit::exception caught during event conversion in smearEventCMS. Check the ColliderBit log for details." << endl;
        #endif
        #pragma omp critical (event_conversion_error)
        {
          // Store Pythia event record in the logs
          std::stringstream ss;
          Dep::HardScatteringEvent->list(ss, 1);
          logger() << LogTags::debug << "Gambit::exception error caught in smearEventCMS. Pythia record for event that failed:\n" << ss.str() << EOM;
        }
        str errmsg = "Bad point: smearEventCMS caught the following runtime error: ";
        errmsg    += e.what();
        piped_invalid_point.request(errmsg);
        Loop::wrapup();
        return;
      }
    }


    void smearEventCMSnoeff(HEPUtils::Event& result)
    {
      using namespace Pipes::smearEventCMSnoeff;
      if (*Loop::iteration <= BASE_INIT or !useBuckFastCMSnoeffDetector) return;
      result.clear();

      // Get the next event from Pythia8, convert to HEPUtils::Event, and smear it
      try
      {
        (*Dep::SimpleSmearingSim).processEvent(*Dep::HardScatteringEvent, result);
      }
      catch (Gambit::exception& e)
      {
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "Gambit::exception caught during event conversion in smearEventCMSnoeff. Check the ColliderBit log for details." << endl;
        #endif
        #pragma omp critical (event_conversion_error)
        {
          // Store Pythia event record in the logs
          std::stringstream ss;
          Dep::HardScatteringEvent->list(ss, 1);
          logger() << LogTags::debug << "Gambit::exception error caught in smearEventCMSnoeff. Pythia record for event that failed:\n" << ss.str() << EOM;
        }
        str errmsg = "Bad point: smearEventCMSnoeff caught the following runtime error: ";
        errmsg    += e.what();
        piped_invalid_point.request(errmsg);
        Loop::wrapup();
        return;
      }
    }


    void copyEvent(HEPUtils::Event& result)
    {
      using namespace Pipes::copyEvent;
      if (*Loop::iteration <= BASE_INIT or !useBuckFastIdentityDetector) return;
      result.clear();

      // Get the next event from Pythia8 and convert to HEPUtils::Event
      try
      {
        (*Dep::SimpleSmearingSim).processEvent(*Dep::HardScatteringEvent, result);
      }
      catch (Gambit::exception& e)
      {
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "Gambit::exception caught during event conversion in copyEvent. Check the ColliderBit log for details." << endl;
        #endif
        #pragma omp critical (event_conversion_error)
        {
          // Store Pythia event record in the logs
          std::stringstream ss;
          Dep::HardScatteringEvent->list(ss, 1);
          logger() << LogTags::debug << "Gambit::exception error caught in copyEvent. Pythia record for event that failed:\n" << ss.str() << EOM;
        }
        str errmsg = "Bad point: copyEvent caught the following runtime error: ";
        errmsg    += e.what();
        piped_invalid_point.request(errmsg);
        Loop::wrapup();
        return;
      }
    }



    // *** Analysis Accumulators ***


    #ifndef EXCLUDE_DELPHES
    void runDetAnalyses(AnalysisDataPointers& result)
    {
      using namespace Pipes::runDetAnalyses;
      static MC_convergence_checker convergence;

      if (*Loop::iteration == BASE_INIT)
      {
        result.clear();
        return;
      }

      if (!useDelphesDetector) return;

      if (*Loop::iteration == COLLIDER_INIT)
      {
        convergence.init(indexPythiaNames, *Dep::MC_ConvergenceSettings);
        return;
      }

      if (*Loop::iteration == COLLECT_CONVERGENCE_DATA)
      {
        // Update the convergence tracker with the new results
        convergence.update(*Dep::DetAnalysisContainer);
        return;
      }

      if (*Loop::iteration == CHECK_CONVERGENCE)
      {
        // Call quits on the event loop if every analysis in every analysis container has sufficient statistics
        if (convergence.achieved(*Dep::DetAnalysisContainer)) Loop::wrapup();
        return;
      }

      // #ifdef COLLIDERBIT_DEBUG
      // if (*Loop::iteration == END_SUBPROCESS)
      // {
      //   for (auto& analysis_pointer_pair : Dep::DetAnalysisContainer->get_current_analyses_map())
      //   {
      //     for (auto& sr : analysis_pointer_pair.second->get_results().srdata)
      //     {
      //       cout << debug_prefix() << "runDetAnalyses: signal region " << sr.sr_label << ", n_signal = " << sr.n_signal << endl;
      //     }
      //   }
      // }
      // #endif

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        // The final iteration for this collider: collect results
        for (auto& analysis_pointer_pair : Dep::DetAnalysisContainer->get_current_analyses_map())
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "runDetAnalyses: Collecting result from " << analysis_pointer_pair.first << endl;
          #endif

          str warning;
          result.push_back(analysis_pointer_pair.second->get_results_ptr(warning));
          if (eventsGenerated && nFailedEvents <= maxFailedEvents && !warning.empty())
          {
            ColliderBit_error().raise(LOCAL_INFO, warning);
          }
        }
        return;
      }

      if (*Loop::iteration == BASE_FINALIZE)
      {
        // Final iteration. Just return.
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "runDetAnalyses: 'result' contains " << result.size() << " results." << endl;
        #endif
        return;
      }

      if (*Loop::iteration <= BASE_INIT) return;

      // Loop over analyses and run them... Managed by HEPUtilsAnalysisContainer
      Dep::DetAnalysisContainer->analyze(*Dep::ReconstructedEvent);

    }
    #endif // not defined EXCLUDE_DELPHES



    void runATLASAnalyses(AnalysisDataPointers& result)
    {
      using namespace Pipes::runATLASAnalyses;
      static MC_convergence_checker convergence;

      if (*Loop::iteration == BASE_INIT)
      {
        result.clear();
        return;
      }

      if (!useBuckFastATLASDetector) return;

      if (*Loop::iteration == COLLIDER_INIT)
      {
        convergence.init(indexPythiaNames, *Dep::MC_ConvergenceSettings);
        return;
      }

      if (*Loop::iteration == COLLECT_CONVERGENCE_DATA)
      {
        // Update the convergence tracker with the new results
        convergence.update(*Dep::ATLASAnalysisContainer);
        return;
      }

      if (*Loop::iteration == CHECK_CONVERGENCE)
      {
        // Call quits on the event loop if every analysis in every analysis container has sufficient statistics
        if (convergence.achieved(*Dep::ATLASAnalysisContainer)) Loop::wrapup();
        return;
      }

      // #ifdef COLLIDERBIT_DEBUG
      // if (*Loop::iteration == END_SUBPROCESS)
      // {
      //   for (auto& analysis_pointer_pair : Dep::ATLASAnalysisContainer->get_current_analyses_map())
      //   {
      //     for (auto& sr : analysis_pointer_pair.second->get_results().srdata)
      //     {
      //       cout << debug_prefix() << "runATLASAnalyses: signal region " << sr.sr_label << ", n_signal = " << sr.n_signal << endl;
      //     }
      //   }
      // }
      // #endif

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        // The final iteration for this collider: collect results
        for (auto& analysis_pointer_pair : Dep::ATLASAnalysisContainer->get_current_analyses_map())
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "runATLASAnalyses: Collecting result from " << analysis_pointer_pair.first << endl;
          #endif

          str warning;
          result.push_back(analysis_pointer_pair.second->get_results_ptr(warning));
          if (eventsGenerated && nFailedEvents <= maxFailedEvents && !warning.empty())
          {
            ColliderBit_error().raise(LOCAL_INFO, warning);
          }
        }
        return;
      }

      if (*Loop::iteration == BASE_FINALIZE)
      {
        // Final iteration. Just return.
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "runATLASAnalyses: 'result' contains " << result.size() << " results." << endl;
        #endif
        return;
      }

      if (*Loop::iteration <= BASE_INIT) return;

      // Loop over analyses and run them... Managed by HEPUtilsAnalysisContainer
      Dep::ATLASAnalysisContainer->analyze(*Dep::ATLASSmearedEvent);

    }


    void runATLASnoeffAnalyses(AnalysisDataPointers& result)
    {
      using namespace Pipes::runATLASnoeffAnalyses;
      static MC_convergence_checker convergence;

      if (*Loop::iteration == BASE_INIT)
      {
        result.clear();
        return;
      }

      if (!useBuckFastATLASnoeffDetector) return;

      if (*Loop::iteration == COLLIDER_INIT)
      {
        convergence.init(indexPythiaNames, *Dep::MC_ConvergenceSettings);
        return;
      }

      if (*Loop::iteration == COLLECT_CONVERGENCE_DATA)
      {
        // Update the convergence tracker with the new results
        convergence.update(*Dep::ATLASnoeffAnalysisContainer);
        return;
      }

      if (*Loop::iteration == CHECK_CONVERGENCE)
      {
        // Call quits on the event loop if every analysis in every analysis container has sufficient statistics
        if (convergence.achieved(*Dep::ATLASnoeffAnalysisContainer)) Loop::wrapup();
        return;
      }

      // #ifdef COLLIDERBIT_DEBUG
      // if (*Loop::iteration == END_SUBPROCESS)
      // {
      //   for (auto& analysis_pointer_pair : Dep::ATLASnoeffAnalysisContainer->get_current_analyses_map())
      //   {
      //     for (auto& sr : analysis_pointer_pair.second->get_results().srdata)
      //     {
      //       cout << debug_prefix() << "runATLASnoeffAnalyses: signal region " << sr.sr_label << ", n_signal = " << sr.n_signal << endl;
      //     }
      //   }
      // }
      // #endif

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        // The final iteration for this collider: collect results
        for (auto& analysis_pointer_pair : Dep::ATLASnoeffAnalysisContainer->get_current_analyses_map())
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "runATLASnoeffAnalyses: Collecting result from " << analysis_pointer_pair.first << endl;
          #endif

          str warning;
          result.push_back(analysis_pointer_pair.second->get_results_ptr(warning));
          if (eventsGenerated && nFailedEvents <= maxFailedEvents && !warning.empty())
          {
            ColliderBit_error().raise(LOCAL_INFO, warning);
          }
        }
        return;
      }

      if (*Loop::iteration == BASE_FINALIZE)
      {
        // Final iteration. Just return.
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "runATLASnoeffAnalyses: 'result' contains " << result.size() << " results." << endl;
        #endif
        return;
      }

      if (*Loop::iteration <= BASE_INIT) return;

      // Loop over analyses and run them... Managed by HEPUtilsAnalysisContainer
      Dep::ATLASnoeffAnalysisContainer->analyze(*Dep::ATLASnoeffSmearedEvent);

    }


    void runCMSAnalyses(AnalysisDataPointers& result)
    {
      using namespace Pipes::runCMSAnalyses;
      static MC_convergence_checker convergence;

      if (*Loop::iteration == BASE_INIT)
      {
        result.clear();
        return;
      }

      if (!useBuckFastCMSDetector) return;

      if (*Loop::iteration == COLLIDER_INIT)
      {
        convergence.init(indexPythiaNames, *Dep::MC_ConvergenceSettings);
        return;
      }

      if (*Loop::iteration == COLLECT_CONVERGENCE_DATA)
      {
        // Update the convergence tracker with the new results
        convergence.update(*Dep::CMSAnalysisContainer);
        return;
      }

      if (*Loop::iteration == CHECK_CONVERGENCE)
      {
        // Call quits on the event loop if every analysis in every analysis container has sufficient statistics
        if (convergence.achieved(*Dep::CMSAnalysisContainer)) Loop::wrapup();
        return;
      }

      // #ifdef COLLIDERBIT_DEBUG
      // if (*Loop::iteration == END_SUBPROCESS)
      // {
      //   for (auto& analysis_pointer_pair : Dep::CMSAnalysisContainer->get_current_analyses_map())
      //   {
      //     for (auto& sr : analysis_pointer_pair.second->get_results().srdata)
      //     {
      //       cout << debug_prefix() << "runCMSAnalyses: signal region " << sr.sr_label << ", n_signal = " << sr.n_signal << endl;
      //     }
      //   }
      // }
      // #endif

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        // The final iteration for this collider: collect results
        for (auto& analysis_pointer_pair : Dep::CMSAnalysisContainer->get_current_analyses_map())
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "runCMSAnalyses: Collecting result from " << analysis_pointer_pair.first << endl;
          #endif

          str warning;
          result.push_back(analysis_pointer_pair.second->get_results_ptr(warning));
          if (eventsGenerated && nFailedEvents <= maxFailedEvents && !warning.empty())
          {
            ColliderBit_error().raise(LOCAL_INFO, warning);
          }
        }
        return;
      }

      if (*Loop::iteration == BASE_FINALIZE)
      {
        // Final iteration. Just return.
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "runCMSAnalyses: 'result' contains " << result.size() << " results." << endl;
        #endif
        return;
      }

      if (*Loop::iteration <= BASE_INIT) return;

      // Loop over analyses and run them... Managed by HEPUtilsAnalysisContainer
      Dep::CMSAnalysisContainer->analyze(*Dep::CMSSmearedEvent);

    }


    void runCMSnoeffAnalyses(AnalysisDataPointers& result)
    {
      using namespace Pipes::runCMSnoeffAnalyses;
      static MC_convergence_checker convergence;

      if (*Loop::iteration == BASE_INIT)
      {
        result.clear();
        return;
      }

      if (!useBuckFastCMSnoeffDetector) return;

      if (*Loop::iteration == COLLIDER_INIT)
      {
        convergence.init(indexPythiaNames, *Dep::MC_ConvergenceSettings);
        return;
      }

      if (*Loop::iteration == COLLECT_CONVERGENCE_DATA)
      {
        // Update the convergence tracker with the new results
        convergence.update(*Dep::CMSnoeffAnalysisContainer);
        return;
      }

      if (*Loop::iteration == CHECK_CONVERGENCE)
      {
        // Call quits on the event loop if every analysis in every analysis container has sufficient statistics
        if (convergence.achieved(*Dep::CMSnoeffAnalysisContainer)) Loop::wrapup();
        return;
      }

      // #ifdef COLLIDERBIT_DEBUG
      // if (*Loop::iteration == END_SUBPROCESS)
      // {
      //   for (auto& analysis_pointer_pair : Dep::CMSnoeffAnalysisContainer->get_current_analyses_map())
      //   {
      //     for (auto& sr : analysis_pointer_pair.second->get_results().srdata)
      //     {
      //       cout << debug_prefix() << "runCMSnoeffAnalyses: signal region " << sr.sr_label << ", n_signal = " << sr.n_signal << endl;
      //     }
      //   }
      // }
      // #endif

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        // The final iteration for this collider: collect results
        for (auto& analysis_pointer_pair : Dep::CMSnoeffAnalysisContainer->get_current_analyses_map())
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "runCMSnoeffAnalyses: Collecting result from " << analysis_pointer_pair.first << endl;
          #endif

          str warning;
          result.push_back(analysis_pointer_pair.second->get_results_ptr(warning));
          if (eventsGenerated && nFailedEvents <= maxFailedEvents && !warning.empty())
          {
            ColliderBit_error().raise(LOCAL_INFO, warning);
          }
        }
        return;
      }

      if (*Loop::iteration == BASE_FINALIZE)
      {
        // Final iteration. Just return.
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "runCMSnoeffAnalyses: 'result' contains " << result.size() << " results." << endl;
        #endif
        return;
      }

      if (*Loop::iteration <= BASE_INIT) return;

      // Loop over analyses and run them... Managed by HEPUtilsAnalysisContainer
      Dep::CMSnoeffAnalysisContainer->analyze(*Dep::CMSnoeffSmearedEvent);

    }


    void runIdentityAnalyses(AnalysisDataPointers& result)
    {
      using namespace Pipes::runIdentityAnalyses;
      static MC_convergence_checker convergence;

      if (*Loop::iteration == BASE_INIT)
      {
        result.clear();
        return;
      }

      if (!useBuckFastIdentityDetector) return;

      if (*Loop::iteration == COLLIDER_INIT)
      {
        convergence.init(indexPythiaNames, *Dep::MC_ConvergenceSettings);
        return;
      }

      if (*Loop::iteration == COLLECT_CONVERGENCE_DATA)
      {
        // Update the convergence tracker with the new results
        convergence.update(*Dep::IdentityAnalysisContainer);
        return;
      }

      if (*Loop::iteration == CHECK_CONVERGENCE)
      {
        // Call quits on the event loop if every analysis in every analysis container has sufficient statistics
        if (convergence.achieved(*Dep::IdentityAnalysisContainer)) Loop::wrapup();
        return;
      }

      // #ifdef COLLIDERBIT_DEBUG
      // if (*Loop::iteration == END_SUBPROCESS)
      // {
      //   for (auto& analysis_pointer_pair : Dep::IdentityAnalysisContainer->get_current_analyses_map())
      //   {
      //     for (auto& sr : analysis_pointer_pair.second->get_results().srdata)
      //     {
      //       cout << debug_prefix() << "runIdentityAnalyses: signal region " << sr.sr_label << ", n_signal = " << sr.n_signal << endl;
      //     }
      //   }
      // }
      // #endif

      if (*Loop::iteration == COLLIDER_FINALIZE)
      {
        // The final iteration for this collider: collect results
        for (auto& analysis_pointer_pair : Dep::IdentityAnalysisContainer->get_current_analyses_map())
        {
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "runIdentityAnalyses: Collecting result from " << analysis_pointer_pair.first << endl;
          #endif

          str warning;
          result.push_back(analysis_pointer_pair.second->get_results_ptr(warning));
          if (eventsGenerated && nFailedEvents <= maxFailedEvents && !warning.empty())
          {
            ColliderBit_error().raise(LOCAL_INFO, warning);
          }
        }
        return;
      }

      if (*Loop::iteration == BASE_FINALIZE)
      {
        // Final iteration. Just return.
        #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "runIdentityAnalyses: 'result' contains " << result.size() << " results." << endl;
        #endif
        return;
      }

      if (*Loop::iteration <= BASE_INIT) return;

      // Loop over analyses and run them... Managed by HEPUtilsAnalysisContainer
      Dep::IdentityAnalysisContainer->analyze(*Dep::CopiedEvent);

    }


    // Loop over all analyses and fill a map of predicted counts
    void CollectAnalyses(AnalysisDataPointers& result)
    {
      using namespace Pipes::CollectAnalyses;
      static bool first = true;

      // Start with an empty vector
      result.clear();

      #ifdef COLLIDERBIT_DEBUG
      if (haveUsedBuckFastATLASDetector)
        cout << debug_prefix() << "CollectAnalyses: Dep::ATLASAnalysisNumbers->size()    = " << Dep::ATLASAnalysisNumbers->size() << endl;
      if (haveUsedBuckFastATLASnoeffDetector)
        cout << debug_prefix() << "CollectAnalyses: Dep::ATLASnoeffAnalysisNumbers->size()    = " << Dep::ATLASnoeffAnalysisNumbers->size() << endl;
      if (haveUsedBuckFastCMSDetector)
        cout << debug_prefix() << "CollectAnalyses: Dep::CMSAnalysisNumbers->size()      = " << Dep::CMSAnalysisNumbers->size() << endl;
      if (haveUsedBuckFastCMSnoeffDetector)
        cout << debug_prefix() << "CollectAnalyses: Dep::CMSnoeffAnalysisNumbers->size() = " << Dep::CMSnoeffAnalysisNumbers->size() << endl;
      if (haveUsedBuckFastIdentityDetector)
        cout << debug_prefix() << "CollectAnalyses: Dep::IdentityAnalysisNumbers->size() = " << Dep::IdentityAnalysisNumbers->size() << endl;
      #ifndef EXCLUDE_DELPHES
      if (haveUsedDelphesDetector)
        cout << debug_prefix() << "CollectAnalyses: Dep::DetAnalysisNumbers->size()      = " << Dep::DetAnalysisNumbers->size() << endl;
      #endif
      #endif

      // Add results 
      if (haveUsedBuckFastATLASDetector)
        result.insert(result.end(), Dep::ATLASAnalysisNumbers->begin(), Dep::ATLASAnalysisNumbers->end());
      if (haveUsedBuckFastATLASnoeffDetector)
        result.insert(result.end(), Dep::ATLASnoeffAnalysisNumbers->begin(), Dep::ATLASnoeffAnalysisNumbers->end());
      if (haveUsedBuckFastCMSDetector)
        result.insert(result.end(), Dep::CMSAnalysisNumbers->begin(), Dep::CMSAnalysisNumbers->end());
      if (haveUsedBuckFastCMSnoeffDetector)
        result.insert(result.end(), Dep::CMSnoeffAnalysisNumbers->begin(), Dep::CMSnoeffAnalysisNumbers->end());
      if (haveUsedBuckFastIdentityDetector)
        result.insert(result.end(), Dep::IdentityAnalysisNumbers->begin(), Dep::IdentityAnalysisNumbers->end());
      #ifndef EXCLUDE_DELPHES
      if (haveUsedDelphesDetector)
        result.insert(result.end(), Dep::DetAnalysisNumbers->begin(), Dep::DetAnalysisNumbers->end());
      #endif

      // When first called, check that all analyses contain at least one signal region.
      if (first)
      {
        // Loop over all AnalysisData pointers
        for (auto& adp : result)
        {
          if (adp->size() == 0)
          {
            str errmsg;
            errmsg = "The analysis " + adp->analysis_name + " has no signal regions.";
            ColliderBit_error().raise(LOCAL_INFO, errmsg);
          }
        }
        first = false;
      }


      // #ifdef COLLIDERBIT_DEBUG
      // cout << debug_prefix() << "CollectAnalyses: Current size of 'result': " << result.size() << endl;        
      // if (result.size() > 0)
      // {
      //   cout << debug_prefix() << "CollectAnalyses: Will loop through 'result'..." << endl;        
      //   for (auto& adp : result)
      //   {
      //     cout << debug_prefix() << "CollectAnalyses: 'result' contains AnalysisData pointer to " << adp << endl;        
      //     cout << debug_prefix() << "CollectAnalyses: -- Will now loop over all signal regions in " << adp << endl;        
      //     for (auto& sr : adp->srdata)
      //     {
      //       cout << debug_prefix() << "CollectAnalyses: -- " << adp << " contains signal region: " << sr.sr_label << ", n_signal = " << sr.n_signal << ", n_signal_at_lumi = " << n_signal_at_lumi << endl;        
      //     }
      //     cout << debug_prefix() << "CollectAnalyses: -- Done looping over signal regions in " << adp << endl;        
      //   }
      //   cout << debug_prefix() << "CollectAnalyses: ...Done looping through 'result'." << endl;        
      // }
      // #endif
    }


    // Loop over all analyses and fill a map of predictions
    void calc_LHC_signals(map_str_dbl& result)
    {
      using namespace Pipes::calc_LHC_signals;

      // Clear the result map
      result.clear();

      // Loop over analyses and collect the predicted events into the map
      for (size_t analysis = 0; analysis < Dep::AllAnalysisNumbers->size(); ++analysis)
      {
        // AnalysisData for this analysis
        const AnalysisData& adata = *(Dep::AllAnalysisNumbers->at(analysis));

        // Loop over the signal regions inside the analysis, and save the predicted number of events for each.
        for (size_t SR = 0; SR < adata.size(); ++SR)
        {
          // Save SR numbers and absolute uncertainties
          const SignalRegionData srData = adata[SR];
          const str key = adata.analysis_name + "_" + srData.sr_label + "_signal";
          result[key] = srData.n_signal_at_lumi;
          const double abs_uncertainty_s_stat = (srData.n_signal == 0 ? 0 : sqrt(srData.n_signal) * (srData.n_signal_at_lumi/srData.n_signal));
          const double abs_uncertainty_s_sys = srData.signal_sys;
          result[key + "_uncert"] = HEPUtils::add_quad(abs_uncertainty_s_stat, abs_uncertainty_s_sys);
        }
      }
    }


    // Loop over all analyses (and SRs within one analysis) and fill a map of per-analysis likelihoods
    void calc_LHC_LogLike_per_analysis(map_str_dbl& result)
    {
      using namespace Pipes::calc_LHC_LogLike_per_analysis;

      // Clear the result map
      result.clear();

      // Loop over analyses and calculate the observed dLL for each
      for (size_t analysis = 0; analysis < Dep::AllAnalysisNumbers->size(); ++analysis)
      {
        // AnalysisData for this analysis
        const AnalysisData& adata = *(Dep::AllAnalysisNumbers->at(analysis));

        #ifdef COLLIDERBIT_DEBUG
        std::streamsize stream_precision = cout.precision();  // get current precision
        cout.precision(2);  // set precision
        cout << debug_prefix() << "calc_LHC_LogLike_per_analysis: " << "Will print content of " << adata.analysis_name << " signal regions:" << endl;
        for (size_t SR = 0; SR < adata.size(); ++SR)
        {
          const SignalRegionData& srData = adata[SR];
          cout << std::fixed << debug_prefix() 
                                 << "calc_LHC_LogLike_per_analysis: " << adata.analysis_name 
                                 << ", " << srData.sr_label 
                                 << ",  n_b = " << srData.n_background << " +/- " << srData.background_sys
                                 << ",  n_obs = " << srData.n_observed 
                                 << ",  excess = " << srData.n_observed - srData.n_background << " +/- " << srData.background_sys
                                 << ",  n_s = " << srData.n_signal_at_lumi
                                 << ",  (excess-n_s) = " << (srData.n_observed-srData.n_background) - srData.n_signal_at_lumi << " +/- " << srData.background_sys
                                 << endl; 
        }
        cout.precision(stream_precision); // restore previous precision
        #endif


        /// If no events have been generated (xsec veto) or too many events have failed, 
        /// short-circut the loop and return delta log-likelihood = 0 for each analysis.
        /// @todo This must be made more sophisticated once we add analyses that
        ///       don't rely on event generation.
        if (!eventsGenerated || nFailedEvents > maxFailedEvents)
        {
          result[adata.analysis_name] = 0.0;
          continue;
        }

        // Loop over the signal regions inside the analysis, and work out the total (delta) log likelihood for this analysis
        /// @todo Unify the treatment of best-only and correlated SR treatments as far as possible
        /// @todo Come up with a good treatment of zero and negative predictions
        if (adata.srcov.rows() > 0)
        {
          /// If (simplified) SR-correlation info is available, so use the
          /// covariance matrix to construct composite marginalised likelihood
          /// Despite initial thoughts, we can't just do independent LL
          /// calculations in a rotated basis, but have to sample from the
          /// covariance matrix.
          ///
          /// @note This means we can't use the nulike LL functions, which
          /// operate in 1D only.  Also, log-normal sampling in the diagonal
          /// basis is not helpful, since the rotation will re-generate negative
          /// rates.
          ///
          /// @todo How about Gaussian sampling in the log(rate) space? Would
          /// protect against negatives in any SR. Requires care with the
          /// explicit transformation of widths.
          ///
          /// @todo Support skewness correction to the pdf.

          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLike_per_analysis: Analysis " << analysis << " has a covariance matrix: computing composite loglike." << endl;
          #endif

          // Construct vectors of SR numbers
          Eigen::ArrayXd n_obs(adata.size()), n_pred_b(adata.size()), n_pred_sb(adata.size()), abs_unc_s(adata.size());
          for (size_t SR = 0; SR < adata.size(); ++SR)
          {
            const SignalRegionData srData = adata[SR];

            // Actual observed number of events
            n_obs(SR) = srData.n_observed;

            // A contribution to the predicted number of events that is not known exactly
            n_pred_b(SR) = srData.n_background;
            n_pred_sb(SR) = srData.n_signal_at_lumi + srData.n_background;

            // Absolute errors for n_predicted_uncertain_*
            const double abs_uncertainty_s_stat = (srData.n_signal == 0 ? 0 : sqrt(srData.n_signal) * (srData.n_signal_at_lumi/srData.n_signal));
            const double abs_uncertainty_s_sys = srData.signal_sys;
            abs_unc_s(SR) = HEPUtils::add_quad(abs_uncertainty_s_stat, abs_uncertainty_s_sys);
          }

          // Diagonalise the background-only covariance matrix, extracting the rotation matrix
          /// @todo No need to recompute the background-only covariance decomposition for every point!
          const Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig_b(adata.srcov);
          const Eigen::ArrayXd Eb = eig_b.eigenvalues();
          const Eigen::ArrayXd sqrtEb = Eb.sqrt();
          const Eigen::MatrixXd Vb = eig_b.eigenvectors();
          const Eigen::MatrixXd Vbinv = Vb.inverse();

          // Construct and diagonalise the s+b covariance matrix, adding the diagonal signal uncertainties in quadrature
          /// @todo Is this the best way, or should we just sample the s numbers independently and then be able to completely cache the cov matrix diagonalisation?
          const Eigen::MatrixXd srcov_s = abs_unc_s.array().square().matrix().asDiagonal();
          const Eigen::MatrixXd srcov_sb = adata.srcov + srcov_s;
          const Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig_sb(srcov_sb);
          const Eigen::ArrayXd Esb = eig_sb.eigenvalues();
          const Eigen::ArrayXd sqrtEsb = Esb.sqrt();
          const Eigen::MatrixXd Vsb = eig_sb.eigenvectors();
          const Eigen::MatrixXd Vsbinv = Vsb.inverse();


          ///////////////////
          /// @todo Split this whole chunk off into a lnlike-style utility function?

          // Sample correlated SR rates from a rotated Gaussian defined by the covariance matrix and offset by the mean rates
          static const double CONVERGENCE_TOLERANCE_ABS = runOptions->getValueOrDef<double>(0.05, "covariance_marg_convthres_abs");
          static const double CONVERGENCE_TOLERANCE_REL = runOptions->getValueOrDef<double>(0.05, "covariance_marg_convthres_rel");
          static const size_t nsample_input = runOptions->getValueOrDef<size_t>(100000, "covariance_nsamples_start");
          size_t NSAMPLE = nsample_input;

          double diff_abs = 9999;
          double diff_rel = 1;
          double ana_dll_prev = 0;
          double ana_dll = 0;
          double llrsum_prev = 0;
          double llrsum = 0;
          bool first_iteration = true;

          // Check absolute difference between independent estimates
          /// @todo Should also implement a check of relative difference
          while ((diff_abs > CONVERGENCE_TOLERANCE_ABS && diff_rel > CONVERGENCE_TOLERANCE_REL) || 1.0/sqrt(NSAMPLE) > CONVERGENCE_TOLERANCE_ABS) 
          {
            Eigen::ArrayXd llrsums = Eigen::ArrayXd::Zero(adata.size());

            /// @note How to correct negative rates? Discard (scales badly), set to
            /// epsilon (= discontinuous & unphysical pdf), transform to log-space
            /// (distorts the pdf quite badly), or something else (skew term)?
            /// We're using the "set to epsilon" version for now.
            ///
            /// @todo Add option for normal sampling in log(rate), i.e. "multidimensional log-normal"

            const bool COVLOGNORMAL = false;
            if (!COVLOGNORMAL) 
            {

              #pragma omp parallel
              {
                std::normal_distribution<> unitnormdbn{0,1};
                Eigen::ArrayXd llrsums_private = Eigen::ArrayXd::Zero(adata.size());

                #pragma omp for nowait

                // Sample correlated SR rates from a rotated Gaussian defined by the covariance matrix and offset by the mean rates
                for (size_t i = 0; i < NSAMPLE; ++i) {

                  Eigen::VectorXd norm_sample_b(adata.size()), norm_sample_sb(adata.size());
                  for (size_t j = 0; j < adata.size(); ++j) {
                    norm_sample_b(j) = sqrtEb(j) * unitnormdbn(Random::rng());
                    norm_sample_sb(j) = sqrtEsb(j) * unitnormdbn(Random::rng());
                  }

                  // Rotate rate deltas into the SR basis and shift by SR mean rates
                  const Eigen::VectorXd n_pred_b_sample = n_pred_b + (Vb*norm_sample_b).array();
                  const Eigen::VectorXd n_pred_sb_sample = n_pred_sb + (Vsb*norm_sample_sb).array();

                  // Calculate Poisson LLR and add to aggregated LL calculation
                  for (size_t j = 0; j < adata.size(); ++j) {
                    const double lambda_b_j = std::max(n_pred_b_sample(j), 1e-3); //< manually avoid <= 0 rates
                    const double lambda_sb_j = std::max(n_pred_sb_sample(j), 1e-3); //< manually avoid <= 0 rates
                    const double llr_j = n_obs(j)*log(lambda_sb_j/lambda_b_j) - (lambda_sb_j - lambda_b_j);
                    llrsums_private(j) += llr_j;
                  }
                }

                #pragma omp critical
                {
                  for (size_t j = 0; j < adata.size(); ++j) { llrsums(j) += llrsums_private(j); }
                }
              } // End omp parallel
            }  // End if !COVLOGNORMAL

            /// @todo Check that this log-normal sampling works as expected.
            else // COVLOGNORMAL
            { 

              const Eigen::ArrayXd ln_n_pred_b = n_pred_b.log();
              const Eigen::ArrayXd ln_n_pred_sb = n_pred_sb.log();
              const Eigen::ArrayXd ln_sqrtEb = (n_pred_b + sqrtEb).log() - ln_n_pred_b;
              const Eigen::ArrayXd ln_sqrtEsb = (n_pred_sb + sqrtEsb).log() - ln_n_pred_sb;

              #pragma omp parallel
              {
                std::normal_distribution<> unitnormdbn{0,1};
                Eigen::ArrayXd llrsums_private = Eigen::ArrayXd::Zero(adata.size());

                #pragma omp for nowait

                // Sample correlated SR rates from a rotated Gaussian defined by the covariance matrix and offset by the mean rates
                for (size_t i = 0; i < NSAMPLE; ++i) {
                  Eigen::VectorXd ln_norm_sample_b(adata.size()), ln_norm_sample_sb(adata.size());
                  for (size_t j = 0; j < adata.size(); ++j) {
                    ln_norm_sample_b(j) = ln_sqrtEb(j) * unitnormdbn(Random::rng());
                    ln_norm_sample_sb(j) = ln_sqrtEsb(j) * unitnormdbn(Random::rng());
                  }

                  // Rotate rate deltas into the SR basis and shift by SR mean rates
                  const Eigen::ArrayXd delta_ln_n_pred_b_sample = Vb*ln_norm_sample_b;
                  const Eigen::ArrayXd delta_ln_n_pred_sb_sample = Vsb*ln_norm_sample_sb;
                  const Eigen::ArrayXd n_pred_b_sample = (ln_n_pred_b + delta_ln_n_pred_b_sample).exp();
                  const Eigen::ArrayXd n_pred_sb_sample = (ln_n_pred_sb + delta_ln_n_pred_sb_sample).exp();

                  // Calculate Poisson LLR and add to aggregated LL calculation
                  for (size_t j = 0; j < adata.size(); ++j) {
                    const double lambda_b_j = std::max(n_pred_b_sample(j), 1e-3); //< shouldn't be needed in log-space sampling
                    const double lambda_sb_j = std::max(n_pred_sb_sample(j), 1e-3); //< shouldn't be needed in log-space sampling
                    const double llr_j = n_obs(j)*log(lambda_sb_j/lambda_b_j) - (lambda_sb_j - lambda_b_j);
                    llrsums_private(j) += llr_j;
                  }
                }

                #pragma omp critical
                {
                  for (size_t j = 0; j < adata.size(); ++j) { llrsums(j) += llrsums_private(j); }
                }
              } // End omp parallel
            }


            // Calculate sum of expected LLRs and compare to previous independent batch
            llrsum = llrsums.sum();
            if (first_iteration)  // The first round must be generated twice
            {
              llrsum_prev = llrsum;
              first_iteration = false;
            } 
            else 
            {
              ana_dll_prev = llrsum_prev / (double)NSAMPLE;
              ana_dll = llrsum / (double)NSAMPLE;
              diff_rel = fabs((ana_dll_prev - ana_dll)/ana_dll);  // Relative convergence check
              diff_abs = fabs(ana_dll_prev - ana_dll);  // Absolute convergence check

              // Update variables 
              llrsum_prev += llrsum;  // Aggregate result. This doubles the effective batch size for llrsum_prev.
              NSAMPLE *=2;  // This ensures that the  next batch for llrsum is as big as the current batch size for llrsum_prev.
            }

            #ifdef COLLIDERBIT_DEBUG
              cout << debug_prefix() << "diff_rel: " << diff_rel <<  "   diff_abs: " << diff_abs << "   ana_dll_prev: " << ana_dll_prev << "   ana_dll: " << ana_dll << endl;
              cout << debug_prefix() << "NSAMPLE for the next iteration is: " << NSAMPLE << endl;
              cout << debug_prefix() << endl;
            #endif
          }  // End while loop

          // Combine the independent estimates ana_dll and ana_dll_prev. 
          // Use equal weights since the estimates are based on equal batch sizes.
          ana_dll = 0.5*(ana_dll + ana_dll_prev);
          #ifdef COLLIDERBIT_DEBUG
            cout << debug_prefix() << "Combined estimate: ana_dll: " << ana_dll << "   (based on 2*NSAMPLE=" << 2*NSAMPLE << " samples)" << endl;
          #endif

          // Check for problem
          if (Utils::isnan(ana_dll))
          {
            std::stringstream msg;
            msg << "Computation of composite loglike for analysis " << adata.analysis_name << " returned NaN. Will now print the signal region data for this analysis:" << endl;
            for (size_t SR = 0; SR < adata.size(); ++SR)
            {
              const SignalRegionData& srData = adata[SR];
              msg << srData.sr_label 
                  << ",  n_background = " << srData.n_background 
                  << ",  background_sys = " << srData.background_sys
                  << ",  n_observed = " << srData.n_observed 
                  << ",  n_signal_at_lumi = " << srData.n_signal_at_lumi
                  << ",  n_signal = " << srData.n_signal
                  << ",  signal_sys = " << srData.signal_sys
                  << endl; 
            }
            invalid_point().raise(msg.str());
          }

          // Store result
          result[adata.analysis_name] = ana_dll;

          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLike_per_analysis: " << adata.analysis_name << "_DeltaLogLike : " << ana_dll << endl;
          #endif

        }

        else
        {
          // No SR-correlation info, so just take the result from the SR *expected* to be most constraining, i.e. with highest expected dLL
          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLike_per_analysis: Analysis " << analysis << " has no covariance matrix: computing single best-expected loglike." << endl;
          #endif

          double bestexp_dll_exp = 0, bestexp_dll_obs = 0;
          str bestexp_sr_label;

          for (size_t SR = 0; SR < adata.size(); ++SR)
          {
            const SignalRegionData &srData = adata[SR];

            // Actual observed number of events
            const int n_obs = (int) round(srData.n_observed);

            // A contribution to the predicted number of events that is known exactly
            // (e.g. from data-driven background estimate)
            const double n_predicted_exact = 0;

            // A contribution to the predicted number of events that is not known exactly
            const double n_predicted_uncertain_s = srData.n_signal_at_lumi;
            const double n_predicted_uncertain_b = srData.n_background;
            const double n_predicted_uncertain_sb = n_predicted_uncertain_s + n_predicted_uncertain_b;

            // Absolute errors for n_predicted_uncertain_*
            const double abs_uncertainty_s_stat = (srData.n_signal == 0 ? 0 : sqrt(srData.n_signal) * (srData.n_signal_at_lumi/srData.n_signal));
            const double abs_uncertainty_s_sys = srData.signal_sys;
            const double abs_uncertainty_b = srData.background_sys;
            const double abs_uncertainty_sb = HEPUtils::add_quad(abs_uncertainty_s_stat, abs_uncertainty_s_sys, abs_uncertainty_b);

            // Relative errors for n_predicted_uncertain_*
            const double frac_uncertainty_b = abs_uncertainty_b / n_predicted_uncertain_b;
            const double frac_uncertainty_sb = abs_uncertainty_sb / n_predicted_uncertain_sb;

            // Predicted total background, as an integer for use in Poisson functions
            const int n_predicted_total_b_int = (int) round(n_predicted_exact + n_predicted_uncertain_b);

            // Marginalise over systematic uncertainties on mean rates
            // Use a log-normal/Gaussia distribution for the nuisance parameter, as requested
            auto marginaliser = (*BEgroup::lnlike_marg_poisson == "lnlike_marg_poisson_lognormal_error")
              ? BEreq::lnlike_marg_poisson_lognormal_error : BEreq::lnlike_marg_poisson_gaussian_error;
            const double llb_exp =  marginaliser(n_predicted_total_b_int, n_predicted_exact, n_predicted_uncertain_b, frac_uncertainty_b);
            const double llsb_exp = marginaliser(n_predicted_total_b_int, n_predicted_exact, n_predicted_uncertain_sb, frac_uncertainty_sb);
            const double llb_obs =  marginaliser(n_obs, n_predicted_exact, n_predicted_uncertain_b, frac_uncertainty_b);
            const double llsb_obs = marginaliser(n_obs, n_predicted_exact, n_predicted_uncertain_sb, frac_uncertainty_sb);

            // Calculate the expected dll and set the bestexp values for exp and obs dll if this one is the best so far
            const double dll_exp = llb_exp - llsb_exp; //< note positive dll convention -> more exclusion here
            #ifdef COLLIDERBIT_DEBUG
            cout << debug_prefix() << adata.analysis_name << ", " << srData.sr_label << ",  llsb_exp-llb_exp = " << llsb_exp-llb_exp << ",  llsb_obs-llb_obs= " << llsb_obs - llb_obs << endl;
            #endif
            if (dll_exp > bestexp_dll_exp || SR == 0)
            {
              bestexp_dll_exp = dll_exp;
              bestexp_dll_obs = llb_obs - llsb_obs;
              bestexp_sr_label = srData.sr_label;
              // #ifdef COLLIDERBIT_DEBUG
              // cout << debug_prefix() << "Setting bestexp_sr_label to: " << bestexp_sr_label << ", LogL_exp = " << -bestexp_dll_exp << ", LogL_obs = " << -bestexp_dll_obs << endl;
              // #endif
            }

            // // For debugging: print some useful numbers to the log.
            // #ifdef COLLIDERBIT_DEBUG
            // cout << endl;
            // cout << debug_prefix() << "COLLIDER_RESULT: " << adata.analysis_name << ", SR: " << srData.sr_label << endl;
            // cout << debug_prefix() << "  LLikes: b_ex      sb_ex     b_obs     sb_obs    (sb_obs-b_obs)" << endl;
            // cout << debug_prefix() << "          " << llb_exp << "  " << llsb_exp << "  " << llb_obs << "  " << llsb_obs << "  " << llsb_obs-llb_obs << endl;
            // cout << debug_prefix() << "  NEvents, not scaled to luminosity: " << srData.n_signal << endl;
            // cout << debug_prefix() << "  NEvents, scaled  to luminosity:    " << srData.n_signal_at_lumi << endl;
            // cout << debug_prefix() << "  NEvents: b [rel err]      sb [rel err]" << endl;
            // cout << debug_prefix() << "           "
            //      << n_predicted_uncertain_b << " [" << 100*frac_uncertainty_b << "%]  "
            //      << n_predicted_uncertain_sb << " [" << 100*frac_uncertainty_sb << "%]" << endl;
            // #endif

          }

          // Check for problem
          if (Utils::isnan(bestexp_dll_obs))
          {
            std::stringstream msg;
            msg << "Computation of single-SR loglike for analysis " << adata.analysis_name << " returned NaN, from signal region: " << bestexp_sr_label << endl;
            msg << "Will now print the signal region data for this analysis:" << endl;
            for (size_t SR = 0; SR < adata.size(); ++SR)
            {
              const SignalRegionData& srData = adata[SR];
              msg << srData.sr_label 
                  << ",  n_background = " << srData.n_background 
                  << ",  background_sys = " << srData.background_sys
                  << ",  n_observed = " << srData.n_observed 
                  << ",  n_signal_at_lumi = " << srData.n_signal_at_lumi
                  << ",  n_signal = " << srData.n_signal
                  << ",  signal_sys = " << srData.signal_sys
                  << endl; 
            }
            invalid_point().raise(msg.str());
          }

          // Set this analysis' total obs dLL to that from the best-expected SR (with conversion to more negative dll = more exclusion convention)
          result[adata.analysis_name] = -bestexp_dll_obs;

          #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLike_per_analysis: " << adata.analysis_name << "_" << bestexp_sr_label << "_DeltaLogLike : " << -bestexp_dll_obs << endl;
          #endif
        }

      }

    }


    // Compute the total likelihood for all analyses
    void calc_LHC_LogLike(double& result)
    {
      using namespace Pipes::calc_LHC_LogLike;
      result = 0.0;

      // Read analysis names from the yaml file
      std::vector<str> default_skip_analyses;  // The default is empty lists of analyses to skip
      static const std::vector<str> skip_analyses = runOptions->getValueOrDef<std::vector<str> >(default_skip_analyses, "skip_analyses");

      // If too many events have failed, do the conservative thing and return delta log-likelihood = 0
      if (nFailedEvents > maxFailedEvents)
      {
        #ifdef COLLIDERBIT_DEBUG
          cout << debug_prefix() << "calc_LHC_LogLike: Too many failed events. Will be conservative and return a delta log-likelihood of 0." << endl;
        #endif
        return;
      }

      // Loop over analyses and calculate the total observed dLL
      for (auto const& analysis_loglike_pair : *Dep::LHC_LogLikes)
      {
        const string& analysis_name = analysis_loglike_pair.first;
        const double& analysis_loglike = analysis_loglike_pair.second;

        // If the analysis name is in skip_analyses, don't add its loglike to the total loglike.
        if (std::find(skip_analyses.begin(), skip_analyses.end(), analysis_name) != skip_analyses.end())
        {
          #ifdef COLLIDERBIT_DEBUG
            cout.precision(5);
            cout << debug_prefix() << "calc_LHC_LogLike: Leaving out analysis " << analysis_name << " with LogL = " << analysis_loglike << endl;
          #endif
          continue;
        }

        // Add analysis loglike
        result += analysis_loglike;

        #ifdef COLLIDERBIT_DEBUG
          cout.precision(5);
          cout << debug_prefix() << "calc_LHC_LogLike: Analysis " << analysis_name << " contributes with a LogL = " << analysis_loglike << endl;
        #endif
      }

      #ifdef COLLIDERBIT_DEBUG
        cout << debug_prefix() << "COLLIDERBIT LOGLIKELIHOOD: " << result << endl;
      #endif

    }


    // Store some information about the event generation
    void getLHCEventLoopInfo(map_str_dbl& result)
    {
      using namespace Pipes::getLHCEventLoopInfo;

      // Clear the result map
      result.clear();

      result["did_event_generation"] = double(eventsGenerated);
      result["too_many_failed_events"] = double(nFailedEvents > maxFailedEvents);

      assert(pythiaNames.size() == colliderInfo.size());
      for (auto& name : pythiaNames)
      {
        result["seed_base_" + name] = colliderInfo[name]["seed_base"];
        result["final_event_count_" + name] = colliderInfo[name]["final_event_count"];
      }

    }

  }
}
