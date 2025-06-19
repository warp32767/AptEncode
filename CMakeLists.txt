cmake_minimum_required(VERSION 3.10)
project(AptEncode)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add math library define for Windows
if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif()

# Add executable
add_executable(AptEncode main.cpp
        WavAptEncoder.cpp
        WavAptEncoder.h
)

# Find and link required libraries
if(WIN32)
    # For Windows, ensure static linking of the standard libraries
    if(MINGW)
        target_link_options(AptEncode PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
endif()

# Install targets
install(TARGETS AptEncode DESTINATION bin)

