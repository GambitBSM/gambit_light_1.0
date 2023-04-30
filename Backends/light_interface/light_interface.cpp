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

#include "gambit_light.h"

#ifdef HAVE_PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

extern "C"
int gambit_light_register_python(const char *fcn_name, const char *python_fcn);

PYBIND11_MAKE_OPAQUE(std::map<std::string, double>);
PYBIND11_MODULE(gambit_light, m) {
    m.def("register_loglike", &gambit_light_register_python, "register a python user likelihood function");
    m.def("invalid_point", &gambit_light_invalid_point, "return a value signifying an invalid point");
    m.def("warning", &gambit_light_warning, "report user warning");
    m.def("error", &gambit_light_error, "report user error");
    pybind11::bind_map<std::map<std::string, double>>(m, "str_dbl_map", pybind11::module_local(false));
}
#endif

// registered functions
namespace Gambit {
    namespace Backends {
        namespace light_interface_0_1 {

            // used for error reporting from C and Fortran interfaces
            static char *str_error = NULL;
            static char *str_warning = NULL;

#ifdef HAVE_PYBIND11
            /// Pointer to the Python interpreter
            pybind11::scoped_interpreter* python_interpreter = nullptr;
#endif

            typedef enum {
#ifdef HAVE_PYBIND11
                LANG_PYTHON,
#endif
                LANG_FORTRAN,
                LANG_CPP,
                LANG_C
            } t_user_loglike_fcn_language;

            typedef struct {
                t_user_loglike_fcn_language lang;
                std::string name;
                union {
                    void *typeless_ptr;
                    user_loglike_fcn_fortran fortran;
                    user_loglike_fcn_cpp cpp;
                    user_loglike_fcn_c c;
#ifdef HAVE_PYBIND11
                    user_loglike_fcn_python python;
#endif
                } fcn;
                std::vector<std::string> inputs;
                std::vector<std::string> outputs;
            } t_user_loglike_desc;

            // NOTE: init_priority attribute is needed to make sure that the map is initialized
            // when the dlopen calls the so init function. Verified to work on GCC, intel, and clang
            std::map<std::string, t_user_loglike_desc> user_loglikes __attribute__ ((init_priority (128)));

            double call_user_function(const std::string& fcn_name, const t_user_loglike_desc &desc, 
                                      const std::map<std::string,double>& input, std::map<std::string,double>& output,
                                      std::vector<std::string>& warnings)
            {
                double retval = 0.0;
                
                if(desc.lang == LANG_FORTRAN ||
                   desc.lang == LANG_C) {
                    double *iparams = (double*)alloca(sizeof(double)*desc.inputs.size());
                    double *oparams = (double*)alloca(sizeof(double)*desc.outputs.size());

                    // translate the input to a C array
                    int iparam = 0;
                    for (auto& pname : desc.inputs) {
                        iparams[iparam++] = input.at(pname);
                    }

                    if(desc.lang == LANG_FORTRAN) retval = desc.fcn.fortran(desc.inputs.size(), iparams, desc.outputs.size(), oparams);
                    if(desc.lang == LANG_C) retval = desc.fcn.c(desc.inputs.size(), iparams, desc.outputs.size(), oparams);

                    // translate the input to a C array
                    iparam = 0;
                    for (auto& pname : desc.outputs) {
                        output[pname] = oparams[iparam++];
                    }
                }

                // this part can throw anything - this will be handled in Gambit
                if(desc.lang == LANG_CPP){
                    retval = desc.fcn.cpp(input, output);
                }

#ifdef HAVE_PYBIND11
                if(desc.lang == LANG_PYTHON){
                    // if a python exception is caught (via pybind11), re-throw it
                    // as a std::runtime_error without the leading "Exception: "
                    // part of the error message
                    try{
                        retval = pybind11::cast<double>((*desc.fcn.python)(input, &output));
                    }
                    catch (pybind11::error_already_set& e)
                    {
                        std::string errmsg(e.what());
                        if(errmsg.substr(0,11) == "Exception: "){
                            errmsg.erase(0,11);
                        }
                        throw std::runtime_error(errmsg);
                    }
                }
#endif

                // check if there were any errors
                if (str_error){
                    std::string s(str_error);
                    free(str_error);
                    str_error = nullptr;
                    throw std::runtime_error(s);
                }

                // check if there were any warnings
                if (str_warning){
                    printf("LIGHT INTERFACE WARNING: %s\n", str_warning);
                    std::string s(str_warning);
                    warnings.push_back("Warning from " + fcn_name + ": " + s);
                    free(str_warning);
                    str_warning = nullptr;
                    // throw std::runtime_error(s);
                }
                
                return retval;
            }
        }
    }
}


extern "C"
double gambit_light_invalid_point()
{
    return std::numeric_limits<double>::quiet_NaN();
}

