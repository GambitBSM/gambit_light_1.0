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
#  \author Will Handley (wh260@cam.ac.uk)
#  \date 2018 May, Dec
#
#************************************************

# Diver
if(NOT GAMBIT_LIGHT) # Do not include this version in GAMBIT-light
set(name "diver")
set(ver "1.0.0")
set(lib "libdiver")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "61c76e948855f19dfa394c14df8c6af2")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/ScannerBit/patches/${name}/${ver}/patch_${name}_${ver}.dif")
set(diverSO_LINK_FLAGS "${CMAKE_Fortran_MPI_SO_LINK_FLAGS} -fopenmp")
if(MPI_Fortran_FOUND)
  set(diverFFLAGS "${BACKEND_Fortran_FLAGS_PLUS_MPI}")
else()
  set(diverFFLAGS "${BACKEND_Fortran_FLAGS}")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_DIR ${scanner_download}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND patch -p1 < ${patch}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${MAKE_PARALLEL} ${lib}.so FF=${CMAKE_Fortran_COMPILER} MODULE=${FMODULE} FOPT=${diverFFLAGS} SO_LINK_FLAGS=${diverSO_LINK_FLAGS}
    INSTALL_COMMAND ""
  )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
endif()
endif() # End if(NOT GAMBIT_LIGHT)

if(NOT GAMBIT_LIGHT) # Do not include this version in GAMBIT-light
set(name "diver")
set(ver "1.0.2")
set(lib "libdiver")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "28c74db26c573d745383e303f6bece18")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(diverSO_LINK_FLAGS "${CMAKE_Fortran_MPI_SO_LINK_FLAGS} -fopenmp")
if(MPI_Fortran_FOUND)
  set(diverFFLAGS "${BACKEND_Fortran_FLAGS_PLUS_MPI}")
else()
  set(diverFFLAGS "${BACKEND_Fortran_FLAGS}")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_DIR ${scanner_download}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${MAKE_PARALLEL} ${lib}.so FF=${CMAKE_Fortran_COMPILER} MODULE=${FMODULE} FOPT=${diverFFLAGS} SO_LINK_FLAGS=${diverSO_LINK_FLAGS}
    INSTALL_COMMAND ""
  )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
endif()
endif() # End if(NOT GAMBIT_LIGHT)

if(NOT GAMBIT_LIGHT) # Do not include this version in GAMBIT-light
set(name "diver")
set(ver "1.0.4")
set(lib "libdiver")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "2cdf72c58d57ba88ef6b747737796ddf")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(diverSO_LINK_FLAGS "${CMAKE_Fortran_MPI_SO_LINK_FLAGS} -fopenmp")
if(MPI_Fortran_FOUND)
  set(diverFFLAGS "${BACKEND_Fortran_FLAGS_PLUS_MPI}")
else()
  set(diverFFLAGS "${BACKEND_Fortran_FLAGS}")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_DIR ${scanner_download}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${MAKE_PARALLEL} ${lib}.so FF=${CMAKE_Fortran_COMPILER} MODULE=${FMODULE} FOPT=${diverFFLAGS} SO_LINK_FLAGS=${diverSO_LINK_FLAGS}
    INSTALL_COMMAND ""
  )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
endif()
endif() # End if(NOT GAMBIT_LIGHT)

set(name "diver")
set(ver "1.0.5")
set(lib "libdiver")
set(dl "https://${name}.hepforge.org/downloads/${name}-${ver}.tar.gz")
set(md5 "faa431910cc837fdc6a2895a74635caa")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(diverSO_LINK_FLAGS "${CMAKE_Fortran_MPI_SO_LINK_FLAGS} -fopenmp")
if(MPI_Fortran_FOUND)
  set(diverFFLAGS "${BACKEND_Fortran_FLAGS_PLUS_MPI}")
else()
  set(diverFFLAGS "${BACKEND_Fortran_FLAGS}")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_DIR ${scanner_download}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} ${lib}.so FF=${CMAKE_Fortran_COMPILER} MODULE=${FMODULE} FOPT=${diverFFLAGS} SO_LINK_FLAGS=${diverSO_LINK_FLAGS}
    INSTALL_COMMAND ""
  )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("scanner" ${name} ${ver})
