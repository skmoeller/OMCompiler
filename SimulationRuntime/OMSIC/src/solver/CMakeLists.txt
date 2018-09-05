CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
#OpenModelica Simulation Interface solver
PROJECT(OMSISolver)
SET(CMAKE_VERBOSE_MAKEFILE ON)

set(OMSISolverName ${LIBPREFIX}OMSISolver${LIBSUFFIX})

#set LAPACK and BLAS lib
find_library(liblapack LAPACK)
find_library(libblas BLAS)

include_directories ("${CMAKE_SOURCE_DIR}/include/solver")

add_library(${OMSISolverName} lapack/solver_lapack.c solver_helper.c)


target_link_libraries(${OMSISolverName} ${CMAKE_DL_LIBS} ${liblapack} ${libblas})

install(TARGETS ${OMSISolverName} DESTINATION ${LIBINSTALLEXT})
