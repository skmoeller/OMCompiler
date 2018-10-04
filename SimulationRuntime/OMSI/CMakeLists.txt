CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
SET(CMAKE_VERBOSE_MAKEFILE ON)
MESSAGE(STATUS "CMake version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

PROJECT(OMSIBaseSimulationRuntime)

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
  SET(LIBINSTALLEXT "${LIB_OMC}/omsi/msvc" CACHE STRING "library directory")
ELSE(MSVC)
  SET(LIBINSTALLEXT "${LIB_OMC}/omsi" CACHE STRING "library directory")
ENDIF(MSVC)
message(STATUS "Libs will be installed in ${LIBINSTALLEXT}")

SET(OSUBaseName ${LIBPREFIX}OMSIBase${LIBSUFFIX})





include_directories ("${CMAKE_SOURCE_DIR}/include")
# omsi base simulation runtime
ADD_SUBDIRECTORY(base)


install(FILES
  ${CMAKE_SOURCE_DIR}/include/omsi.h
  ${CMAKE_SOURCE_DIR}/include/omsi_callbacks.h
  ${CMAKE_SOURCE_DIR}/include/omsi_cs.h
  ${CMAKE_SOURCE_DIR}/include/omsi_jacobian.h
  ${CMAKE_SOURCE_DIR}/include/omsi_me.h
  DESTINATION /include/omc/omsi)

install(FILES
  ${CMAKE_SOURCE_DIR}/include/fmi2/omsi_fmi2_cs.h
  ${CMAKE_SOURCE_DIR}/include/fmi2/omsi_fmi2_me.h
  DESTINATION /include/omc/omsi/fmi2/)





