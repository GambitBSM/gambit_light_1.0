# GAMBIT: Global and Modular BSM Inference Tool
#************************************************
# \file
#
#  CMake configuration script for final executables
#  of GAMBIT-light, based on the corresponding file
#  for GAMBIT.
#
#************************************************
#
#  Authors (add name and date if you modify):
#
#  \author Antje Putze
#          (antje.putze@lapth.cnrs.fr)
#  \date 2014 Sep, Oct, Nov
#        2015 Feb
#
#  \author Pat Scott
#          (p.scott@imperial.ac.uk)
#  \date 2014 Nov, Dec
#
#  \author Christopher Chang
#          (christopher.chang@uqconnect.edu.au)
#  \date 2021 Feb
#
#  \author Tomas Gonzalo
#          (gonzalo@physik.rwth-aachen.de)
#  \date 2021 Mar
#
#  \author Anders Kvellestad
#          (anders.kvellestad@fys.uio.no)
#  \date 2023 May
#
#************************************************

# Add the module standalones
add_custom_target(standalones)
include(cmake/standalones.cmake)

# Add the main GAMBIT executable
if(EXISTS "${PROJECT_SOURCE_DIR}/Core/")
  add_gambit_executable(${PROJECT_NAME} "${gambit_XTRA}"
                        SOURCES ${PROJECT_SOURCE_DIR}/Core/src/gambit.cpp
                                ${GAMBIT_ALL_COMMON_OBJECTS}
                                ${GAMBIT_BIT_OBJECTS}
                                $<TARGET_OBJECTS:Core>
                                $<TARGET_OBJECTS:Printers>
  )
  set_target_properties(gambit PROPERTIES EXCLUDE_FROM_ALL 0)

  # EXPERIMENTAL: Linking against Electric Fence for heap corruption debugging
  #target_link_libraries(gambit PUBLIC efence) # just segfaults. Be good if it could be made to work though.
endif()

# Add C++ hdf5 combine tool, if we have HDF5 libraries
# There are a lot of annoying peripheral dependencies on GAMBIT things here, would be good to try and decouple things better
if(HDF5_FOUND)
  if(EXISTS "${PROJECT_SOURCE_DIR}/Printers/")
    if(EXISTS "${PROJECT_SOURCE_DIR}/Utils/")
       add_gambit_executable(hdf5combine ${HDF5_LIBRARIES}
                        SOURCES ${PROJECT_SOURCE_DIR}/Printers/standalone/manual_hdf5_combine.cpp
                                $<TARGET_OBJECTS:Printers>
                                ${GAMBIT_BASIC_COMMON_OBJECTS}
                                )
       set_target_properties(hdf5combine PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/Printers/bin")
    endif()
  endif()
endif()