endif()

# PolyChord
if(NOT GAMBIT_LIGHT) # Do not include this version in GAMBIT-light
set(name "polychord")
set(ver "1.17.1")
set(lib "libchord")
set(md5 "c47a4b58e1ce5b98eec3b7d79ec7284f")
set(dl "https://github.com/PolyChord/PolyChordLite/archive/${ver}.tar.gz")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(pcSO_LINK "${CMAKE_Fortran_COMPILER} ${OpenMP_Fortran_FLAGS} ${CMAKE_Fortran_MPI_SO_LINK_FLAGS} ${CMAKE_CXX_MPI_SO_LINK_FLAGS}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  string(REGEX REPLACE "(-lstdc\\+\\+)" "" pcSO_LINK "${pcSO_LINK}")
  set(pcSO_LINK "${pcSO_LINK} -lc++")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  string(REGEX REPLACE "(-lc\\+\\+)" "" pcSO_LINK "${pcSO_LINK}")
  set(pcSO_LINK "${pcSO_LINK} -lstdc++")
endif()
if(MPI_Fortran_FOUND)
  set(pcFFLAGS "${BACKEND_Fortran_FLAGS_PLUS_MPI}")
else()
  set(pcFFLAGS "${BACKEND_Fortran_FLAGS}")
endif()
if(MPI_CXX_FOUND)
  set(pcCXXFLAGS "${BACKEND_CXX_FLAGS_PLUS_MPI}")
else()
  set(pcCXXFLAGS "${BACKEND_CXX_FLAGS}")
endif()
if("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Intel")
  set(pcFFLAGS "${pcFFLAGS} -heap-arrays -assume noold_maxminloc ")
  set(pcCOMPILER_TYPE "intel")
else()
  set(pcFFLAGS "${pcFFLAGS} -fno-stack-arrays")
  set(pcCOMPILER_TYPE "gnu")
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(pcCXXFLAGS "${pcCXXFLAGS} -Wl,-undefined,dynamic_lookup")
  set(pcFFLAGS "${pcFFLAGS} -Wl,-undefined,dynamic_lookup")
  set(pcSO_LINK "${pcSO_LINK} -lstdc++")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${MAKE_PARALLEL} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FFLAGS=${pcFFLAGS} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${pcCXXFLAGS} LD=${pcSO_LINK} COMPILER_TYPE=${pcCOMPILER_TYPE}
    INSTALL_COMMAND ""
    )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
endif()
endif() # End if(NOT GAMBIT_LIGHT)

# PolyChord
if(NOT GAMBIT_LIGHT) # Do not include this version in GAMBIT-light
set(name "polychord")
set(ver "1.18.2")
set(lib "libchord")
set(md5 "66eca0d1fbcc87091e8079515810b421")
set(dl "https://github.com/PolyChord/PolyChordLite/archive/${ver}.tar.gz")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(pcSO_LINK "${CMAKE_Fortran_COMPILER} ${OpenMP_Fortran_FLAGS} ${CMAKE_Fortran_MPI_SO_LINK_FLAGS} ${CMAKE_CXX_MPI_SO_LINK_FLAGS}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  string(REGEX REPLACE "(-lstdc\\+\\+)" "" pcSO_LINK "${pcSO_LINK}")
  set(pcSO_LINK "${pcSO_LINK} -lc++")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  string(REGEX REPLACE "(-lc\\+\\+)" "" pcSO_LINK "${pcSO_LINK}")
  set(pcSO_LINK "${pcSO_LINK} -lstdc++")
endif()
if(MPI_Fortran_FOUND)
  set(pcFFLAGS "${BACKEND_Fortran_FLAGS_PLUS_MPI}")
else()
  set(pcFFLAGS "${BACKEND_Fortran_FLAGS}")
endif()
if(MPI_CXX_FOUND)
  set(pcCXXFLAGS "${BACKEND_CXX_FLAGS_PLUS_MPI}")
else()
  set(pcCXXFLAGS "${BACKEND_CXX_FLAGS}")
endif()
if("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Intel")
  set(pcFFLAGS "${pcFFLAGS} -heap-arrays -assume noold_maxminloc ")
  set(pcCOMPILER_TYPE "intel")
else()
  set(pcFFLAGS "${pcFFLAGS} -fno-stack-arrays")
  set(pcCOMPILER_TYPE "gnu")
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(pcCXXFLAGS "${pcCXXFLAGS} -Wl,-undefined,dynamic_lookup")
  set(pcFFLAGS "${pcFFLAGS} -Wl,-undefined,dynamic_lookup")
  set(pcSO_LINK "${pcSO_LINK} -lstdc++")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND sed ${dashi} -e "s#-lc#-lstdc#g" Makefile_gnu
    BUILD_COMMAND ${MAKE_PARALLEL} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FFLAGS=${pcFFLAGS} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${pcCXXFLAGS} LD=${pcSO_LINK} COMPILER_TYPE=${pcCOMPILER_TYPE}
    INSTALL_COMMAND ""
  )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
