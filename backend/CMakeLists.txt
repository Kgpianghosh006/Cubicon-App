cmake_minimum_required(VERSION 3.20)
project(rubiks_cube_solver)

set(CMAKE_CXX_STANDARD 17)

# Core Library
add_library(rubiks_core
    Model/RubiksCube.cpp
    Model/RubiksCube1dArray.cpp
    Model/RubiksCube3dArray.cpp
    Model/RubiksCubeBitboard.cpp
    PatternDatabases/CornerDBMaker.cpp
    PatternDatabases/CornerPatternDatabase.cpp
    PatternDatabases/NibbleArray.cpp
    PatternDatabases/PatternDatabase.cpp
    PatternDatabases/math.cpp
)

target_include_directories(rubiks_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Model
    ${CMAKE_CURRENT_SOURCE_DIR}/PatternDatabases
    ${CMAKE_CURRENT_SOURCE_DIR}/Solver
)

# REST API Server
add_executable(rubiks_server
    server/main.cpp
)

target_include_directories(rubiks_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/server
)

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
target_link_libraries(rubiks_server PRIVATE rubiks_core Threads::Threads ${OpenCV_LIBS})

if(WIN32)
    target_link_libraries(rubiks_server PRIVATE ws2_32)
endif()