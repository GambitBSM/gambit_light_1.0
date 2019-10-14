"""
Master module for all SpecBit related routines.
"""

from setup import *
from files import *
from cmake_variables import *

"""
BASIC SPECTRA - NO SPECTRUM GENERATION
"""

def write_basic_spectrum(gambit_model_name, model_parameters, spec,
                         add_higgs):
    """
    Writes basic spectrum object wrapper for new model:
    SpecBit/src/SpecBit_<new_model_name>. 
    """
    
    modelSS = gambit_model_name + "SimpleSpec"
    modelclass = gambit_model_name + "Model"
    modelcont = gambit_model_name + "model"
    
    intro_message = (
            "///  Implementation of SpecBit routines for \n"
            "///  " + gambit_model_name + "."
    )

    towrite = blame_gum(intro_message)

    towrite += (
            "#include \"gambit/Elements/gambit_module_headers.hpp\"\n"
            "#include \"gambit/Elements/spectrum.hpp\"\n"
            "#include \"gambit/Utils/stream_overloads.hpp\"\n"
            "#include \"gambit/Utils/util_macros.hpp\"\n"
            "#include \"gambit/SpecBit/SpecBit_rollcall.hpp\"\n"
            "#include \"gambit/SpecBit/SpecBit_helpers.hpp\"\n"
            "#include \"gambit/SpecBit/QedQcdWrapper.hpp\"\n"
            "#include \"gambit/Models/SimpleSpectra/{0}.hpp\"\n"
    ).format(modelSS, gambit_model_name)

    # If there's a Higgs dependency 
    if add_higgs:
    	towrite += (
    		"#include \"gambit/Models/SimpleSpectra/SMHiggsSimpleSpec.hpp\"\n"
    		)
    
          
    towrite += (
            "\n"
            "namespace Gambit\n"
            "{{\n"
            "\n"
            "namespace SpecBit\n"
            "{{\n"
            "using namespace LogTags;\n"
            "\n"
            "/// Get a simple wrapper for Spectrum object.\n"
            "void get_{0}(Spectrum& result)\n"
            "{{\n"
            "namespace myPipe = Pipes::get_{0};\n"
            "const SMInputs& sminputs = *myPipe::Dep::SMINPUTS;\n"
            "\n"
            "// Initialise model object \n"
            "Models::{1} {2};\n\n"
            "// BSM parameters\n"
    ).format(spec, modelclass, modelcont)
    
    
    # Now add each BSM model parameter to spectrum
    for i in range(0, len(model_parameters)):
    
        par = model_parameters[i] 

        # Don't have anything that's an output of spectrum computation as 
        # a scan parameter
        if par.is_output: continue
      
        if not isinstance(par, SpectrumParameter):
            raise GumError(("\n\nModel Parameters at position " + i + 
                            "not passed as instance of class "
                            "SpectrumParameter."))
          
        if not par.sm:
            toadd = ""
            if par.tag == "Pole_Mass":
                toadd = "_Pole_Mass"

            shape = "scalar"
            size = 1

            if model_parameters[i].shape:
                if re.match("v[2-9]", par.shape):
                    shape = "vector"
                    size = par.shape[-1]
                elif re.match("m[2-9]x[2-9]", par.shape):
                    # Assuming all matrices will be square...
                    shape = "matrix"
                    size = par.shape[-1]

            # If it's a scalar shape, just add it one by one
            if shape == "scalar":
                towrite += (
                        "{0}.{1}_{2}{3} = *myPipe::Param.at(\"{4}\");\n"
                ).format(modelcont, gambit_model_name, par.fullname, toadd, par.gb_in)

            # If it's a matrix then do each element individually
            elif shape == "matrix":

                for i in xrange(int(size)):
                    for j in xrange(int(size)):
                        towrite += (
                            "{0}.{1}_{2}{3}[{4}][{5}] = *myPipe::Param.at(\"{6}{7}x{8}\");\n"
                        ).format(modelcont, gambit_model_name, par.fullname, toadd, i, j, par.gb_in, i+1, j+1)

            # Todo : vectors
            else:
                raise GumError("Parameter with shape " + shape + " is not currently supported.")

    if add_higgs:
        towrite += add_simple_sminputs(modelcont)
      
    towrite += (
            "\n"
            "// Create a SubSpectrum object wrapper\n"
            "Models::{0} spec({1});\n\n" 
            "// Retrieve any mass cuts\n"
            "static const Spectrum::mc_info mass_cut = "
            "myPipe::runOptions->getValueOrDef<Spectrum::mc_info>"
            "(Spectrum::mc_info(), \"mass_cut\");\n"
            "static const Spectrum::mr_info mass_ratio_cut = "
            "myPipe::runOptions->getValueOrDef<Spectrum::mr_info>"
            "(Spectrum::mr_info(), \"mass_ratio_cut\");\n\n"
            "// We don't supply a LE subspectrum here; an "
            "SMSimpleSpec will therefore be automatically created "
            "from 'sminputs'\n"
            "result = Spectrum(spec,sminputs,&myPipe::Param,"
            "mass_cut,mass_ratio_cut);\n"
            "}}\n\n"
            "// Declaration: print spectrum out\n"
            "void fill_map_from_{2}_spectrum(std::map<std::string,double>&, "
            "const Spectrum&);\n\n"
            "void get_{2}_spectrum_as_map" 
            "(std::map<std::string,double>& specmap)\n"
            "{{\n"
            "namespace myPipe = Pipes::get_{3}_as_map;\n"
            "const Spectrum& spec(*myPipe::Dep::{3});\n"
            "fill_map_from_{2}_spectrum(specmap, spec);\n"
            "}}\n\n"
            "void fill_map_from_{2}_spectrum"
            "(std::map<std::string, double>& specmap, "
            "const Spectrum& spec)\n"
            "{{\n"
            "/// Use SpectrumContents routines to automate\n"
            "static const SpectrumContents::{2} contents;\n"
            "static const std::vector<SpectrumParameter> "
            "required_parameters = contents.all_parameters();\n\n"
            "for(std::vector<SpectrumParameter>::const_iterator "
            "it = required_parameters.begin(); "
            "it != required_parameters.end(); ++it)\n"
            "{{\n"
            "const Par::Tags        tag   = it->tag();\n"
            "const std::string      name  = it->name();\n"
            "const std::vector<int> shape = it->shape();\n"
            "\n"
            "// Scalar case\n"
            "if(shape.size()==1 and shape[0]==1)\n"
            "{{\n"
            "std::ostringstream label;\n"
            "label << name <<\" \"<< Par::toString.at(tag);\n"
            "specmap[label.str()] = spec.get_HE().get(tag,name);\n"
            "}}\n"
            "// Vector case\n"
            "else if(shape.size()==1 and shape[0]>1)\n"
            "{{\n"
            "for(int i = 1; i<=shape[0]; ++i)\n"
            "{{\n"
            "std::ostringstream label;\n"
            "label << name <<\"_\"<<i<<\" \"<< Par::toString.at(tag);\n"
            "specmap[label.str()] = spec.get_HE().get(tag,name,i);\n"
            "}}\n"
            "}}\n"
            "// Matrix case\n"
            "else if(shape.size()==2)\n"
            "{{\n"
            "for(int i = 1; i<=shape[0]; ++i)\n"
            "{{\n"
            "for(int j = 1; j<=shape[0]; ++j)\n"
            "{{\n"
            "std::ostringstream label;\n"
            "label << name <<\"_(\"<<i<<\",\"<<j<<\") \""
            "<<Par::toString.at(tag);\n"
            "specmap[label.str()] = spec.get_HE().get(tag,name,i,j);\n"
            "}}\n"
            "}}\n"
            "}}\n"
            "// Deal with all other cases\n"
            "else\n"
            "{{\n"
            "// ERROR\n"
            "std::ostringstream errmsg;\n"
            "errmsg << \"Invalid parameter received while converting " 
            "{3} to map of strings!\";\n"
            "errmsg << \"Problematic parameter was: \"<< tag "
            "<<\", \" << name << \", shape=\"<< shape;\n"
            "utils_error().forced_throw(LOCAL_INFO,errmsg.str());\n"
            "}}\n"
            "}}\n"
            "}}\n\n"
            "}}\n\n"
            "}}\n"
    ).format(modelSS, modelcont, gambit_model_name, spec)
            
    return indent(towrite)
    
    