endif()
endif() # End if(NOT GAMBIT_LIGHT)

# PolyChord
set(name "polychord")
set(ver "1.20.1")
set(lib "libchord")
set(md5 "e05d2f53f7ef909b214ea2372ac8b5eb")
set(dl "https://github.com/PolyChord/PolyChordLite/archive/${ver}.tar.gz")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(pcSO_LINK "${CMAKE_Fortran_COMPILER} ${OpenMP_Fortran_FLAGS} ${CMAKE_Fortran_MPI_SO_LINK_FLAGS} ${CMAKE_CXX_MPI_SO_LINK_FLAGS}")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  string(REGEX REPLACE "(-lstdc\\+\\+)" "" pcSO_LINK "${pcSO_LINK}")
  set(pcSO_LINK "${pcSO_LINK} -lc++")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  string(REGEX REPLACE "(-lc\\+\\+)" "" pcSO_LINK "${pcSO_LINK}")
  set(pcSO_LINK "${pcSO_LINK} -lstdc++")
endif()
if(MPI_Fortran_FOUND)
  set(pcFFLAGS "${BACKEND_Fortran_FLAGS_PLUS_MPI}")
else()
  set(pcFFLAGS "${BACKEND_Fortran_FLAGS}")
endif()
if(MPI_CXX_FOUND)
  set(pcCXXFLAGS "${BACKEND_CXX_FLAGS_PLUS_MPI}")
else()
  set(pcCXXFLAGS "${BACKEND_CXX_FLAGS}")
endif()
if("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Intel")
  set(pcFFLAGS "${pcFFLAGS} -heap-arrays -assume noold_maxminloc ")
  set(pcCOMPILER_TYPE "intel")
else()
  set(pcFFLAGS "${pcFFLAGS} -fno-stack-arrays")
  set(pcCOMPILER_TYPE "gnu")
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(pcCXXFLAGS "${pcCXXFLAGS} -Wl,-undefined,dynamic_lookup")
  set(pcFFLAGS "${pcFFLAGS} -Wl,-undefined,dynamic_lookup")
  set(pcSO_LINK "${pcSO_LINK} -lstdc++")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND sed ${dashi} -e "s#-lc#-lstdc#g" Makefile_gnu
    BUILD_COMMAND ${MAKE_PARALLEL} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FFLAGS=${pcFFLAGS} CXX=${CMAKE_CXX_COMPILER} CXXFLAGS=${pcCXXFLAGS} LD=${pcSO_LINK} COMPILER_TYPE=${pcCOMPILER_TYPE}
    INSTALL_COMMAND ""
    )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("scanner" ${name} ${ver})
endif()

