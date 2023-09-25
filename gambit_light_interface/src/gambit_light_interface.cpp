#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <limits>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

#ifdef HAVE_PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
PYBIND11_MAKE_OPAQUE(std::vector<std::string>);
PYBIND11_MAKE_OPAQUE(std::vector<double>);
PYBIND11_MAKE_OPAQUE(std::map<std::string, double>);
#endif

#include "gambit_light_interface.h"

#define OUTPUT_PREFIX "gambit_light_interface: "

#ifdef HAVE_PYBIND11
PYBIND11_MODULE(gambit_light_interface, m)
{
    m.def("invalid_point", &gambit_light_invalid_point, "report an invalid input point");
    m.def("warning", &gambit_light_warning, "report user warning");
    m.def("error", &gambit_light_error, "report user error");
    pybind11::bind_map<std::map<std::string, double>>(m, "str_dbl_map", pybind11::module_local(false));
    pybind11::bind_vector<std::vector<std::string>>(m, "str_vec", pybind11::module_local(false));
    pybind11::bind_vector<std::vector<double>>(m, "dbl_vec", pybind11::module_local(false));
}
#endif


namespace Gambit
{
    namespace gambit_light_interface
    {

        // Variable used to collect warning messages from user libraries
        static char *str_warning = NULL;

        // Variable to hold the name of the user function 
        // currently being run, used for error messages
        std::string current_user_function_name;


        #ifdef HAVE_PYBIND11
            /// Pointer to the Python interpreter.
            pybind11::scoped_interpreter* python_interpreter = nullptr;
        #endif

        // Enum for the allowed user library languages.
        typedef enum
        {
            #ifdef HAVE_PYBIND11
                LANG_PYTHON,
            #endif
            LANG_FORTRAN,
            LANG_CPP,
            LANG_C
        } t_fcn_language;

        // A struct to hold info about a user loglike.
        typedef struct
        {
            t_fcn_language lang;
            std::string name;
            union 
            {
                void *typeless_ptr;
                t_loglike_fcn_fortran fortran;
                t_loglike_fcn_cpp cpp;
                t_loglike_fcn_c c;
                #ifdef HAVE_PYBIND11
                    t_loglike_fcn_python python;
                #endif
            } fcn;
            std::vector<std::string> outputs;
        } t_loglike_desc;

        // A map between loglike names and the corresponding t_loglike_desc instances.
        // NOTE: init_priority attribute is needed to make sure that the map is initialised
        // when the dlopen calls the so init function. Verified to work on GCC, intel, and clang.
        // std::map<std::string, t_loglike_desc> user_loglikes __attribute__ ((init_priority (128)));
        std::map<std::string, t_loglike_desc> user_loglikes;

        // A struct to hold info about a user prior transformation function
        typedef struct
        {
            t_fcn_language lang;
            std::string name;
            union 
            {
                void *typeless_ptr;
                t_prior_fcn_fortran fortran;
                t_prior_fcn_cpp cpp;
                t_prior_fcn_c c;
                #ifdef HAVE_PYBIND11
                    t_prior_fcn_python python;
                #endif
            } fcn;
            std::vector<std::string> outputs;
        } t_prior_desc;

        // An instance of t_prior_desc to hold info about a single user-supplied prior.
        t_prior_desc user_prior;

    }
}


extern "C"
void gambit_light_invalid_point(const char *invalid_point_msg)
{
    std::string msg = "Invalid point message from " 
                      + Gambit::gambit_light_interface::current_user_function_name + ": " 
                      + std::string(invalid_point_msg) + "\n";
    throw std::runtime_error("[invalid]" + msg);
}


extern "C"
void gambit_light_error(const char *error_msg)
{
    std::string msg = "Error message from "
                      + Gambit::gambit_light_interface::current_user_function_name + ": " 
                      + std::string(error_msg) + "\n";
    throw std::runtime_error("[fatal]" + msg);
}


extern "C"
void gambit_light_warning(const char *warning_msg)
{
    using namespace Gambit::gambit_light_interface;
    if(str_warning)
        free(str_warning);
    str_warning = strdup(warning_msg);
}


