# GAMBIT: Global and Modular BSM Inference Tool
#************************************************
# \file
#
#  CMake CMakeLists.txt file for Elements
#
#************************************************
#
#  Authors (add name and date if you modify):
#
#  \author Pat Scott
#          (p.scott@imperial.ac.uk)
#  \date 2015 Apr
#
#  \author Tomas Gonzalo
#          (tomas.gonzalo@monash.edu)
#  \date 2020 Dec
#
#************************************************

set(source_files src/equivalency_singleton.cpp
                 src/functors.cpp
                 src/ini_functions.cpp
                 src/ini_catch.cpp
                 src/terminator.cpp
                 src/translator.cpp
                 src/type_equivalency.cpp
)

set(header_files include/gambit/Elements/equivalency_singleton.hpp
                 include/gambit/Elements/functors.hpp
                 include/gambit/Elements/functor_definitions.hpp
                 include/gambit/Elements/ini_functions.hpp
                 include/gambit/Elements/ini_catch.hpp
                 include/gambit/Elements/module_macros_common.hpp
                 include/gambit/Elements/module_macros_incore.hpp
                 include/gambit/Elements/module_macros_incore_defs.hpp
                 include/gambit/Elements/module_macros_inmodule.hpp
                 include/gambit/Elements/module_macros_inmodule_defs.hpp
                 include/gambit/Elements/safety_bucket.hpp
                 include/gambit/Elements/shared_types.hpp
                 include/gambit/Elements/terminator.hpp
                 include/gambit/Elements/translator.hpp
                 include/gambit/Elements/type_equivalency.hpp
                 include/gambit/Elements/types_rollcall.hpp
)

add_gambit_library(Elements OPTION OBJECT SOURCES ${source_files} HEADERS ${header_files})
