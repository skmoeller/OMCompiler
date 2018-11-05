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

#set OMSIBase lib
if(NOT PLATFORM OR PLATFORM STREQUAL "dynamic")
  #set (libOMSIBase ${CMAKE_INSTALL_PREFIX}/${LIBINSTALLEXT}/libOMSIBase${SHREXT})
  set (libOMSIBase ${CMAKE_SOURCE_DIR}/Build_dynamic/base/libOMSIBase${SHREXT})
elseif(PLATFORM STREQUAL "static")
  #set (libOMSIBase ${CMAKE_INSTALL_PREFIX}/${LIBINSTALLEXT}/libOMSIBase${LIBSUFFIX}.a)
  set (libOMSIBase ${CMAKE_SOURCE_DIR}/Build_dynamic/base/libOMSIBase${LIBSUFFIX}.a)
else()
endif()
MESSAGE(STATUS "OMSI Base Library: ${libOMSIBase}")


include_directories ("${CMAKE_SOURCE_DIR}/solver/include")

add_library(${OMSISolverName}
  src/solver_api.c
)


target_link_libraries(${OMSISolverName} ${CMAKE_DL_LIBS} ${libOMSIBase} ${LAPACK_LIBRARIES})

install(TARGETS ${OMSISolverName} DESTINATION ${LIBINSTALLEXT})

install(FILES
  ${CMAKE_SOURCE_DIR}/solver/include/omsi_solver.h
DESTINATION include/omc/omsi/solver)
