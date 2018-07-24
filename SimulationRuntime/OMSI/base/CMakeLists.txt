CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
#OMSI base simulation runtime
PROJECT(OMSIBASE)
SET(CMAKE_VERBOSE_MAKEFILE ON)
include_directories ("${CMAKE_SOURCE_DIR}/base/include")
add_library(${OSUBaseName} SHARED src/omsu_input_json.c src/omsu_input_xml.c )
IF(WIN32)
    target_link_libraries(${OSUBaseName}  ${CMAKE_DL_LIBS} wsock32 ws2_32)
ELSE(WIN32)
    target_link_libraries(${OSUBaseName} ${CMAKE_DL_LIBS} )
ENDIF(WIN32)

install(TARGETS ${OSUBaseName} DESTINATION ${LIBINSTALLEXT})




