# GAMBIT: Global and Modular BSM Inference Tool
#************************************************
# \file
#
#  Master CMake configuration script for GAMBIT-light,
#  based on the corresponding file for GAMBIT.
#
#  CMakeLists files in this project can refer to
#  the root source directory of the project as
#  ${PROJECT_SOURCE_DIR} and to the root binary
#  directory of the project as ${PROJECT_BINARY_DIR}.
#
#************************************************
#
#  Authors (add name and date if you modify):
#
#  \author Antje Putze
#          (antje.putze@lapth.cnrs.fr)
#  \date 2014 Sep, Oct, Nov
#        2015 Jan, Feb, Apr, Sep
#
#  \author Pat Scott
#          (p.scott@imperial.ac.uk)
#  \date 2014 Nov, Dec
#
#  \author Tomas Gonzalo
#          (t.e.gonzalo@fys.uio.no)
#  \date 2016 Sep
#
#  \author Ben Farmer
#          (b.farmer@imperial.ac.uk)
#  \date 2018 Oct
#
#  \author Anders Kvellestad
#          (anders.kvellestad@fys.uio.no)
#  \date 2021 Jul, Aug
#  \date 2023 May
#
#************************************************

# Define minimum cmake version
cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

# Define minimum compiler versions
set(MIN_GCC_VERSION 5.1)
set(MIN_ICC_VERSION 15.0.2)
set(MIN_CLANG_VERSION 10.0.0)
set(MIN_APPLECLANG_VERSION 13.0.0)

SET(CMAKE_BUILD_TYPE_STRING "Choose the type of build, options are: None Debug Release Release_O3 RelWithDebInfo MinSizeRel.")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  SET(CMAKE_BUILD_TYPE None CACHE STRING "${CMAKE_BUILD_TYPE_STRING}" FORCE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release_O3")
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "${CMAKE_BUILD_TYPE_STRING}" FORCE)
  SET(FORCE_O3 TRUE)
endif()
message("${Yellow}-- Build type is set to ${CMAKE_BUILD_TYPE} ${ColourReset}")

# Set certain policies to NEW
foreach(p
  CMP0003 # CMake 2.6.0
  CMP0012 # CMake 2.8.0
  CMP0022 # CMake 2.8.12
  CMP0025 # CMake 3.0
  CMP0042 # CMake 3.0
  CMP0048 # Cmake 3.0
  CMP0051 # CMake 3.1
  CMP0054 # CMake 3.1
  CMP0060 # CMake 3.16.2
  CMP0063 # CMake 3.3.2
  CMP0068 # CMake 3.9.1
  CMP0074 # CMake 3.12
  )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()

# Set certain policies to OLD
foreach(p
  CMP0148 # CMake 3.27
  CMP0167 # CMake 3.30
  )
  if(POLICY ${p})
    cmake_policy(SET ${p} OLD)
  endif()
endforeach()

# Set the project name, enabling C, C++ and Fortran support
project(gambit C CXX Fortran)

# Some settings needed for GAMBIT-light
set(GAMBIT_LIGHT 1)

set(light_interface_name "gambit_light_interface")
set(light_interface_dir "${PROJECT_SOURCE_DIR}/gambit_light_interface")
include_directories("${light_interface_dir}/include")

set(HAVE_MATHEMATICA 0)
set(EXCLUDE_ROOT 1)
set(EXCLUDE_RESTFRAMES 1)
set(itch "${itch};DarkBit")
set(itch "${itch};ColliderBit")
set(itch "${itch};CosmoBit")
set(itch "${itch};FlavBit")
set(itch "${itch};PrecisionBit")
set(itch "${itch};NeutrinoBit")
set(itch "${itch};ObjectivesBit")
set(itch "${itch};DecayBit")
set(itch "${itch};SpecBit")
set(itch "${itch};ExampleBit_A")
set(itch "${itch};ExampleBit_B")
set(itch "${itch};great")  # Leave out the GreAT scanner, to avoid all the ROOT stuff

# Make sure the user hasn't accidentally set the C++ compiler to the C compiler.
get_filename_component(CXX_COMPILER_NAME ${CMAKE_CXX_COMPILER} NAME)
if (CXX_COMPILER_NAME MATCHES "gcc.*$")
  message(FATAL_ERROR "\nYou have set CMAKE_CXX_COMPILER to gcc.  Did you mean g++?")