// Callback to register the user log-likelihood functions: pass function address.
extern "C"
int gambit_light_register(const char *loglike_name, void *fcn)
{
    using namespace Gambit::gambit_light_interface;
    t_loglike_desc desc;
    desc.fcn.typeless_ptr = fcn;
    user_loglikes.insert({loglike_name, desc});
    return 0;
}


// Callback to register the user log-likelihood functions: pass function address.
extern "C"
int gambit_light_register_prior(void *fcn)
{
    using namespace Gambit::gambit_light_interface;
    user_prior.fcn.typeless_ptr = fcn;
    return 0;
}



namespace Gambit
{
    namespace gambit_light_interface
    {

        // Run a given user loglike function
        double run_user_loglike(const std::string& loglike_name, const std::vector<std::string>& input_names, 
                              const std::vector<double>& input_vals, std::map<std::string,double>& output, 
                              std::vector<std::string>& warnings)
        {
            current_user_function_name = loglike_name;

            double loglike = 0.0;

            // Grab the registered info for this loglike function
            const t_loglike_desc& desc = user_loglikes[loglike_name];

            // We have different ways of calling the loglike function
            // based on the language of the user library

            // C or Fortran library
            if(desc.lang == LANG_FORTRAN || desc.lang == LANG_C)
            {
                const double *iparams = (double*)alloca(sizeof(double)*input_vals.size());
                double *oparams = (double*)alloca(sizeof(double)*desc.outputs.size());

                // Translate the input to a C array.
                iparams = input_vals.data();

                // Call the function from the user library
                if(desc.lang == LANG_FORTRAN) loglike = desc.fcn.fortran(input_vals.size(), iparams, desc.outputs.size(), oparams);
                if(desc.lang == LANG_C) loglike = desc.fcn.c(input_vals.size(), iparams, desc.outputs.size(), oparams);

                // Add outputs to the output map
                int index = 0;
                for (auto& oname : desc.outputs)
                {
                    output[oname] = oparams[index];
                    index++;
                }
            }

            // C++ library
            if(desc.lang == LANG_CPP)
            {
                std::map<std::string,double> new_output;

                // Call the function from the user library.
                // This part can throw anything - this will be handled in GAMBIT.
                loglike = desc.fcn.cpp(input_names, input_vals, new_output);

                // A C++ library can in principle return an output map with
                // any number of elements. We therefore manually extract only
                // the outputs expected by GAMBIT.
                // TODO: Add option to switch this check off, for efficiency.
                for (auto& oname : desc.outputs)
                {
                    try
                    {
                        output[oname] = new_output.at(oname);
                    }
                    catch (const std::out_of_range& e)
                    {
                        throw std::runtime_error(
                            std::string(OUTPUT_PREFIX) + "Cannot find the expected entry '" + oname 
                            + "' in the output map for the loglike '" + loglike_name + "'."
                        );
                    }
                }
            }

            // Python library
            #ifdef HAVE_PYBIND11
                if(desc.lang == LANG_PYTHON)
                {
                    std::map<std::string,double> new_output;

                    // If a Python exception is caught (via pybind11), re-throw it
                    // as a std::runtime_error without the leading "Exception: "
                    // or "RuntimeError: " part of the error message.
                    // TODO: This is silly. Find a better solution.
                    try
                    {
                        // Call the function from the user library
                        loglike = pybind11::cast<double>((*desc.fcn.python)(input_names, input_vals, &new_output));
                    }
                    catch (const pybind11::error_already_set& e)
                    {
                        std::string errmsg(e.what());
                        if (errmsg.substr(0,11) == "Exception: ")
                        {
                            errmsg.erase(0,11);
                        }
                        else if(errmsg.substr(0,14) == "RuntimeError: ")
                        {
                            errmsg.erase(0,14);
                        }

                        throw std::runtime_error(errmsg);
                    }

                    // A Python library can in principle return an output map with
                    // any number of elements. We therefore manually extract only
                    // the outputs expected by GAMBIT.
                    // TODO: Maybe add option to switch this check off, for efficiency?
                    for (auto& oname : desc.outputs)
                    {
                        try
                        {
                            output[oname] = new_output.at(oname);
                        }
                        catch (const std::out_of_range& e)
                        {
                            throw std::runtime_error(
                                std::string(OUTPUT_PREFIX) + "Cannot find the expected entry '" + oname 
                                + "' in the output map for the loglike '" + loglike_name + "'."
                            );
                        }
                    }
                }
            #endif

            // Collect any warnings raised via gambit_light_warning.
            if (str_warning)
            {
                std::string msg(str_warning);
                warnings.push_back("Warning from " + loglike_name + ": " + msg);
                free(str_warning);
                str_warning = nullptr;
            }
            
            current_user_function_name = "";

            return loglike;
        }



