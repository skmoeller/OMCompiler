CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
#OpenModelica Simulation Interface solver
PROJECT(OMSISolver)
SET(CMAKE_VERBOSE_MAKEFILE ON)

set(OMSISolverName ${LIBPREFIX}OMSISolver${LIBSUFFIX})

#set LAPACK lib

include_directories ("${CMAKE_SOURCE_DIR}/include/solver")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/src/solver/lapack)

add_library(solver_lapack SHARED lapack/solver_lapack.c solver_helper.c)

target_link_libraries(${OMSISolverName} ${CMAKE_DL_LIBS} lapack blas)