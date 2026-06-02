# This CMake file was made with the help of LowLevelGameDev on youtube.
# https://www.youtube.com/watch?v=IBgfeZME2Vw

# No particular reason for this version. Just what the tutorial used.
cmake_minimum_required(VERSION 3.13)

# The tutorial I followed recommended to just include these few lines.
# This is how we tell CMAKE to link the C++ runtime statically.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

#add_subdirectory("third_party")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/")

project(main)

# Set the C++ standard.
set(CMAKE_CXX_STANDARD 17)

# Define MY_SOURCES to be a list of all the source files used in this project.
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
# Define MY_LIBRARIES to be a list of all the library files used in this project.
file(GLOB_RECURSE MY_LIBRARIES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.a")

# Add all source files to main project.
add_executable(main "${MY_SOURCES}")

# These two lines were also recommended by the tutorial I followed.
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/") # This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line
#target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/") # Uncomment this line to setup the ASSETS_PATH macro to the final assets directory when you share the project

# Lets CMake know where to find the header and library files.
target_include_directories(main PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories(main PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/")

# Link the libraries I put in the project.
target_link_libraries(main ${MY_LIBRARIES})
# Also link requirements for Raylib.
target_link_libraries(main
    winmm
    gdi32
    opengl32
)