        // Run the registered user prior
        void run_user_prior(const std::vector<std::string>& input_names, const std::vector<double>& input_vals, 
                            std::vector<double>& output, std::vector<std::string>& warnings)
        {
            current_user_function_name = "user-supplied prior transform";

            if(user_prior.lang == LANG_FORTRAN || user_prior.lang == LANG_C)
            {
                // Use C arrays as input. We connect these arrays
                // directly to the internal arrays of the 'input_vals'
                // and 'output' vectors. This means the 'output' vector
                // is directly updated by the user prior.
                const double *iparams = input_vals.data();
                double *oparams = output.data();

                if(user_prior.lang == LANG_FORTRAN) user_prior.fcn.fortran(input_vals.size(), iparams, oparams);
                if(user_prior.lang == LANG_C) user_prior.fcn.c(input_vals.size(), iparams, oparams);
            }

            if(user_prior.lang == LANG_CPP)
            {
                // This part can throw anything - this will be handled in GAMBIT.
                user_prior.fcn.cpp(input_names, input_vals, output);
            }

            #ifdef HAVE_PYBIND11
                if(user_prior.lang == LANG_PYTHON)
                {
                    // If a Python exception is caught (via pybind11), re-throw it
                    // as a std::runtime_error without the leading "Exception: "
                    // or "RuntimeError: " part of the error message.
                    // TODO: This is silly. Find a better solution.
                    try
                    {

                        std::cerr << "DEBUG: Will now call a Python prior transform function with the following arguments:" << std::endl;
                        std::cerr << "DEBUG: input_names:" << std::endl;
                        for (const std::string& s : input_names)
                        {
                            std::cerr << "DEBUG:   - " << s << std::endl;
                        }
                        std::cerr << "DEBUG: input_vals:" << std::endl;
                        for (const double& d : input_vals)
                        {
                            std::cerr << "DEBUG:   - " << d << std::endl;
                        }
                        std::cerr << "DEBUG: output (before):" << std::endl;
                        for (const double& d : output)
                        {
                            std::cerr << "DEBUG:   - " << d << std::endl;
                        }

                        // // DEBUG
                        // pybind11::list input_names_list;
                        // for (const std::string& s : input_names)
                        // {
                        //     input_names_list.append(std::string(s));
                        // }

                        (*user_prior.fcn.python)(input_names, input_vals, &output);
                        // int status = pybind11::cast<int>((*user_prior.fcn.python)(input_names, input_vals, &output));
                        // int status = pybind11::cast<int>((*user_prior.fcn.python)(input_names_copy, input_vals, &output));
                        // (*user_prior.fcn.python)(input_names_list, input_vals, &output);

                        std::cerr << "DEBUG: output (after):" << std::endl;
                        for (const double& d : output)
                        {
                            std::cerr << "DEBUG:   - " << d << std::endl;
                        }
                    }
                    catch (const pybind11::error_already_set& e)
                    {
                        std::string errmsg(e.what());

                        std::cerr << "DEBUG: " << errmsg << std::endl;

                        if (errmsg.substr(0,11) == "Exception: ")
                        {
                            errmsg.erase(0,11);
                        }
                        else if(errmsg.substr(0,14) == "RuntimeError: ")
                        {
                            errmsg.erase(0,14);
                        }

                        throw std::runtime_error(errmsg);
                    }
                }
            #endif

            // Collect any warnings raised via gambit_light_warning.
            if (str_warning)
            {
                std::string msg(str_warning);
                warnings.push_back("Warning from " + current_user_function_name + ": " + msg);
                free(str_warning);
                str_warning = nullptr;
            }

            current_user_function_name = "";
        }