extern "C"
void gambit_light_error(const char *error)
{
    using namespace Gambit::Backends::light_interface_0_1;
    if(str_error)
        free(str_error);
    str_error = strdup(error);
}


extern "C"
void gambit_light_warning(const char *warning)
{
    using namespace Gambit::Backends::light_interface_0_1;
    if(str_warning)
        free(str_warning);
    str_warning = strdup(warning);
}


extern "C"
void run(const std::map<std::string,double>& input, std::map<std::string,double>& output, std::vector<std::string>& warnings)
{
    std::cout << "light_interface: run" << std::endl;

    double loglike;

    // call user functions
    // TODO:
    //  - what to do when there are multiple functions?
    //  - what if there are multiple backend languages? which is first? does it matter
    using namespace Gambit::Backends::light_interface_0_1;
    for (const auto& fn : user_loglikes){
        double user_loglike = call_user_function(fn.first, fn.second, input, output, warnings);
        // Add each separate loglike contribution to the output map
        output[fn.first] = user_loglike;
        loglike += user_loglike;
    }

    // Add the expected "loglike" entry
    output["loglike"] = loglike;
}


#ifdef HAVE_PYBIND11
// callback to register the user log-likelihood functions from Python: pass function name as string
extern "C"
int gambit_light_register_python(const char *fcn_name, const char *python_fcn)
{
    using namespace Gambit::Backends::light_interface_0_1;
    t_user_loglike_desc desc;
    desc.name = std::string(python_fcn);
    user_loglikes.insert({fcn_name, desc});
    printf("light_interface: registering Python function '%s'\n", fcn_name);
    return 0;
}
#endif


// callback to register the user log-likelihood functions: pass function address
extern "C"
int gambit_light_register(const char *fcn_name, void *fcn)
{
    using namespace Gambit::Backends::light_interface_0_1;
    t_user_loglike_desc desc;
    desc.fcn.typeless_ptr = fcn;
    user_loglikes.insert({fcn_name, desc});
    printf("light_interface: registering function '%s'\n", fcn_name);
    return 0;
}


extern "C"
void lightLibrary_C_CXX_Fortran(const std::string &path, const std::string &init_fun,
                                const std::string &lang, const std::string &function_name,
                                std::vector<std::string> &inputs, std::vector<std::string> &outputs)
{
    using namespace Gambit::Backends::light_interface_0_1;

    // load the init symbol from the user library (should be added to the config file)
    void *handle = dlopen(path.c_str(), RTLD_LAZY);
    if(!handle){
        printf("ligth_interface: could not load dynamic library: %s\n", dlerror());
        return;
    }

    dlerror();

    char *error;
    user_init_fcn user_init_function;
    *(void**) (&user_init_function) = dlsym(handle, init_fun.c_str());

    if ((error = dlerror()) != NULL)  {
        printf("light_interface: could not load init function: %s\n", error);
        return;
    }

    // call user init function
    (*user_init_function)(function_name.c_str(), gambit_light_register);

    // at this point function_name should be a key in user_loglikes, registered by the user
    if(user_loglikes.find(function_name) != user_loglikes.end()) {

        // add parameter and output information to user function description
        t_user_loglike_desc &desc = user_loglikes[function_name];

        if (lang == "fortran")  desc.lang = LANG_FORTRAN;
        else if (lang == "c")   desc.lang = LANG_C;
        else if (lang == "c++") desc.lang = LANG_CPP;

        desc.inputs = std::move(inputs);
        desc.outputs = std::move(outputs);
    } else {
        printf("light_interface: expected function '%s' nas not been registered, skipping.\n", function_name.c_str());
    }
}