# MultiNest
if(NOT GAMBIT_LIGHT) # Do not include this version in GAMBIT-light
set(name "multinest")
set(ver "3.10")
set(lib "libnest3")
set(md5 "f94eb954d31aefac0bef14c0717adfa1")
set(dl "https://github.com/farhanferoz/MultiNest/archive/4b3709c6d659adbd62c85e3e95ff7eeb6e6617af.tar.gz")
set(dl "https://github.com/farhanferoz/MultiNest/archive/MultiNestv${ver}.tar.gz")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(mn_subdir "${dir}/MultiNest_v${ver}")
set(mnSO_LINK "${CMAKE_Fortran_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS} ${OpenMP_Fortran_FLAGS} ${CMAKE_Fortran_MPI_SO_LINK_FLAGS}")
if (NOT LAPACK_STATIC)
  set(mnLAPACK "${LAPACK_LINKLIBS}")
endif()
if(MPI_Fortran_FOUND)
  set(mnFFLAGS "${BACKEND_Fortran_FLAGS_PLUS_MPI}")
else()
  set(mnFFLAGS "${BACKEND_Fortran_FLAGS}")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND sed ${dashi} -e "s#nested.o[[:space:]]*$#nested.o cwrapper.o#g"
                                   -e "s#-o[[:space:]]*\\(\\$\\)(LIBS)[[:space:]]*\\$@[[:space:]]*\\$^#-o \\$\\(LIBS\\)\\$@ \\$^ ${mnLAPACK}#g"
                                   -e "s#default:#.NOTPARALLEL:${nl}${nl}default:#"
                                   ${mn_subdir}/Makefile
              COMMAND ${CMAKE_COMMAND} -E copy ${mn_subdir}/Makefile ${mn_subdir}/Makefile.tmp
              COMMAND awk "{gsub(/${nl}/,${true_nl})}{print}" ${mn_subdir}/Makefile.tmp > ${mn_subdir}/Makefile
              COMMAND ${CMAKE_COMMAND} -E remove ${mn_subdir}/Makefile.tmp
              COMMAND sed ${dashi} -e "s#function[[:space:]]*loglike_proto(Cube,n_dim,nPar,context)[[:space:]]*$#function loglike_proto(Cube,n_dim,nPar,context) bind(c)#g"
                                   -e "s#subroutine[[:space:]]*dumper_proto(nSamples,nlive,nPar,physLive,posterior,paramConstr,maxLogLike,logZ,INSlogZ,logZerr,context)[[:space:]]*$#subroutine dumper_proto(nSamples,nlive,nPar,physLive,posterior,paramConstr,maxLogLike,logZ,INSlogZ,logZerr,context) bind(c)#g"
                                   ${mn_subdir}/cwrapper.f90
    BUILD_COMMAND cd ${mn_subdir} && ${MAKE_PARALLEL} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FFLAGS=${mnFFLAGS} LINKLIB=${mnSO_LINK} LIBS=${dir}/
    INSTALL_COMMAND ""
  )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
endif()
endif() # End if(NOT GAMBIT_LIGHT)

# MultiNest
if(NOT GAMBIT_LIGHT) # Do not include this version in GAMBIT-light
set(name "multinest")
set(ver "3.11")
set(lib "libnest3")
set(md5 "ebaf960c348592a1b6e3a50b3794c357")
set(dl "https://github.com/farhanferoz/MultiNest/archive/4b3709c6d659adbd62c85e3e95ff7eeb6e6617af.tar.gz")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(mn_subdir "${dir}/MultiNest_v${ver}")
set(mnSO_LINK "${CMAKE_Fortran_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS} ${OpenMP_Fortran_FLAGS} ${CMAKE_Fortran_MPI_SO_LINK_FLAGS}")
if (NOT LAPACK_STATIC)
  set(mnLAPACK "${LAPACK_LINKLIBS}")
endif()
if(MPI_Fortran_FOUND)
  set(mnFFLAGS "${BACKEND_Fortran_FLAGS_PLUS_MPI}")