        void init_user_lib_C_CXX_Fortran(const std::string &path, const std::string &func_name,
                                         const std::string &lang, const std::string &entry_name,
                                         const std::vector<std::string> &outputs)
        {
            using namespace Gambit::gambit_light_interface;

            bool is_prior = false;
            if (entry_name == "[prior]") is_prior = true;

            // Load the symbol for the registration function from the user library.
            void *handle = dlopen(path.c_str(), RTLD_LAZY);
            if(!handle)
            {
                throw std::runtime_error(std::string(OUTPUT_PREFIX) + "Could not load dynamic library '" + path + "': " + std::string(dlerror()));
            }

            dlerror();

            if (lang == "c" || lang == "fortran")
            {
                std::string symbol_name = func_name;

                char *error;
                void* vptr = dlsym(handle, symbol_name.c_str());
                if ((error = dlerror()) != NULL)
                {
                    throw std::runtime_error(std::string(OUTPUT_PREFIX) + "Could not load function '" + func_name + "' for entry '" + entry_name + "': " + std::string(error));
                }

                // Are we registering a prior transform or a loglike function?
                if (is_prior)
                {
                    if (lang == "c")   user_prior.lang = LANG_C;
                    else if (lang == "fortran")  user_prior.lang = LANG_FORTRAN;
                    user_prior.fcn.typeless_ptr = vptr;
                    user_prior.outputs = outputs;
                    std::cout << OUTPUT_PREFIX << "Registering prior transform function '" << func_name << "'." << std::endl;
                }
                else
                {
                    t_loglike_desc desc;
                    if (lang == "c")   desc.lang = LANG_C;
                    else if (lang == "fortran")  desc.lang = LANG_FORTRAN;
                    desc.fcn.typeless_ptr = vptr;
                    desc.outputs = outputs;
                    user_loglikes.insert({entry_name, desc});
                    std::cout << OUTPUT_PREFIX << "Registering function '" << func_name << "' for the loglike '" << entry_name << "'." << std::endl;
                }
            }
            else if (lang == "c++")
            {
                std::string symbol_name;
                if (is_prior)
                {
                    symbol_name = "gambit_light_register_prior_" + func_name;
                }
                else
                {
                    symbol_name = "gambit_light_register_loglike_" + func_name;                
                }

                char *error;
                void* vptr = dlsym(handle, symbol_name.c_str());
                if ((error = dlerror()) != NULL)
                {
                    throw std::runtime_error(std::string(OUTPUT_PREFIX) + "Could not load function '" + func_name + "' for entry '" + entry_name + "': " + std::string(error));
                }

                // Are we registering a prior transform or a loglike function?
                if (is_prior)
                {
                    t_fcn_prior user_function;
                    *(void**) (&user_function) = vptr;

                    // Call registration function.
                    (*user_function)(gambit_light_register_prior);

                    // Fill in the rest of the function info in the struct 'user_prior'
                    if (lang == "fortran")  user_prior.lang = LANG_FORTRAN;
                    else if (lang == "c")   user_prior.lang = LANG_C;
                    else if (lang == "c++") user_prior.lang = LANG_CPP;
                    user_prior.outputs = outputs;

                    std::cout << OUTPUT_PREFIX << "Registering prior transform function '" << func_name << "'." << std::endl;
                }
                else
                {
                    t_fcn_loglike user_function;
                    *(void**) (&user_function) = vptr;

                    // Call registration function.
                    (*user_function)(entry_name.c_str(), gambit_light_register);

                    std::cout << OUTPUT_PREFIX << "Registering function '" << func_name << "' for the loglike '" << entry_name << "'." << std::endl;

                    // At this point entry_name should be a key in user_loglikes, registered by the user.
                    if(user_loglikes.find(entry_name) != user_loglikes.end())
                    {

                        // Add parameter and output information to user loglike description.
                        t_loglike_desc &desc = user_loglikes[entry_name];

                        if (lang == "fortran")  desc.lang = LANG_FORTRAN;
                        else if (lang == "c")   desc.lang = LANG_C;
                        else if (lang == "c++") desc.lang = LANG_CPP;

                        desc.outputs = outputs;
                    } 
                    else 
                    {
                        throw std::runtime_error(
                            std::string(OUTPUT_PREFIX) + "The loglike '" + entry_name
                            + "' listed in the config file is not recognized. "
                        );
                    }
                }
            }
            else
            {
                throw std::runtime_error(
                    std::string(OUTPUT_PREFIX) + "'" + lang + "' is not "
                    + "a supported language for user libraries."
                );
            }
        }



