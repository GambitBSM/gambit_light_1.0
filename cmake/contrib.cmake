# GAMBIT: Global and Modular BSM Inference Tool
#************************************************
# \file
#
#  CMake configuration script for contributed
#  packages in GAMBIT-light, based on corresponding
#  file for GAMBIT.
#
#************************************************
#
#  Authors (add name and date if you modify):
#
#  \author Antje Putze
#          (antje.putze@lapth.cnrs.fr)
#  \date 2014 Sep, Oct, Nov
#
#  \author Pat Scott
#          (p.scott@imperial.ac.uk)
#  \date 2014 Nov, Dec
#
# \author Tomas Gonzalo
#         (tomas.gonzalo@monash.edu)
# \dae 2019 June, Oct
#
# \author Tomasz Procter
#         (t.procter.1@research.gla.ac.uk)
# \date June 2021
#
#  \author Anders Kvellestad
#          (anders.kvellestad@fys.uio.no)
#  \date 2023 May
#
#************************************************

include(ExternalProject)

# Define the newline strings to use for OSX-safe substitution.
# This can be moved into externals.cmake if ever it is no longer used in this file.
set(nl "___totally_unlikely_to_occur_naturally___")
set(true_nl \"\\n\")

# Define the download command to use for contributed packages
set(DL_CONTRIB "${PROJECT_SOURCE_DIR}/cmake/scripts/safe_dl.sh" "${CMAKE_BINARY_DIR}" "${CMAKE_COMMAND}" "${CMAKE_DOWNLOAD_FLAGS}")

# Define a series of functions and macros to be used for cleaning ditched components and adding nuke and clean targets for contributed codes
macro(get_paths package build_path clean_stamps nuke_stamps)
  set(stamp_path "${CMAKE_BINARY_DIR}/${package}-prefix/src/${package}-stamp/${package}")
  set(${build_path} "${CMAKE_BINARY_DIR}/${package}-prefix/src/${package}-build")
  set(${clean_stamps} ${stamp_path}-configure ${stamp_path}-build ${stamp_path}-install ${stamp_path}-done)
  set(${nuke_stamps} ${stamp_path}-download ${stamp_path}-mkdir ${stamp_path}-patch ${stamp_path}-update)
endmacro()

function(nuke_ditched_contrib_content package dir)
  get_paths(${package} build_path clean-stamps nuke-stamps)
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${build_path}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${dir}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove -f ${clean-stamps} ${nuke-stamps})
endfunction()

function(add_contrib_clean_and_nuke package dir clean)
  get_paths(${package} build_path clean-stamps nuke-stamps)
  add_custom_target(clean-${package} COMMAND ${CMAKE_COMMAND} -E remove -f ${clean-stamps}
                                     COMMAND [ -e ${dir} ] && cd ${dir} && ([ -e makefile ] || [ -e Makefile ] && ${MAKE_SERIAL} ${clean}) || true
                                     COMMAND [ -e ${build_path} ] && cd ${build_path} && ([ -e makefile ] || [ -e Makefile ] && ${MAKE_SERIAL} ${clean}) || true)
  add_dependencies(distclean clean-${package})
  add_custom_target(nuke-${package} COMMAND ${CMAKE_COMMAND} -E remove -f ${nuke-stamps}
                                    COMMAND ${CMAKE_COMMAND} -E remove_directory "${build_path}"
                                    COMMAND ${CMAKE_COMMAND} -E remove_directory "${dir}")
  add_dependencies(nuke-${package} clean-${package})
  add_dependencies(nuke-contrib nuke-${package})
  add_dependencies(nuke-all nuke-${package})
endfunction()

#contrib/preload
set(name "gambit_preload")
set(dir "${CMAKE_BINARY_DIR}/contrib")
add_library(${name} SHARED "${PROJECT_SOURCE_DIR}/contrib/preload/gambit_preload.cpp")
target_include_directories(${name} PRIVATE "${PROJECT_SOURCE_DIR}/cmake/include" "${PROJECT_SOURCE_DIR}/Utils/include")
set_target_properties(${name} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${dir}"
  LIBRARY_OUTPUT_DIRECTORY "${dir}"
  RUNTIME_OUTPUT_DIRECTORY "${dir}"
)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(gambit_preload_LDFLAGS "-L${dir} -lgambit_preload")
else()
  set(gambit_preload_LDFLAGS "-L${dir} -Wl,--no-as-needed -lgambit_preload")
endif()
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${dir}")

#contrib/slhaea
include_directories("${PROJECT_SOURCE_DIR}/contrib/slhaea/include")

#contrib/mkpath
set(mkpath_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/contrib/mkpath/include")
include_directories("${mkpath_INCLUDE_DIR}")
add_gambit_library(mkpath OPTION OBJECT
                          SOURCES ${PROJECT_SOURCE_DIR}/contrib/mkpath/src/mkpath.c
                          HEADERS ${PROJECT_SOURCE_DIR}/contrib/mkpath/include/mkpath/mkpath.h)
set(GAMBIT_BASIC_COMMON_OBJECTS "${GAMBIT_BASIC_COMMON_OBJECTS}" $<TARGET_OBJECTS:mkpath>)

#contrib/yaml-cpp-0.6.2
set(yaml_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/contrib/yaml-cpp-0.6.2/include)
include_directories("${yaml_INCLUDE_DIR}")
add_definitions(-DYAML_CPP_DLL)
add_subdirectory(${PROJECT_SOURCE_DIR}/contrib/yaml-cpp-0.6.2 EXCLUDE_FROM_ALL)
