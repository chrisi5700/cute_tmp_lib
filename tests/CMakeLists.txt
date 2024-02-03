# Set the minimum required version of CMake for this project.
cmake_minimum_required(VERSION 3.22)

# Define the test executable
add_executable(TMP_Lib_tests)

# Add your test source files here
target_sources(TMP_Lib_tests PRIVATE
        type_bag_tests.cpp
        # ... add more test files as needed
)

# Link your library to the test executable
target_link_libraries(TMP_Lib_tests PRIVATE TMP_Lib)

# Set the C++ standard to C++23 for the test target
target_compile_features(TMP_Lib_tests PRIVATE cxx_std_23)

# Include directories for the test target
target_include_directories(TMP_Lib_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)