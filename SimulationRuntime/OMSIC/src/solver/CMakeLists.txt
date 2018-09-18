CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
#OpenModelica Simulation Interface solver
PROJECT(OMSISolver)
SET(CMAKE_VERBOSE_MAKEFILE ON)

set(OMSISolverName ${LIBPREFIX}OMSISolver${LIBSUFFIX})


# Find blas and lapack

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