elseif (CXX_COMPILER_NAME MATCHES "clang($|-.*)")
  message(FATAL_ERROR "\nYou have set CMAKE_CXX_COMPILER to clang.  Did you mean clang++?")
elseif (CXX_COMPILER_NAME MATCHES "icc.*$")
  message(FATAL_ERROR "\nYou have set CMAKE_CXX_COMPILER to icc.  Did you mean icpc?")
endif()

# Include cmake utility scripts, including colour definitions.
include(cmake/utilities.cmake)

# Enforce minimum versions for the C/C++ compiler
if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS MIN_ICC_VERSION) OR
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS MIN_GCC_VERSION) OR
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS MIN_CLANG_VERSION) OR
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS MIN_APPLECLANG_VERSION))
  message(FATAL_ERROR "${BoldRed}\nGAMBIT requires at least g++ ${MIN_GCC_VERSION} or icpc ${MIN_ICC_VERSION}.  Please install a newer compiler.${ColourReset}")
endif()

# Don't relink all binaries when shared lib changes (programs will be rebuilt anyway if used headers change)
set(CMAKE_LINK_DEPENDS_NO_SHARED 1)

# Include ./cmake in search path for projects
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Add common system library search variables to cmake library search variable, used by find_library
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} $ENV{LIBRARY_PATH})
string(REPLACE ":" ";" CMAKE_LIBRARY_PATH "${CMAKE_LIBRARY_PATH}")

# When building, use the install RPATH already
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Add the automatically determined parts of the RPATH that point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Check if pybind11 is to be ditched
string(REGEX MATCH ";pybind;|;pybind11;|;Pybind;|;Pybind11;" DITCH_PYBIND ";${itch};")

# Check for Python interpreter
# We also need to search for PythonLibs before letting pybind11 look for them,
# otherwise it seems to get it wrong.  Also, we need to add versions of python
# greater than 3.3 manually, for compatibility with CMake 2.8.12.
# If pybind11 is ditched, do not worry about PythonLibs
set(Python_ADDITIONAL_VERSIONS 3.4 3.5 3.6 3.7 3.8 3.9)

find_package(PythonInterp 3)
if(NOT PYTHONINTERP_FOUND)
  message("${BoldYellow}   Python 3 not found.${ColourReset}")
  message(FATAL_ERROR "\nGAMBIT requires Python 3.  \nIf you need to set the path to the Python interpreter manually, "
                        "please use -D PYTHON_EXECUTABLE=path/to/preferred/python.")
endif()
message("${BoldYellow}   Using Python interpreter version ${PYTHON_VERSION_STRING} for build.${ColourReset}")

find_package(PythonLibs 3)

set(HAVE_PYBIND11 0) # Start from assumption that pybind is not present.
if(DITCH_PYBIND)
  message("${BoldCyan} X Excluding pybind11 from GAMBIT configuration. This means GAMBIT won't work with Python libraries.${ColourReset}")
