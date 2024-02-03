# Set the minimum required version of CMake for this project.
cmake_minimum_required(VERSION 3.22)

# Declare the project and specify the languages used.
project(TMP_Lib VERSION 1.0.0 LANGUAGES CXX)

# Add the header-only library.
add_library(TMP_Lib INTERFACE)

# Target specific include directories.
target_include_directories(TMP_Lib INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Set the C++ standard to C++23 for this target
target_compile_features(TMP_Lib INTERFACE cxx_std_23)


# If you want to install your library (optional)
install(TARGETS TMP_Lib EXPORT TMP_LibTargets
        INCLUDES DESTINATION include
)

install(EXPORT TMP_LibTargets
        FILE TMP_LibTargets.cmake
        NAMESPACE TMP_Lib::
        DESTINATION lib/cmake/TMP_Lib
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("TMP_LibConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES "TMP_LibConfig.cmake" "TMP_LibConfigVersion.cmake"
        DESTINATION lib/cmake/TMP_Lib
)

# This is for the library consumers to find the headers.
install(DIRECTORY include/ DESTINATION include)

# Export the targets (optional, but useful for other CMake projects to use this project)
export(EXPORT TMP_LibTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/TMP_LibTargets.cmake"
        NAMESPACE TMP_Lib::
)


# Enable testing functionality
enable_testing()

# Add the test subdirectory
add_subdirectory(tests)