else()
  set(mnFFLAGS "${BACKEND_Fortran_FLAGS}")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND sed ${dashi} -e "s#nested.o[[:space:]]*$#nested.o cwrapper.o#g"
                                   -e "s#-o[[:space:]]*\\(\\$\\)(LIBS)[[:space:]]*\\$@[[:space:]]*\\$^#-o \\$\\(LIBS\\)\\$@ \\$^ ${mnLAPACK}#g"
                                   -e "s#default:#.NOTPARALLEL:${nl}${nl}default:#"
                                   ${mn_subdir}/Makefile
              COMMAND ${CMAKE_COMMAND} -E copy ${mn_subdir}/Makefile ${mn_subdir}/Makefile.tmp
              COMMAND awk "{gsub(/${nl}/,${true_nl})}{print}" ${mn_subdir}/Makefile.tmp > ${mn_subdir}/Makefile
              COMMAND ${CMAKE_COMMAND} -E remove ${mn_subdir}/Makefile.tmp
              COMMAND sed ${dashi} -e "s#function[[:space:]]*loglike_proto(Cube,n_dim,nPar,context)[[:space:]]*$#function loglike_proto(Cube,n_dim,nPar,context) bind(c)#g"
                                   -e "s#subroutine[[:space:]]*dumper_proto(nSamples,nlive,nPar,physLive,posterior,paramConstr,maxLogLike,logZ,INSlogZ,logZerr,context)[[:space:]]*$#subroutine dumper_proto(nSamples,nlive,nPar,physLive,posterior,paramConstr,maxLogLike,logZ,INSlogZ,logZerr,context) bind(c)#g"
                                   ${mn_subdir}/cwrapper.f90
    BUILD_COMMAND cd ${mn_subdir} && ${MAKE_PARALLEL} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FFLAGS=${mnFFLAGS} LINKLIB=${mnSO_LINK} LIBS=${dir}/
    INSTALL_COMMAND ""
  )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
endif()
endif() # End if(NOT GAMBIT_LIGHT)

# MultiNest
set(name "multinest")
set(ver "3.12")
set(lib "libnest3")
set(md5 "1a7af4d9c73f16b0038876c3857c5ff8")
set(dl "https://github.com/farhanferoz/MultiNest/archive/cc616c17ed440fbff8d35937997c86256ed4d744.tar.gz")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(mn_subdir "${dir}/MultiNest_v${ver}")
set(mnSO_LINK "${CMAKE_Fortran_COMPILER} ${CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS} ${OpenMP_Fortran_FLAGS} ${CMAKE_Fortran_MPI_SO_LINK_FLAGS}")
if (NOT LAPACK_STATIC)
  set(mnLAPACK "${LAPACK_LINKLIBS}")
endif()
if(MPI_Fortran_FOUND)
  set(mnFFLAGS "${BACKEND_Fortran_FLAGS_PLUS_MPI}")
else()
  set(mnFFLAGS "${BACKEND_Fortran_FLAGS}")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND sed ${dashi} -e "s#nested.o[[:space:]]*$#nested.o cwrapper.o#g"
                                   -e "s#-o[[:space:]]*\\(\\$\\)(LIBS)[[:space:]]*\\$@[[:space:]]*\\$^#-o \\$\\(LIBS\\)\\$@ \\$^ ${mnLAPACK}#g"
                                   -e "s#default:#.NOTPARALLEL:${nl}${nl}default:#"
                                   ${mn_subdir}/Makefile
              COMMAND ${CMAKE_COMMAND} -E copy ${mn_subdir}/Makefile ${mn_subdir}/Makefile.tmp
              COMMAND awk "{gsub(/${nl}/,${true_nl})}{print}" ${mn_subdir}/Makefile.tmp > ${mn_subdir}/Makefile
              COMMAND ${CMAKE_COMMAND} -E remove ${mn_subdir}/Makefile.tmp
              COMMAND sed ${dashi} -e "s#function[[:space:]]*loglike_proto(Cube,n_dim,nPar,context)[[:space:]]*$#function loglike_proto(Cube,n_dim,nPar,context) bind(c)#g"
                                   -e "s#subroutine[[:space:]]*dumper_proto(nSamples,nlive,nPar,physLive,posterior,paramConstr,maxLogLike,logZ,INSlogZ,logZerr,context)[[:space:]]*$#subroutine dumper_proto(nSamples,nlive,nPar,physLive,posterior,paramConstr,maxLogLike,logZ,INSlogZ,logZerr,context) bind(c)#g"
                                   ${mn_subdir}/cwrapper.f90
    BUILD_COMMAND cd ${mn_subdir} && ${MAKE_PARALLEL} ${lib}.so FC=${CMAKE_Fortran_COMPILER} FFLAGS=${mnFFLAGS} LINKLIB=${mnSO_LINK} LIBS=${dir}/
    INSTALL_COMMAND ""
  )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("scanner" ${name} ${ver})
