# GAMBIT: Global and Modular BSM Inference Tool
#************************************************
# \file
#
#  CMake configuration scripts for obtaining,
#  configuring, compiling and installing
#  backends.
#
#  To add an entry for a new backend, copy
#  and modify an existing one.  Don't use
#  CMAKE_C_FLAGS, CMAKE_CXX_FLAGS, etc here,
#  as these contain extra flags for building
#  GAMBIT itself that will break backends. Use
#  BACKEND_C_FLAGS
#  BACKEND_CXX_FLAGS
#  BACKEND_Fortran_FLAGS
#  If you need to avoid the optimisation
#  settings passed in those, instead use
#  BACKEND_C_FLAGS_NO_BUILD_OPTIMISATIONS
#  BACKEND_CXX_FLAGS_NO_BUILD_OPTIMISATIONS
#  BACKEND_Fortran_FLAGS_NO_BUILD_OPTIMISATIONS.
#
#************************************************
#
#  Authors (add name and date if you modify):
#
#  \author Antje Putze
#          (antje.putze@lapth.cnrs.fr)
#  \date 2014 Sep, Oct, Nov
#  \date 2015 Sep
#
#  \author Pat Scott
#          (p.scott@imperial.ac.uk)
#  \date 2014 Nov, Dec
#  \date 2015 May, Dec
#  \date For the term of my natural life
#
#  \author Chris Rogan
#          (crogan@cern.ch)
#  \date 2015 May
#
#  \author Anders Kvellestad
#          (anderkve@fys.uio.no)
#  \date 2015 May
#  \date 2023 May
#
#  \author Christoph Weniger
#          (c.weniger@uva.nl)
#  \date 2015 Sep
#
#  \author Tomas Gonzalo
#          (tomas.gonzalo@monash.edu)
#  \date 2016 Apr, Dec
#  \date 2017 Nov
#  \date 2019 June
#  \date 2020 Apr
#
#  \author James McKay
#          (j.mckay14@imperial.ac.uk)
#  \date 2016 Aug
#
#  \author Sanjay Bloor
#          (sanjay.bloor12@imperial.ac.uk)
#  \date 2017 May
#  \date 2018 Sep
#
#  \author Ankit Beniwal
#      (ankit.beniwal@adelaide.edu.au)
#  \date 2016 Aug
#  \date 2017 Jun
#  \date 2018 Aug
#
#  \author Aaron Vincent
#          (aaron.vincent@cparc.ca)
#  \date 2017 Sep, Nov
#
#  \author Marcin Chrzaszcz
#          (mchrzasz@cern.ch)
#  \date 2019 July
#
#  \author Jihyun Bhom
#          (jihyun.bhom@ifj.edu.pl)
#  \date 2019 July

#  \author Janina Renk
#          (janina.renk@fysik.su.se)
#  \data 2018 Jun
#
#  \author Patrick Stöcker
#          (stoecker@physik.rwth-aachen.de)
#  \date 2019 Aug
#  \date 2021 Sep
#
#  \author Will Handley
#          (wh260@cam.ac.uk)
#  \date 2020 Mar
#
#************************************************


# TODO: Add setup for building the light_interface library


# Alternative download command for getting unreleased things from the gambit_internal repository.
# If you don't know what that is, you don't need to tinker with these.
#    DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow --bold ${private_code_warning1}
#             COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold ${private_code_warning2}
#             COMMAND ${CMAKE_COMMAND} -E copy_directory ${loc} ${dir}