else()  # pybind is not ditched
  if(NOT PYTHONLIBS_FOUND)
    message("${BoldRed}   PythonLibs NOT found, so pybind11 cannot be used. This means GAMBIT won't work with Python libraries.${ColourReset}\n"
            "   If you *do* have the Python libraries installed, you can manually set the following\n"
            "   variables when invoking cmake (also making sure to clean out your build dir):\n"
            "     PYTHON_LIBRARY\n"
            "     PYTHON_INCLUDE_DIR\n"
            "     PYTHON_EXECUTABLE")
    set(itch "${itch};pybind11")
    set(DITCH_PYBIND 1)
  else() # pybind is not ditched and the PythonLibs were found. Can now actually look for pybind.
    set(MIN_pybind11_VERSION "2.13.6")
    set(PREFERRED_pybind11_VERSION "2.13.6")
    set(pybind11_CONTRIB_DIR "${PROJECT_SOURCE_DIR}/contrib/pybind11")
    include_directories("${PYTHON_INCLUDE_DIRS}")
    find_package(pybind11 QUIET)
    if(pybind11_FOUND)
      if(pybind11_VERSION VERSION_LESS MIN_pybind11_VERSION)
        if(EXISTS "${pybind11_CONTRIB_DIR}")
          use_contributed_pybind11()
        else()
          message("${BoldRed}   Found pybind11 ${pybind11_VERSION}. GAMBIT requires at least v${MIN_pybind11_VERSION}.${ColourReset}")
          set(pybind11_FOUND FALSE)
        endif()
      else()
        message("${BoldYellow}   Found pybind11 ${pybind11_VERSION} at ${pybind11_DIR}.${ColourReset}")
        if(NOT pybind11_INCLUDE_DIR)
            include_directories("${PYBIND11_INCLUDE_DIR}")
        else()
            include_directories("${pybind11_INCLUDE_DIR}")
        endif()
      endif()
    endif()
    if(NOT pybind11_FOUND)
      if(EXISTS "${pybind11_CONTRIB_DIR}")
        use_contributed_pybind11()
      else()
        message("${BoldRed}   CMake will now download and install pybind11 v${PREFERRED_pybind11_VERSION}.${ColourReset}")
        execute_process(RESULT_VARIABLE result COMMAND git clone https://github.com/pybind/pybind11.git ${pybind11_CONTRIB_DIR})
        if(${result} STREQUAL "0")
          execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${pybind11_CONTRIB_DIR} git checkout -q v${PREFERRED_pybind11_VERSION})
          use_contributed_pybind11()
        else()
          message("${BoldRed}   Attempt to clone git repository for pybind11 failed.  This may be because you are disconnected from the internet.\n   "
                  "Otherwise, your git installation may be faulty. Errors about missing .so files are usually due to\n   "
                  "your git installation being linked to a buggy version of libcurl.  In that case, try reinstalling libcurl.${ColourReset}")
        endif()
      endif()
    endif()
    set(HAVE_PYBIND11 "${pybind11_FOUND}")
  endif()
endif()

# Check for required Python libraries
foreach(module yaml os re datetime sys getopt shutil itertools)
  gambit_find_python_module(${module} REQUIRED)
endforeach()

# Check for axel
set(CMAKE_DOWNLOAD_FLAGS "NONE")
option(WITH_AXEL "Compile with Axel enabled" OFF)
if(WITH_AXEL)
  find_program(axel_FOUND axel)
  if(axel_FOUND)
    message("${BoldYellow}   Found axel.${ColourReset} Downloads will be as fast as possible.")
    set(CMAKE_DOWNLOAD_FLAGS "WITH_AXEL")
  else()
    message("${Red}   Axel utility not found.  Downloads would be faster if you installed axel.${ColourReset}")
  endif()
else()
  message("${BoldCyan} X Axel is disabled. Please use -DWITH_AXEL=ON to enable fast downloads with Axel.${ColourReset}")
endif()

# # Check for X11
# find_package(X11)
# if(NOT X11_FOUND)
#   message("${BoldRed}   X11 not found. Backends such as CalcHEP that require X11 will be disabled.${ColourReset}")
# endif()

# Do OSX checks
include(cmake/MacOSX.cmake)

# Check if we are using 2019 intel compilers or gcc 6.1 or 6.2 together with pybind11, and forbid the use of C++17 if so.
if(HAVE_PYBIND11)
  if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 18.0.6) OR
     ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.6 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.3))
    set(PERMIT_CXX17_CHECK FALSE)
  else()
    set(PERMIT_CXX17_CHECK TRUE)
  endif()
endif()

# Prevent using C++17 if using gnu version 7.2
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.1 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.3)
    set(PERMIT_CXX17_CHECK FALSE)
endif()

# Check for C++11, C++14 and C++17 support.
include(CheckCXXCompilerFlag)
if(PERMIT_CXX17_CHECK)
  CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
