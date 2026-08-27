add_executable(rubiks_server
    main.cpp
)

target_include_directories(rubiks_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/backend
    ${CMAKE_SOURCE_DIR}/backend/Model
    ${CMAKE_SOURCE_DIR}/backend/Solver
    ${CMAKE_SOURCE_DIR}/backend/PatternDatabases
)

# Enable threads (required for cpp-httplib)
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
target_link_libraries(rubiks_server PRIVATE rubiks_core Threads::Threads ${OpenCV_LIBS})

# Add compile flag for WIN32 (cpp-httplib needs to know it's on Windows, unless automatically detected)
if(WIN32)
    target_link_libraries(rubiks_server PRIVATE ws2_32)
endif()