endif()

# GreAT
if(NOT GAMBIT_LIGHT) # Do not include this version in GAMBIT-light
set(name "great")
set(ver "1.0.0")
set(lib "libgreat")
set(dl "https://gitlab.in2p3.fr/derome/GreAT/-/archive/master/GreAT-master.tar.gz")
set(md5 "a0b6aeb4c6307f8ba980f69bb797a093")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
set(patch "${PROJECT_SOURCE_DIR}/ScannerBit/patches/${name}/${ver}/patch_${name}_${ver}.dif")
set(build_dir "${PROJECT_BINARY_DIR}/${name}_${ver}-prefix/src/${name}_${ver}-build")
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    PATCH_COMMAND patch -p1 < ${patch}
    CMAKE_COMMAND ${CMAKE_COMMAND} ..
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS=${BACKEND_CXX_FLAGS}
    BUILD_COMMAND ${MAKE_PARALLEL} ${name}
    INSTALL_COMMAND ${CMAKE_COMMAND} -E create_symlink ${build_dir}/Manager/Manager_Dict_rdict.pcm ${build_dir}/lib/Manager_Dict_rdict.pcm
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${build_dir}/MCMC/MCMC_Dict_rdict.pcm ${build_dir}/lib/MCMC_Dict_rdict.pcm
  )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("scanner" ${name} ${ver})
endif()
endif() # End if(NOT GAMBIT_LIGHT)

# minuit2
# omp possible in principle but disabled in gambit, as only likelihood uses omp
set(name "minuit2")
set(ver "6.23.01")
set(lib "libminuit2")
set(md5 "862cc44a3defa0674e6b5a9960ed6f89")
set(dl "https://github.com/GooFit/Minuit2/archive/v6-23-01.tar.gz")
set(dir "${PROJECT_SOURCE_DIR}/ScannerBit/installed/${name}/${ver}")
# We need to disable MPI for Minuit2 on MacOS as the build system fails to find the proper headers
set(minuit2_MPI "${MPI_CXX_FOUND}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(minuit2_MPI "OFF")
endif()
check_ditch_status(${name} ${ver} ${dir})
if(NOT ditched_${name}_${ver})
  ExternalProject_Add(${name}_${ver}
    DOWNLOAD_COMMAND ${DL_SCANNER} ${dl} ${md5} ${dir} ${name} ${ver}
    SOURCE_DIR ${dir}
    BUILD_IN_SOURCE 1
    UPDATE_COMMAND ${CMAKE_COMMAND} -E echo "set_target_properties(Minuit2 PROPERTIES OUTPUT_NAME Minuit2 SUFFIX \".so\")" >> ${dir}/CMakeLists.txt
    CMAKE_COMMAND ${CMAKE_COMMAND} ..
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DBUILD_SHARED_LIBS=1 -Dminuit2_mpi=${minuit2_MPI} -Dminuit2_openmp=0 -Dminuit2_omp=0
    BUILD_COMMAND ${MAKE_PARALLEL}
    INSTALL_COMMAND ""
  )
  add_extra_targets("scanner" ${name} ${ver} ${dir} ${dl} clean)
  set_as_default_version("scanner" ${name} ${ver})
endif()

# Finally, deal with all the python scanners
include(cmake/python_scanners.cmake)

# All other scanners are implemented natively in ScannerBit.
