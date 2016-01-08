set (APP MUSI8903Exec)

## get all source files (note: this is considered bad style, one could also add file names individually)
file(GLOB ${APP}_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/src/${APP} *.c;*.cpp;*.txt)
file(GLOB ${APP}_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/src/${APP} *.h;../inc/*.h)

## add include directories
include_directories(${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/src/inc)

## create the target
add_executable(${APP} ${${APP}_SOURCES} ${${APP}_HEADERS})

