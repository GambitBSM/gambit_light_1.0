# GAMBIT: Global and Modular BSM Inference Tool  
#************************************************
# \file                                          
#                                                
#  Cmake configuration scripts for obtaining,
#  configuring, compiling and installing 
#  external scanners.
#  
#  Note that this is not necessarily the canonical
#  way to manage the compilation of all scanners,
#  and GAMBIT support for scanner compilation is
#  minimal, even with this method -- so please
#  contact the authors of the respective codes
#  if they won't compile!  
#    
#************************************************
#                                                
#  Authors (add name and date if you modify):
#
#  \author Pat Scott
#          (p.scott@imperial.ac.uk)              
#  \date 2014 Nov, Dec
#  \date 2015 May  
#
#  \author Antje Putze (putze@lapth.cnrs.fr)
#  \date 2016 Jan
#
#************************************************


# Diver
set(loc "${GAMBIT_INTERNAL}/Diver") #FIXME delete on release
set(name "diver")
set(ver "1.0.0")
set(lib "libdiver")
set(dl "null")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(diverSO_LINK_FLAGS "${CMAKE_Fortran_MPI_SO_LINK_FLAGS} -fopenmp")
if(MPI_Fortran_FOUND)
  set(diverFFLAGS "${GAMBIT_Fortran_FLAGS_PLUS_MPI}")
else()
  set(diverFFLAGS "${GAMBIT_Fortran_FLAGS}")
endif()
ExternalProject_Add(${name}_${ver}
  DOWNLOAD_DIR ${scanner_download}
  DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow --bold ${private_code_warning1}
           COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold ${private_code_warning2}
           COMMAND ${CMAKE_COMMAND} -E copy_directory ${loc} ${dir}
  SOURCE_DIR ${dir}
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FF=${CMAKE_Fortran_COMPILER} MODULE=${FMODULE} FOPT=${diverFFLAGS} SO_LINK_FLAGS=${diverSO_LINK_FLAGS} 
  INSTALL_COMMAND "" 
)
add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
set_as_default_version("scanner" ${name} ${ver})


# MultiNest
set(loc "${GAMBIT_INTERNAL}/MultiNest_v3.9")
set(name "multinest")
set(ver "3.9")
set(lib "libnest3")
set(dl "null")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(mnSO_LINK "${CMAKE_Fortran_COMPILER} -shared ${OpenMP_Fortran_FLAGS} ${CMAKE_Fortran_MPI_SO_LINK_FLAGS}")
if (NOT LAPACK_STATIC)
  set(mnLAPACK "${LAPACK_LINKLIBS}")
endif()
if(MPI_Fortran_FOUND)
  set(mnFFLAGS "${GAMBIT_Fortran_FLAGS_PLUS_MPI}")
else()
  set(mnFFLAGS "${GAMBIT_Fortran_FLAGS}")
endif()
ExternalProject_Add(${name}_${ver} 
  #FIXME automated download of multinest is not possible, as it is behind a login redirection wall.  Need to ask CCPForge for a solution.
  #URL http://ccpforge.cse.rl.ac.uk/gf/download/frsrelease/413/5871/MultiNest_v3.9.tar.gz
  #URL_MD5 6c0c9e9ee0ac3c906109675302fb30f0
  DOWNLOAD_DIR ${scanner_download}
  DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow --bold ${private_code_warning1}
           COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold ${private_code_warning2}
           COMMAND ${CMAKE_COMMAND} -E copy_directory ${loc} ${dir}
  SOURCE_DIR ${dir}
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND sed ${dashi} -e "s#nested.o[[:space:]]*$#nested.o cwrapper.o#g"
                                 -e "s#-o[[:space:]]*\\(\\$\\)(LIBS)[[:space:]]*\\$@[[:space:]]*\\$^#-o \\$\\(LIBS\\)\\$@ \\$^ ${mnLAPACK}#g"
                                 <SOURCE_DIR>/Makefile 
            COMMAND sed ${dashi} -e "s#function[[:space:]]*loglike_proto(Cube,n_dim,nPar,context)[[:space:]]*$#function loglike_proto(Cube,n_dim,nPar,context) bind(c)#g"
                                 -e "s#subroutine[[:space:]]*dumper_proto(nSamples,nlive,nPar,physLive,posterior,paramConstr,maxLogLike,logZ,INSlogZ,logZerr,context)[[:space:]]*$#subroutine dumper_proto(nSamples,nlive,nPar,physLive,posterior,paramConstr,maxLogLike,logZ,INSlogZ,logZerr,context) bind(c)#g"
                                 <SOURCE_DIR>/cwrapper.f90
  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FFLAGS=${mnFFLAGS} LINKLIB=${mnSO_LINK}$ LIBS=${dir}/
  INSTALL_COMMAND "" 
)
add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
set_as_default_version("scanner" ${name} ${ver})


# GreAT
set(name "great")
set(ver "1.0.0")
set(lib "libgreat")
set(dl "null")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/GreAT/${ver}")
ExternalProject_Add(${name}_${ver}
  GIT_REPOSITORY https://gitlab.in2p3.fr/derome/GreAT.git
  SOURCE_DIR ${dir}
  CMAKE_COMMAND ${CMAKE_COMMAND} ..
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
  INSTALL_COMMAND ""
)
add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
if (ROOT_FOUND)
  set_as_default_version("scanner" ${name} ${ver})
endif()


# All other scanners are implemented natively in ScannerBit.
