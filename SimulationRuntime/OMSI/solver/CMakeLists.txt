CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
#OpenModelica Simulation Interface solver
PROJECT(OMSISolver)
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Find blas and lapack
IF(IS_MINGW32)
  SET(LAPACK_MINGW  $ENV{OMDEV}/tools/msys/mingw32/bin/libopenblas.dll)
  SET(LAPACK_LIBRARIES  ${LAPACK_MINGW})
ELSEIF(IS_MINGW64)
  SET(LAPACK_MINGW  $ENV{OMDEV}/tools/msys/mingw64/bin/libopenblas.dll)
  SET(LAPACK_LIBRARIES  ${LAPACK_MINGW})
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

# Find SUNDIALS KINSOL
#FIND_LIBRARY(KINSOL_LIBRARIES sundials_kinsol
#             PATH ${CMAKE_INSTALL_PREFIX}/lib/omc)
#MESSAGE(STATUS "KINSOL Libraries: ${KINSOL_LIBRARIES}")

#include_directories ("${CMAKE_INSTALL_PREFIX}/include/omc/c/sundials")
#MESSAGE(STATUS "Location of SUNDIALS headers: ${CMAKE_INSTALL_PREFIX}/include/omc/c/sundials")

# OMSISolver includes
include_directories ("${CMAKE_SOURCE_DIR}/solver/include")

add_library(${OMSISolverName}
  src/solver_api.c
  src/solver_helper.c
#  src/solver_kinsol.c
  src/solver_lapack.c
  src/solver_newton.c
)


target_link_libraries(${OMSISolverName} ${CMAKE_DL_LIBS} ${LAPACK_LIBRARIES} ${KINSOL_LIBRARIES})

install(TARGETS ${OMSISolverName} DESTINATION ${LIBINSTALLEXT})

install(FILES
  ${CMAKE_SOURCE_DIR}/solver/include/omsi_solver.h
  ${CMAKE_SOURCE_DIR}/solver/include/solver_api.h
  ${CMAKE_SOURCE_DIR}/solver/include/solver_helper.h
DESTINATION include/omc/omsi/solver)
