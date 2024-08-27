add_definitions(-DIGEXPORTDEF)

file(GLOB_RECURSE INFOGEN_HEADERS "./Headers/*.h")
file(GLOB_RECURSE INFOGEN_SOURCES "./Sources/*.cxx" "./Sources/*.hxx")

add_library(${MAIN_PROGRAM_NAME}Lib SHARED ${INFOGEN_HEADERS} ${INFOGEN_SOURCES} ./InfoGen.cc ./Headers/InfoGen.h)
SET_TARGET_PROPERTIES(${MAIN_PROGRAM_NAME}Lib PROPERTIES
    OUTPUT_NAME ${MAIN_PROGRAM_NAME}
    PREFIX ""
    SUFFIX ".so"
)

target_include_directories(${MAIN_PROGRAM_NAME}Lib PRIVATE ${CSE_INCLUDE_DIRS} ${FMT_HEADERS_DIR} Headers Headers/InfoGen)
target_link_directories(${MAIN_PROGRAM_NAME}Lib PRIVATE ${CSE_LIBRARY_DIRS} ${FMT_LIBRARY_DIR})
target_link_libraries(${MAIN_PROGRAM_NAME}Lib PRIVATE
    CSE_PlanSystem CSE_Object CSE_Parser CSE_MathExtras_Sexagesimal CSE_Base
    Boost::regex Boost::program_options fmt::fmt)

install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/InfoGen_Data/Headers"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/InfoGen_Data/
    FILES_MATCHING PATTERN "*.h"
)
install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/InfoGen_Data/WebResources"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/InfoGen_Data/
)

install(
    TARGETS InfoGenLib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/Libraries
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/InfoGen_Data/Headers/InfoGen
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