        #ifdef HAVE_PYBIND11
            void init_user_lib_Python(const std::string &path, const std::string &func_name, const std::string &entry_name, 
                                      const std::vector<std::string> &outputs)
            {
                using namespace Gambit::gambit_light_interface;

                bool is_prior = false;
                if (entry_name == "[prior]") is_prior = true;

                // Bail now if the backend is not present.
                std::ifstream f(path.c_str());
                if(!f.good())
                {
                    throw std::runtime_error(std::string(OUTPUT_PREFIX) + "Could not load Python library; source file not found at " + path);
                }

                if (nullptr == python_interpreter)
                {
                    // Fire up the Python interpreter if it hasn't been started yet.
                    // Create an instance of the interpreter.
                    try
                    {
                        python_interpreter = new pybind11::scoped_interpreter;
                        std::cout << OUTPUT_PREFIX << "Python interpreter successfully started." << std::endl;
                    }
                    catch (const std::runtime_error& e)
                    {
                        std::cout << OUTPUT_PREFIX << "Did not start Python interpreter: " << e.what() << std::endl;
                    }
                }


                // Add the path to the backend to the Python system path.
                pybind11::object sys_path = pybind11::module::import("sys").attr("path");
                pybind11::object sys_path_insert = sys_path.attr("insert");
                pybind11::object sys_path_remove = sys_path.attr("remove");
                const std::string module_path = std::filesystem::path(path).remove_filename();
                const std::string name = std::filesystem::path(path).stem();
                sys_path_insert(0,module_path);

                // Attempt to import the user module.
                pybind11::module user_module;
                try
                {
                    user_module = pybind11::module::import(name.c_str());
                    // Needed if opaque str_dbl_map type defined in another file?
                    // pybind11::module::import("gambit_light_interface");
                }
                catch (const std::exception& e)
                {
                    sys_path_remove(module_path);
                    throw std::runtime_error(
                        std::string(OUTPUT_PREFIX) + "Failed to import Python module '" + name + "'. "
                        + "Python error was: " + std::string(e.what())
                    );
                }

                // Look for the user function.
                pybind11::object user_function;
                try
                {
                    user_function = user_module.attr(func_name.c_str());
                }
                catch (const std::exception& e)
                {
                    sys_path_remove(module_path);
                    throw std::runtime_error(
                        std::string(OUTPUT_PREFIX) + "Failed to load function '" + func_name 
                        + "' from Python module '" + name + "'. Python error was: " + std::string(e.what())
                    );
                }

                // Are we registering a prior transform or a loglike function?
                if (is_prior)
                {
                    user_prior.name = func_name;
                    user_prior.fcn.python = new pybind11::object(user_module.attr(func_name.c_str()));
                    user_prior.lang = LANG_PYTHON;
                    user_prior.outputs = outputs;
                    std::cout << OUTPUT_PREFIX << "Registering prior transform function '" << user_prior.name << "'." << std::endl;
                }
                else
                {
                    t_loglike_desc desc;
                    desc.name = func_name;
                    desc.fcn.python = new pybind11::object(user_module.attr(func_name.c_str()));
                    desc.lang = LANG_PYTHON;
                    desc.outputs = outputs;
                    user_loglikes.insert({entry_name, desc});
                    std::cout << OUTPUT_PREFIX << "Registering function '" << func_name << "' for the loglike '" << entry_name << "'." << std::endl;
                }

                // Remove the path to the backend from the Python system path.
                sys_path_remove(module_path);
            }
        #endif
    }
}

