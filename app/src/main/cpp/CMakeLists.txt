
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("cameraxapp")

# opencv
set(OpenCV_STATIC ON)
set(OpenCV_DIR $ENV{OPENCV_ANDROID}/sdk/native/jni)
find_package(OpenCV REQUIRED)

# Include directories for headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find all source files
file(GLOB_RECURSE CPP_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
set(CPP_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/data_read.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/img_proc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ook.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/csk.cpp
)

# Add your main source files
set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/cameraxapp.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
        ${CPP_SOURCES}
)

add_library(${CMAKE_PROJECT_NAME}
    SHARED
    ${SOURCES}
)

# jnigraphics lib from NDK is used for Bitmap manipulation in native code
find_library(jnigraphics-lib jnigraphics)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
    # List libraries link to the target library
    android
    ${OpenCV_LIBS}
    ${jnigraphics-lib}
    log)
