cmake_minimum_required(VERSION 3.29)
project(pathfinder LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Include FetchContent to manage external dependencies
include(FetchContent)

# Fetch SFML library
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

# Fetch nlohmann/json library
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2) # Replace with the latest stable version if needed
FetchContent_MakeAvailable(nlohmann_json)

# Add all source files in the Source directory
file(GLOB_RECURSE SOURCES "Source/*.cpp")

# Create the executable
add_executable(pathfinder ${SOURCES})

# Link libraries to the executable
target_link_libraries(pathfinder PRIVATE sfml-graphics nlohmann_json::nlohmann_json)
target_compile_features(pathfinder PRIVATE cxx_std_17)

# Copy resource files to the output directory
file(COPY ${CMAKE_SOURCE_DIR}/Source/Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if(WIN32)
    # Special handling for OpenAL DLL on Windows
    add_custom_command(
        TARGET pathfinder
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy 
        ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll 
        $<TARGET_FILE_DIR:pathfinder>
        VERBATIM)
endif()

# Install the executable
install(TARGETS pathfinder)