def write_spectrum_header(model_name, add_higgs, is_spheno):
    """
    Writes the header for spectrum object,
    SpecBit/include/gambit/SpecBit/SpecBit_<model>_rollcall.hpp
    """
    
    towrite = blame_gum(("///  Rollcall declarations for routines declared \n"
                         "///  in SpecBit_{0}.cpp.".format(model_name)))
    
    towrite += (
            "#ifndef __SpecBit_{0}_hpp__\n"
            "#define __SpecBit_{0}_hpp__\n"
            "\n"
            "  // Spectrum object\n"
            "  #define CAPABILITY {0}_spectrum\n"
            "  START_CAPABILITY\n"
            "\n"
    ).format(model_name)

    if is_spheno:
        towrite += (
                "// =========================\n"
                "// {0} spectrum (from SARAH-generated SPheno)\n"
                "//\n"
                "#define FUNCTION get_{0}_spectrum_SPheno\n"
                "START_FUNCTION(Spectrum)\n"
                "ALLOW_MODELS({0})\n"
                "DEPENDENCY(SMINPUTS, SMInputs)\n"
                "BACKEND_REQ({0}_spectrum, (libSPheno{0}), int, (Spectrum&, const Finputs&) )\n"
                "BACKEND_OPTION((SARAHSPheno_{0}, {1}), (libSPheno{0}))\n"
                "#undef FUNCTION\n"
                "\"n"
        ).format(model_name, SPHENO_VERSION)
    # If we want to make a simple container spectrum only.
    else:
        towrite += (
            "    // Create simple object from SMInputs & new params.\n"
            "    #define FUNCTION get_{0}_spectrum\n"
            "    START_FUNCTION(Spectrum)\n"
            "    DEPENDENCY(SMINPUTS, SMInputs)\n"
        ).format(model_name)

        # Add a conditional Higgs dependency if necessary
        if add_higgs:
            towrite += (
                    "    ALLOW_MODEL_DEPENDENCE(StandardModel_Higgs, {0})\n"
                    "    MODEL_GROUP(higgs, (StandardModel_Higgs))\n"
                    "    MODEL_GROUP({1}, ({0}))\n"
                    "    ALLOW_MODEL_COMBINATION(higgs, {1})\n"
            ).format(model_name, model_name + "_group")
        else:
            towrite += "    ALLOW_MODELS({0})\n"

        towrite +=(
                "    #undef FUNCTION\n"
                "\n"
        )

    towrite += (
        "    // Map for Spectrum, for printing.\n"
        "    #define FUNCTION get_{0}_spectrum_as_map\n"
        "    START_FUNCTION(map_str_dbl)\n"
        "    DEPENDENCY({0}_spectrum, Spectrum)\n"
        "    #undef FUNCTION\n"
        "\n"
        "  #undef CAPABILITY\n"
        "\n"
    ).format(model_name)

    # HiggsBounds output:
    if is_spheno:
        towrite += (
                "// Generalised Higgs couplings\n"
                "#define CAPABILITY Higgs_Couplings\n"
                "\n"
                "  // From partial widths\n"
                "  #define FUNCTION {0}_higgs_couplings_SPheno\n"
                "  START_FUNCTION(HiggsCouplingsTable)\n"
                "  DEPENDENCY({0}_spectrum, Spectrum)\n"
                "  // SM rates.\n"
                # TODO : numHiggses needed
                "  DEPENDENCY(Reference_SM_Higgs_decay_rates, DecayTable::Entry)\n"
                # "  DEPENDENCY(Reference_SM_other_Higgs_decay_rates, DecayTable::Entry)\n"
                # "  DEPENDENCY(Reference_SM_h0_3_decay_rates, DecayTable::Entry)\n"
                # "  DEPENDENCY(Reference_SM_A0_decay_rates, DecayTable::Entry)\n"
                # "  DEPENDENCY(Reference_SM_A0_2_decay_rates, DecayTable::Entry)\n"
                "  // DecayTable to provide us with the rest of the decays\n"
                "  DEPENDENCY(decay_rates, DecayTable)\n"
                "  BACKEND_REQ({0}_HiggsCouplingsTable, (libSPheno{0}), int, (const Spectrum&, HiggsCouplingsTable&, const Finputs&) )\n"
                "  ALLOW_MODELS({0})\n"
                "  #undef FUNCTION\n"
                "\n"
                "#undef CAPABILITY\n"
                "\n"
        ).format(model_name)
    towrite += "#endif\n"
    return towrite
    
