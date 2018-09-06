CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
#OpenModelica Simulation Interface solver
PROJECT(OMSISolver)
SET(CMAKE_VERBOSE_MAKEFILE ON)

set(OMSISolverName ${LIBPREFIX}OMSISolver${LIBSUFFIX})


# Find blas and lapack

#IF(WIN32)
#  SET(BLAS_blas_LIBRARY $ENV{OMDEV}/lib/3rdParty/Lapack/Lib/blas_win32.dll CACHE FILEPATH "Blas library")
#  SET(LAPACK_lapack_LIBRARY  $ENV{OMDEV}/lib/3rdParty/Lapack/Lib/lapack_win32.dll CACHE FILEPATH "Lapack library")
#  SET(BLAS_DIRY "$ENV{OMDEV}/lib/3rdParty/Lapack/Lib/" CACHE LOCATION "where was yarp built?")
#  SET(LAPACK_LIBRARIES  ${BLAS_blas_LIBRARY} ${LAPACK_lapack_LIBRARY} )
#ELSEIF(MSVC)
#  #workaround  because cmake does not find the lapack libraries for Visual Studio 10
#  SET(LAPACK_MSVC_10  $ENV{OMDEV}/lib/3rdParty/Lapack/Lib/lapack_win32.lib )
#  SET(BLAS_MSVC_10 $ENV{OMDEV}/lib/3rdParty/Lapack/Lib/blas_win32.lib )
#  SET(LAPACK_LIBRARIES  ${LAPACK_MSVC_10}   ${BLAS_MSVC_10} )
#ELSEIF(IS_MINGW32)
IF(IS_MINGW32)
  SET(LAPACK_MINGW  $ENV{OMDEV}/tools/msys/mingw32/bin/libopenblas.dll )
  SET(LAPACK_LIBRARIES  ${LAPACK_MINGW} )
ELSEIF(IS_MINGW64)
  SET(LAPACK_MINGW  $ENV{OMDEV}/tools/msys/mingw64/bin/libopenblas.dll )
  SET(LAPACK_LIBRARIES  ${LAPACK_MINGW} )
ELSE()
  FIND_PACKAGE(BLAS)
  FIND_PACKAGE(LAPACK)
ENDIF()


IF (BLAS_FOUND STREQUAL "NOTFOUND")
  MESSAGE(FATAL_ERROR "Error: Blas Libraries not found!")
ENDIF()
IF (LAPACK_FOUND STREQUAL "NOTFOUND")
  MESSAGE(FATAL_ERROR "Error: Lapack Libraries not found!")
ENDIF()

MESSAGE(STATUS "Lapack Libraries: ${LAPACK_LIBRARIES}")



include_directories ("${CMAKE_SOURCE_DIR}/include/solver")

add_library(${OMSISolverName} lapack/solver_lapack.c solver_helper.c)


target_link_libraries(${OMSISolverName} ${CMAKE_DL_LIBS} ${LAPACK_LIBRARIES})

install(TARGETS ${OMSISolverName} DESTINATION ${LIBINSTALLEXT})
