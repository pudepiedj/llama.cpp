# set(TARGETS cmap-example read-dir)
# add_executable(${TARGETS} cmap-example.cpp read-dir.cpp)
# install(TARGETS ${TARGETS} RUNTIME)
# target_link_libraries(${TARGETS} PRIVATE common llama ${CMAKE_THREAD_LIBS_INIT})
# target_compile_features(${TARGETS} PRIVATE cxx_std_11)

cmake_minimum_required(VERSION 3.27)
project(ParallelLlama)

# Define the source files for the targets
set(SOURCE_FILES cmap-example.cpp read-dir.cpp cfindimplementedargs.cpp test-file.cpp)

# Define the targets using the source files
add_executable(cmap-example cmap-example.cpp)
add_executable(read-dir read-dir.cpp)
add_executable(cfindimplementedargs cfindimplementedargs.cpp)
add_executable(test-file test-file.cpp)

# Set the desired output executable names
# set_target_properties(cmap-example read-dir PROPERTIES OUTPUT_NAME "${TARGET_SUFFIX}")
# get as much information as possible about the unreachable code or whatever
# set(CMAKE_CXX_FLAGS "-Weverything")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# don't warn about incompatibility before C++17
set(CMAKE_CXX_STANDARD 11)

# Install the runtime
install(TARGETS cmap-example read-dir cfindimplementedargs test-file RUNTIME DESTINATION bin)

# Link the libraries
target_link_libraries(cmap-example PRIVATE common llama ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(read-dir PRIVATE common llama ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(cfindimplementedargs PRIVATE common llama ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(test-file PRIVATE common llama ${CMAKE_THREAD_LIBS_INIT})

# Specify the C++ standard note here changed from 11 to 17
target_compile_features(cmap-example PRIVATE cxx_std_11)
target_compile_features(read-dir PRIVATE cxx_std_11)
target_compile_features(cfindimplementedargs PRIVATE cxx_std_11)
target_compile_features(test-file PRIVATE cxx_std_11)

# Add a dependency
add_dependencies(cmap-example BUILD_INFO)
add_dependencies(read-dir BUILD_INFO)
add_dependencies(cfindimplementedargs BUILD_INFO)
add_dependencies(test-file BUILD_INFO)