endif()
if(COMPILER_SUPPORTS_CXX17)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  set(GAMBIT_SUPPORTS_CXX17 TRUE)
else()
  if(PERMIT_CXX17_CHECK)
    CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1z)
  endif()
  CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
  if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    set(GAMBIT_SUPPORTS_CXX14 TRUE)
  else()
    CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX1y)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    if(COMPILER_SUPPORTS_CXX11)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
      set(GAMBIT_SUPPORTS_CXX11 TRUE)
    else()
      CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
      if(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        set(GAMBIT_SUPPORTS_CXX0X TRUE)
      else()
        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
      endif()
    endif()
  endif()
endif()

# Check for C99, C11 and C18 support
include(CheckCCompilerFlag)
CHECK_C_COMPILER_FLAG("-std=c18" COMPILER_SUPPORTS_C18)
CHECK_C_COMPILER_FLAG("-std=c11" COMPILER_SUPPORTS_C11)
CHECK_C_COMPILER_FLAG("-std=c99" COMPILER_SUPPORTS_C99)

# Add -fPIC for 64 bit systems
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -fPIC")
endif()

# Add some Fortran compiler flags
if("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -ffree-line-length-none -ffixed-line-length-none -cpp")
elseif("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Intel")
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -extend-source -fpp")
endif()

# Set output paths
set(mylibdir ${PROJECT_SOURCE_DIR}/lib)
set(mybindir ${PROJECT_SOURCE_DIR})
# First for the generic no-config case (e.g. with mingw)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${mybindir} )
set(GAMBIT_RUN_DIR ${mybindir} )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${mylibdir} )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${mylibdir} )
# Second, for multi-config builds (e.g. msvc)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${mybindir} )
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${mylibdir} )
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${mylibdir} )
endforeach()

# Check for Boost
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.48)
if(Boost_FOUND)
  include_directories("${Boost_INCLUDE_DIR}")
else()
  message(FATAL_ERROR "GAMBIT requires Boost v1.48 or greater.\nPlease install a suitable version of Boost and rerun cmake.")
endif()

# Fix Boost < 1.55 for Clang
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  if(${CMAKE_VERSION} VERSION_LESS 3.12.0)
    add_definitions(-DBOOST_PP_VARIADICS=1)
  else()
    add_compile_definitions(BOOST_PP_VARIADICS=1)
  endif()
endif()

# Check for Eigen
find_package(Eigen3 3.1.0)
if(EIGEN3_FOUND)
  include_directories("${EIGEN3_INCLUDE_DIR}")
  message("-- Eigen version: ${EIGEN3_VERSION}")
else()
  message("${BoldRed}   Eigen v3.1.0 or greater not found.${ColourReset}")
  message(FATAL_ERROR "\nGAMBIT requires Eigen v3.1.0 or greater. You can specify the path to Eigen3 using the cmake flag -D EIGEN3_INCLUDE_DIR=/path/to/eigen.")
endif()