#ifdef HAVE_PYBIND11
extern "C"
void lightLibrary_Python(const std::string &path, const std::string &init_fun,
                         const std::string &lang, const std::string &function_name,
                         std::vector<std::string> &inputs, std::vector<std::string> &outputs)
{
    using namespace Gambit::Backends::light_interface_0_1;

    // Bail now if the backend is not present.
    std::ifstream f(path.c_str());
    if(!f.good()) {
        printf("Failed loading Python backend; source file not found at %s\n", path.c_str());
        return;
    }

    if (nullptr == python_interpreter) {
        // Fire up the Python interpreter if it hasn't been started yet.

        // Create an instance of the interpreter.
        try{
            python_interpreter = new pybind11::scoped_interpreter;
            printf("Python interpreter successfully started.\n");
        }
        catch (std::runtime_error e){
            printf("Did not start python interpreter: %s\n", e.what());
        }
    }

    // Add the path to the backend to the Python system path
    pybind11::object sys_path = pybind11::module::import("sys").attr("path");
    pybind11::object sys_path_insert = sys_path.attr("insert");
    pybind11::object sys_path_remove = sys_path.attr("remove");
    const std::string module_path = std::filesystem::path(path).remove_filename();
    const std::string name = std::filesystem::path(path).stem();
    sys_path_insert(0,module_path);

    // Attempt to import the user module
    pybind11::module user_module;
    try {
        user_module = pybind11::module::import(name.c_str());
        // needed if opaque str_dbl_map type defined in another file?
        // pybind11::module::import("gambit_light");
    }
    catch (std::exception& e) {
        printf("Failed to import Python module from %s.\n", name.c_str());
        printf("Python error was: %s\n", e.what());
        // Remove the path to the backend from the Python system path
        sys_path_remove(module_path);
        return;
    }

    // call user init function
    pybind11::object user_init_function = user_module.attr(init_fun.c_str());
    user_init_function(function_name.c_str(), "register_loglike");

    // at this point function_name should be a key in user_loglikes, registered by the user
    if(user_loglikes.find(function_name) != user_loglikes.end()) {

        // add parameter and output information to user function description
        t_user_loglike_desc &desc = user_loglikes[function_name];

        desc.fcn.python = new pybind11::object(user_module.attr(desc.name.c_str()));
        desc.lang = LANG_PYTHON;
        desc.inputs = std::move(inputs);
        desc.outputs = std::move(outputs);
    } else {
        printf("light_interface: expected Python function '%s' nas not been registered, skipping.\n", function_name.c_str());
    }

    // Remove the path to the backend from the Python system path
    sys_path_remove(module_path);
}
#endif


// light_interface module init function (currently called ad dlload time)
// TODO: in the dl constructor we can't use C++ objects, as they are not yet initialized
// at this point. For example, std::cout cannot be used, so we use C stdio.
// NOTE: BE_INI_FUNCTION function cannot be used for this, as it's called
// for each sampled point, not only once for the module
#ifdef DEBUG_MAIN

#include "yaml-cpp/yaml.h"

static void light_interface_init() __attribute__((constructor));
void light_interface_init() {

    using namespace Gambit::Backends::light_interface_0_1;
    YAML::Node root;
    std::string user_lib;
    std::string function_name;
    std::string init_fun;
    std::string lang;
    std::vector<std::string> inputs, outputs;

    root = YAML::LoadFile("light_interface.yaml");
    YAML::Node lightRootNode = root["LightInterface"];

    for (std::size_t fi = 0; fi < lightRootNode.size(); fi++) {
        const YAML::Node& lightNode = lightRootNode[fi];

        if (not lightNode["function_name"].IsDefined()) {
            printf("ligth_interface: could not load dynamic library: 'function_name' not specified in config file\n");
            continue;
        }
        function_name = lightNode["function_name"].as<std::string>();

        if (not lightNode["user_lib"].IsDefined()) {
            printf("ligth_interface: could not load dynamic library: 'user_lib' not specified in config file\n");
            continue;
        }
        user_lib = lightNode["user_lib"].as<std::string>();

        if (not lightNode["init_fun"].IsDefined())
            init_fun = "init_like";
        else
            init_fun = lightNode["init_fun"].as<std::string>();

        if (not lightNode["lang"].IsDefined()) {
            printf("ligth_interface: could not load dynamic library: 'lang' not specified in config file\n");
            continue;
        }
        lang = lightNode["lang"].as<std::string>();
        if (lang != "fortran" and
            lang != "c" and
#ifdef HAVE_PYBIND11
            lang != "python" and
#endif
            lang != "c++") {
            printf("ligth_interface: could not load dynamic library: unsupported plugin language '%s'\n", lang.c_str());
            continue;
        }

        if (not lightNode["input"].IsDefined()) {
            printf("ligth_interface: could not load dynamic library: 'input' not specified in config file\n");
            continue;
        }
        const YAML::Node& node_input = lightNode["input"];
        for (std::size_t i = 0; i < node_input.size(); i++) {
            inputs.push_back(node_input[i].as<std::string>());
        }

        if (lightNode["output"].IsDefined()) {
            const YAML::Node& node_output = lightNode["output"];
            for (std::size_t i = 0; i < node_output.size(); i++) {
                outputs.push_back(node_output[i].as<std::string>());
            }
        }

        printf("-- LIGHT CONFIG for function '%s'\n", function_name.c_str());
        printf("user_lib: %s\n", user_lib.c_str());
        printf("init_fun: %s\n", init_fun.c_str());
        printf("lang:     %s\n", lang.c_str());
        printf("-- ----- ------\n");

        if (lang == "c" or
            lang == "c++" or
            lang == "fortran")
            lightLibrary_C_CXX_Fortran(user_lib, init_fun, lang, function_name, inputs, outputs);

#ifdef HAVE_PYBIND11
        if (lang == "python")
            lightLibrary_Python(user_lib, init_fun, lang, function_name, inputs, outputs);
#endif

    }

    // TODO: cleanup in the destructor
    // dlclose(handle);
}
#endif /* DEBUG_MAIN */
