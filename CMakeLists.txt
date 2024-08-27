cmake_minimum_required(VERSION 3.5)

project(DetailedInfoGen LANGUAGES CXX)

set(MAIN_PROGRAM_NAME "InfoGen")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX})
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/InfoGen_Data)

set(boost_DIR "" CACHE PATH "Boost library config path")
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_DEBUG ON)
find_package(Boost REQUIRED PATHS ${boost_DIR} NO_DEFAULT_PATH COMPONENTS
    program_options regex
)

message("Boost version: ${Boost_VERSION}")
message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message("Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message("Boost_LIBRARIES: ${Boost_LIBRARIES}")

include_directories(${Boost_INCLUDE_DIRS})

set(FMT_FIND_DIRECTORY "" CACHE PATH "fmtlib directory")
find_package(fmt 7.0 REQUIRED PATHS ${FMT_FIND_DIRECTORY} NO_DEFAULT_PATH)

message ("fmtlib configs are found at: ${fmt_DIR}")
message ("fmtlib version: ${fmt_VERSION}")
message ("fmtlib headers at: ${FMT_FIND_DIRECTORY}/include")
message ("fmtlib library at: ${FMT_FIND_DIRECTORY}/lib")

set(FMT_HEADERS_DIR "${FMT_FIND_DIRECTORY}/include" CACHE STRING "fmtlib headers directory" FORCE)
set(FMT_LIBRARY_DIR "${FMT_FIND_DIRECTORY}/lib" CACHE STRING "fmtlib library directory" FORCE)

set(CSE_DIR "" CACHE PATH "CSpaceEngine library path")
set(CSE_INCLUDE_DIRS ${CSE_DIR}/Headers)
set(CSE_LIBRARY_DIRS ${CSE_DIR}/Libraries)

message("CSE_INCLUDE_DIRS: ${CSE_INCLUDE_DIRS}")
message("CSE_LIBRARY_DIRS: ${CSE_LIBRARY_DIRS}")

add_subdirectory(InfoGen_Data)

add_executable(InfoGen InfoGen.cc)
target_include_directories(InfoGen PRIVATE ${CSE_INCLUDE_DIRS})
target_link_directories(InfoGen PRIVATE ${CSE_LIBRARY_DIRS})
target_link_libraries(InfoGen PRIVATE CSE_Base)

include(GNUInstallDirs)
install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/IGOutputData"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/
)
install(TARGETS ${MAIN_PROGRAM_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