def write_specbit_rollcall(model_name):
    """
    Writes the entry for the SpecBit rollcall header.
    """
    
    towrite = (
            "\n"
            "/// Module function declarations for SpecBit_{0}.cpp\n"
            "#include \"gambit/SpecBit/SpecBit_{0}_rollcall.hpp\"\n"
            "\n"
    ).format(model_name)
    
    return dumb_indent(2, towrite)
    
def add_simple_sminputs(model):
    """
    Adds simple SMInputs definitions to a spectrum object.
    """
      
    towrite = (
            "\n"
            "// quantities needed to fill container spectrum\n"
            "double alpha_em = 1.0 / sminputs.alphainv;\n"
            "double C = alpha_em * pi / (sminputs.GF * pow(2,0.5));\n"
            "double sinW2 = 0.5 - pow( 0.25 - C/pow(sminputs.mZ,2) , 0.5);\n"
            "double cosW2 = 0.5 + pow( 0.25 - C/pow(sminputs.mZ,2) , 0.5);\n"
            "double e = pow( 4*pi*( alpha_em ),0.5);\n"
            "double vev = 1. / sqrt(sqrt(2.)*sminputs.GF);\n"
            "\n"
            "// Gauge couplings\n"
            "{0}.vev = vev;\n"    
            "{0}.g1 = e / sqrt(sinW2);\n"    
            "{0}.g2 = e / sqrt(cosW2);\n"    
            "{0}.g3 = pow( 4*pi*( sminputs.alphaS ),0.5);\n"    
            "\n"
            "// Yukawas\n"
            "double sqrt2v = pow(2.0,0.5)/vev;\n"
            "{0}.Yu[0][0] = sqrt2v * sminputs.mU;\n"
            "{0}.Yu[1][1] = sqrt2v * sminputs.mCmC;\n"
            "{0}.Yu[2][2] = sqrt2v * sminputs.mT;\n"
            "{0}.Ye[0][0] = sqrt2v * sminputs.mE;\n"
            "{0}.Ye[1][1] = sqrt2v * sminputs.mMu;\n"
            "{0}.Ye[2][2] = sqrt2v * sminputs.mTau;\n"
            "{0}.Yd[0][0] = sqrt2v * sminputs.mD;\n"
            "{0}.Yd[1][1] = sqrt2v * sminputs.mS;\n"
            "{0}.Yd[2][2] = sqrt2v * sminputs.mBmB;\n"
    ).format(model)
      
    return towrite
    
"""
SPHENO-SPECTRA
"""

def write_spheno_spectrum_src(model_name):
    """
    Writes SpecBit/src/SpecBit_<MODEL>.cpp
    for a Spectrum object interface to SPheno.
    """

    towrite = ""
    return indent(towrite)