# Check for OpenMP
find_package(OpenMP QUIET)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  find_program(BREW NAMES brew)
  # This block modified from https://github.com/CLIUtils/cmake/blob/master/PatchOpenMPApple.cmake
  if(BREW)
    execute_process(COMMAND ${BREW} ls libomp RESULT_VARIABLE BREW_RESULT_CODE OUTPUT_QUIET ERROR_QUIET)
    if(BREW_RESULT_CODE)
      if(NOT OPENMP_FOUND)
        message(FATAL_ERROR "You are using the Apple Clang compiler, and have HomeBrew installed, but have not installed OpenMP. Please run \"brew install libomp\"")
      endif()
    else()
      execute_process(COMMAND ${BREW} --prefix libomp OUTPUT_VARIABLE BREW_LIBOMP_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
      if(NOT OPENMP_FOUND)
        set(OpenMP_CXX_FLAGS "-Xclang -fopenmp -I${BREW_LIBOMP_PREFIX}/include")
        set(OpenMP_C_FLAGS "-Xclang -fopenmp -I${BREW_LIBOMP_PREFIX}/include")
      endif()
      set(OpenMP_CXX_LIB_NAMES "omp")
      set(OpenMP_C_LIB_NAMES "omp")
      set(OpenMP_omp_LIBRARY "${BREW_LIBOMP_PREFIX}/lib/libomp.dylib")
      include_directories("${BREW_LIBOMP_PREFIX}/include")
      message(STATUS "Using Homebrew libomp from ${BREW_LIBOMP_PREFIX}")
      set(FOUND_BREW_OPENMP True)
    endif()
  endif()
endif()
if (OPENMP_FOUND OR FOUND_BREW_OPENMP)
  message("-- OpenMP found successfully")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  if (NOT DEFINED OpenMP_Fortran_FLAGS)
    set(OpenMP_Fortran_FLAGS "-fopenmp")
  endif()
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${OpenMP_Fortran_FLAGS}")
else()
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    if(NOT FOUND_BREW_OPENMP)
      message(FATAL_ERROR "\nOpenMP libraries not found. You are using the Apple Clang compiler. The easiest way to get OpenMP support for Apple Clang is to install it via HomeBrew. If you want to install it some other way, e.g. via MacPorts, you need to manually set -DOpenMP_CXX_FLAGS -DOpenMP_CXX_LIB_NAMES -DOpenMP_C_FLAGS -DOpenMP_C_LIB_NAMES -DOpenMP_omp_LIBRARY when invoking cmake.")
    endif()
  else()
    message(FATAL_ERROR "\nOpenMP libraries not found.  Your compiler installation seems to be broken.")
  endif()
endif()

# Check for DL libraries
include(cmake/FindLibDL.cmake)

# Add compiler warning flags
include(cmake/warnings.cmake)

# Construct the full set of compiler flags to be used for external projects
set(BACKEND_C_FLAGS_NO_BUILD_OPTIMISATIONS "${CMAKE_C_FLAGS}")
set(BACKEND_CXX_FLAGS_NO_BUILD_OPTIMISATIONS "${CMAKE_CXX_FLAGS}")
set(BACKEND_Fortran_FLAGS_NO_BUILD_OPTIMISATIONS "${CMAKE_Fortran_FLAGS}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(BACKEND_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
  set(BACKEND_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
  set(BACKEND_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${CMAKE_Fortran_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  # Unless invoked with FORCE_O3, drop down to -O2 optimisation for more reasonable compile time.
  if (NOT DEFINED FORCE_O3)
    string(REGEX REPLACE "(-O3)" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REGEX REPLACE "(-O3)" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REGEX REPLACE "(-O3)" "-O2" CMAKE_Fortran_FLAGS_RELEASE "${CMAKE_Fortran_FLAGS_RELEASE}")
  endif()
  set(BACKEND_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
  set(BACKEND_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
  set(BACKEND_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${CMAKE_Fortran_FLAGS_RELEASE}")
  # Never send the -O3 from cmake's release build config onwards to backends, as some are touchy.
  string(REGEX REPLACE "(-O3)" "-O2" BACKEND_C_FLAGS "${BACKEND_C_FLAGS}")
  string(REGEX REPLACE "(-O3)" "-O2" BACKEND_CXX_FLAGS "${BACKEND_CXX_FLAGS}")
  string(REGEX REPLACE "(-O3)" "-O2" BACKEND_Fortran_FLAGS "${BACKEND_Fortran_FLAGS}")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(BACKEND_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
  set(BACKEND_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  set(BACKEND_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${CMAKE_Fortran_FLAGS_RELWITHDEBINFO}")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  set(BACKEND_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_MINSIZEREL}")
  set(BACKEND_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_MINSIZEREL}")
  set(BACKEND_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${CMAKE_Fortran_FLAGS_MINSIZEREL}")
else()
  set(BACKEND_C_FLAGS "${CMAKE_C_FLAGS}")
  set(BACKEND_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(BACKEND_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
endif()
set(BACKEND_C18_FLAGS "${BACKEND_C_FLAGS} -std=c18")
set(BACKEND_C11_FLAGS "${BACKEND_C_FLAGS} -std=c11")
set(BACKEND_C99_FLAGS "${BACKEND_C_FLAGS} -std=c99")
set(BACKEND_GNU99_FLAGS "${BACKEND_C_FLAGS} -std=gnu99")

# Should we use pragmas to suppress common compiler warnings from external libraries?
option(SUPPRESS_LIBRARY_WARNINGS "Suppress common compiler warnings due to external libraries" ON)

# Allow global symbols in the GAMBIT executable to resolve references in dynamically loaded libraries
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")

# Check for optional packages and disable sections of GAMBIT accordingly
include(cmake/optional.cmake)

# Look for the latest tag and use it to set the version number.  If there is no such tag, use the version_info.cmake file.
find_package(Git)
set(FOUND_GAMBIT_VERSION_FROM_GIT_TAG FALSE)
if(GIT_FOUND)
  get_version_from_git(GAMBIT_VERSION_MAJOR GAMBIT_VERSION_MINOR GAMBIT_VERSION_REVISION
                       GAMBIT_VERSION_PATCH GAMBIT_VERSION_FULL)
  if (GAMBIT_VERSION_MAJOR OR GAMBIT_VERSION_MINOR)
    message("${BoldYellow}   GAMBIT version detected from git tag: ${GAMBIT_VERSION_FULL}${ColourReset}")
    set(FOUND_GAMBIT_VERSION_FROM_GIT_TAG TRUE)
  endif()
endif()
if(NOT GIT_FOUND OR NOT FOUND_GAMBIT_VERSION_FROM_GIT_TAG)
  message("${BoldYellow}   GAMBIT version not detected via git.  Reverting to cmake/version_info.cmake.${ColourReset}")
  include(cmake/version_info.cmake)
endif()

# Add doxygen build as an external project
add_custom_target(docs WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND doxygen doc/doxygen.conf)

# # Set the modules to include in the compile
# set(GAMBIT_BITS "SpecBit;LightBit")

# Include contributed packages
include(cmake/contrib.cmake)

# Reprocess the ditch set into a comma-separated list
string (REPLACE ";" "," itch_with_commas "${itch}")

# Create the scratch directory if it isn't there already
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/scratch")
  message("${Yellow}-- Creating scratch directory${ColourReset}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory scratch WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory build_time WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/scratch)
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory run_time WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/scratch)
  message("${Yellow}-- Creating scratch directory - done.${ColourReset}")
endif()

# Generate the ScannerBit compilation files
if(EXISTS "${PROJECT_SOURCE_DIR}/ScannerBit/")
  message("${Yellow}-- Updating GAMBIT scanner cmake and related files${ColourReset}")
  set(scanner_harvester ${PROJECT_SOURCE_DIR}/ScannerBit/scripts/scanner+_harvester.py ${PROJECT_BINARY_DIR} -x __not_a_real_name__,${itch_with_commas})
  execute_process(RESULT_VARIABLE result COMMAND ${PYTHON_EXECUTABLE} ${scanner_harvester} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
  check_result(${result} ${scanner_harvester})
  message("${Yellow}-- Updating GAMBIT scanner cmake and related files - done.${ColourReset}")
endif()

# Generate the cmake_variables.hpp file
include(cmake/preprocessor.cmake)

# Generate particle_database.cpp from particle_database.yaml.
if(EXISTS "${PROJECT_SOURCE_DIR}/Models/")
  set(particle_harvester ${PROJECT_SOURCE_DIR}/Models/scripts/particle_harvester.py ${PROJECT_BINARY_DIR} -x __not_a_real_name__,${itch_with_commas})
  execute_process(COMMAND ${PYTHON_EXECUTABLE} ${particle_harvester} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
  message("${Yellow}-- Generated particle_database.cpp from particle_database.yaml.${ColourReset}")
endif()

# Add the main gambit sources and identify which modules are to be included.
include(cmake/gambit.cmake)

# Identify the different harvester scripts
set(MODULE_HARVESTER ${PROJECT_SOURCE_DIR}/Elements/scripts/module_harvester.py)
set(BACKEND_HARVESTER ${PROJECT_SOURCE_DIR}/Backends/scripts/backend_harvester.py)
set(MODEL_HARVESTER ${PROJECT_SOURCE_DIR}/Models/scripts/model_harvester.py)
set(PRINTER_HARVESTER ${PROJECT_SOURCE_DIR}/Printers/scripts/printer_harvester.py)
set(HARVEST_TOOLS ${PROJECT_SOURCE_DIR}/Utils/scripts/harvesting_tools.py)

# Create module_rollcall.hpp, module_types_rollcall.hpp, module_functor_types.hpp, models_rollcall.hpp, model_types_rollcall.hpp,
# backend_rollcall.hpp, backend_types_rollcall.hpp, backend_functor_types.hpp, printer_rollcall.hpp, particle_database.cpp
file(GLOB MODULE_HARVESTER_FILES   "${PROJECT_SOURCE_DIR}/*Bit*/include/gambit/*Bit*/*_rollcall.hpp"
                                   "${PROJECT_SOURCE_DIR}/*Bit*/include/gambit/*Bit*/*_types.hpp")
file(GLOB BACKEND_HARVESTER_FILES  "${PROJECT_SOURCE_DIR}/Backends/include/gambit/Backends/frontends/*.hpp"
                                   "${PROJECT_SOURCE_DIR}/Backends/CMakeLists.txt")
file(GLOB MODEL_HARVESTER_FILES    "${PROJECT_SOURCE_DIR}/Models/include/gambit/Models/models/*.hpp"
                                   "${PROJECT_SOURCE_DIR}/Models/CMakeLists.txt")
file(GLOB PRINTER_HARVESTER_FILES  "${PROJECT_SOURCE_DIR}/Printers/include/gambit/Printers/printers/*.hpp"
                                   "${PROJECT_SOURCE_DIR}/Printers/CMakeLists.txt")
string (REPLACE "//" "/" MODULE_HARVESTER_FILES   "${MODULE_HARVESTER_FILES}")
string (REPLACE "//" "/" BACKEND_HARVESTER_FILES  "${BACKEND_HARVESTER_FILES}")
string (REPLACE "//" "/" MODEL_HARVESTER_FILES    "${MODEL_HARVESTER_FILES}")   # (GLOB creates erroneous double slashes)
string (REPLACE "//" "/" PRINTER_HARVESTER_FILES  "${PRINTER_HARVESTER_FILES}")
list(REMOVE_ITEM MODULE_HARVESTER_FILES "${PROJECT_SOURCE_DIR}/ScannerBit//include//gambit//ScannerBit//priors_rollcall.hpp"
                                        "${PROJECT_SOURCE_DIR}/ScannerBit//include//gambit//ScannerBit//test_function_rollcall.hpp")
list(APPEND MODULE_HARVESTER_FILES "${PROJECT_SOURCE_DIR}/config/resolution_type_equivalency_classes.yaml")
set(MODULE_HARVESTER_FILES ${MODULE_HARVESTER_FILES} ${BACKEND_HARVESTER_FILES})
remove_build_files(models_harvested backends_harvested modules_harvested printers_harvested)
if(EXISTS "${PROJECT_SOURCE_DIR}/Elements/")
  add_gambit_custom(module_harvest modules_harvested MODULE_HARVESTER MODULE_HARVESTER_FILES ${itch_with_commas})
endif()
if(EXISTS "${PROJECT_SOURCE_DIR}/Backends/")
  add_gambit_custom(backend_harvest backends_harvested BACKEND_HARVESTER BACKEND_HARVESTER_FILES ${itch_with_commas})
  add_dependencies(module_harvest backend_harvest)
endif()
if(EXISTS "${PROJECT_SOURCE_DIR}/Models/")
 add_gambit_custom(model_harvest models_harvested MODEL_HARVESTER MODEL_HARVESTER_FILES)
endif()
if(EXISTS "${PROJECT_SOURCE_DIR}/Printers/")
  add_gambit_custom(printer_harvest printers_harvested PRINTER_HARVESTER PRINTER_HARVESTER_FILES ${itch_with_commas})
  add_dependencies(printer_harvest module_harvest)
else()
  add_definitions(-DNO_PRINTERS)
endif()

# Generate the CMakeLists.txt files for GAMBIT modules, Backends, Models and Printers)
message("${Yellow}-- Updating GAMBIT module, model, backend, and printer CMake files.${ColourReset}")
set(update_cmakelists ${PROJECT_SOURCE_DIR}/cmake/scripts/update_cmakelists.py -x __not_a_real_name__,${itch_with_commas})
execute_process(RESULT_VARIABLE result COMMAND ${PYTHON_EXECUTABLE} ${update_cmakelists} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
check_result(${result} ${update_cmakelists})
message("${Yellow}-- Updating GAMBIT module, backend, and printer CMake files - done.${ColourReset}")

# Add backends and scanners
include(cmake/externals.cmake)

# Add GAMBIT subdirectories.
add_subdirectory(Logs)
add_subdirectory(Utils)
add_subdirectory_if_present(Core)
add_subdirectory_if_present(Models)
add_subdirectory_if_present(Backends)
add_subdirectory_if_present(Elements)
add_subdirectory_if_present(Printers)

# Lists of different GAMBIT object files to link
set(GAMBIT_BASIC_COMMON_OBJECTS "${GAMBIT_BASIC_COMMON_OBJECTS}" $<TARGET_OBJECTS:Logs> $<TARGET_OBJECTS:Utils>)
set(GAMBIT_ALL_COMMON_OBJECTS "${GAMBIT_BASIC_COMMON_OBJECTS}" $<TARGET_OBJECTS:Models> $<TARGET_OBJECTS:Backends> $<TARGET_OBJECTS:Elements>)

# Set the modules to include in the compile
set(GAMBIT_BITS "ScannerBit;LightBit")

# Set compilation targets for GAMBIT modules
foreach(bit ${GAMBIT_BITS})
  add_subdirectory(${bit})
  set(GAMBIT_BIT_OBJECTS ${GAMBIT_BIT_OBJECTS} "$<TARGET_OBJECTS:${bit}>")
  # Add dependencies that need to be built before any bit
  foreach(dep ${MODULE_DEPENDENCIES})
    add_dependencies(${bit} ${dep})
  endforeach()
endforeach()

# Add the executables
include(cmake/executables.cmake)

# Finish setting the link commands and rpath variables for ScannerBit
if(EXISTS "${PROJECT_SOURCE_DIR}/ScannerBit/")
  include(${PROJECT_BINARY_DIR}/linkedout.cmake)
endif()

# Create a list of Gambit bits to be used in the diagnostic system of Gambit.
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Utils/scripts/find_all_gambit_bits.py --source-dir ${PROJECT_SOURCE_DIR} --output-file=${PROJECT_SOURCE_DIR}/config/gambit_bits.yaml)


# Add the gambit_light_interface shared library 
add_library(${light_interface_name} SHARED "${light_interface_dir}/src/gambit_light_interface.cpp")
set_target_properties(${light_interface_name} PROPERTIES
  PREFIX ""
  SUFFIX ".so"  # This library will be loaded as a Python module, so it must be .so also on MacOS
  LIBRARY_OUTPUT_DIRECTORY "${light_interface_dir}/lib"
  ARCHIVE_OUTPUT_DIRECTORY "${light_interface_dir}/lib"
)
target_include_directories(${light_interface_name} PRIVATE "${light_interface_dir}/include" "${PROJECT_SOURCE_DIR}/cmake/include" "${yaml_INCLUDE_DIR}")
target_link_libraries(${light_interface_name} PRIVATE yaml-cpp)
if(HAVE_PYBIND11)
  target_include_directories(${light_interface_name} PRIVATE "${PYBIND11_INCLUDE_DIR}" "${PYTHON_INCLUDE_DIRS}")
  target_compile_options(${light_interface_name} PRIVATE "-DHAVE_PYBIND11")
  target_link_libraries(${light_interface_name} PRIVATE "${PYTHON_LIBRARIES}")
endif()
# Need to switch symbol visibility back to default since we're using the 
# same CMAKE_CXX_FLAGS as for the GAMBIT source files (see above).
target_compile_options(${light_interface_name} PRIVATE "-fvisibility=default") 

# Link the main GAMBIT executable to the gambit_light_interface.so library and update GAMBIT's RPATH.
# We do this to ensure that GAMBIT symbols are available (via gambit_light_interface.so) to the user 
# libraries that gambit_light_interface.so load at runtime (dlopen), so that e.g. functions like 
# "gambit_light_warning" can be called from the user library.
target_link_libraries(${PROJECT_NAME} PRIVATE ${light_interface_name})
set_target_properties(${PROJECT_NAME} PROPERTIES
  INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${light_interface_dir}/lib"
)

# Add include directory
# include_directories("${light_interface_dir}/include")

# Make sure the gambit_light shared library is built before the the main gambit executable
add_dependencies(${PROJECT_NAME} ${light_interface_name})  


