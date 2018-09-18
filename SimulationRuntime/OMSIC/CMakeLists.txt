CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
SET(CMAKE_VERBOSE_MAKEFILE ON)
MESSAGE(STATUS "CMake version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

PROJECT(OMSICSimulationRuntime)

# enable warnings and use ANSI C compatible compiler
if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
  set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra -ansi -pedantic")
endif(CMAKE_COMPILER_IS_GNUCXX)

if(NOT PLATFORM OR PLATFORM STREQUAL "dynamic")
  set(BUILD_SHARED_LIBS ON)
elseif(PLATFORM STREQUAL "static")
  set(BUILD_SHARED_LIBS OFF)
else()
endif()

IF(BUILD_SHARED_LIBS)
  SET(LIBSUFFIX "")
ELSE(BUILD_SHARED_LIBS)
  SET(LIBSUFFIX "_static")
ENDIF(BUILD_SHARED_LIBS)

 IF(MSVC)
  MESSAGE(STATUS "MSVC")
  SET(LIBINSTALLEXT "$ENV{OPENMODELICAHOME}/lib/omc/omsi/msvc" CACHE STRING "library directory")
ELSE(MSVC)
  SET(LIBINSTALLEXT "$ENV{OPENMODELICAHOME}/lib/omc/omsi" CACHE STRING "library directory")
 ENDIF(MSVC)

SET(OSUCName ${LIBPREFIX}OMSIC${LIBSUFFIX})

SET(OMSI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../OMSI)

include_directories ("${OMSI_SOURCE_DIR}/include")
include_directories ("${OMSI_SOURCE_DIR}/base/include")

# omsic simulation runtime
ADD_SUBDIRECTORY(src/solver)
ADD_SUBDIRECTORY(src/omsu)


install(FILES
  ${OMSI_SOURCE_DIR}/include/omsi.h
  ${OMSI_SOURCE_DIR}/include/omsi_callbacks.h
  ${OMSI_SOURCE_DIR}/include/omsi_cs.h
  ${OMSI_SOURCE_DIR}/include/omsi_jacobian.h
  ${OMSI_SOURCE_DIR}/include/omsi_me.h
DESTINATION $ENV{OPENMODELICAHOME}/include/omc/omsi)

install(FILES
  ${CMAKE_SOURCE_DIR}/include/omsu/omsu_common.h
  ${CMAKE_SOURCE_DIR}/include/omsu/omsu_helper.h
  ${CMAKE_SOURCE_DIR}/include/omsu/omsu_initialization.h
  ${CMAKE_SOURCE_DIR}/include/omsu/omsu_utils.h
DESTINATION $ENV{OPENMODELICAHOME}/include/omc/omsic)

install(FILES
  ${CMAKE_SOURCE_DIR}/include/solver/solver_helper.h
  ${CMAKE_SOURCE_DIR}/include/solver/solver_lapack.h
DESTINATION $ENV{OPENMODELICAHOME}/include/omc/omsi/solver)
