cmake_minimum_required(VERSION 3.20)
project(OpenClaw-VST-Plugin VERSION 0.1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE Root (set via environment or command line)
if(NOT DEFINED JUCE_ROOT)
    if(DEFINED ENV{JUCE_ROOT})
        set(JUCE_ROOT $ENV{JUCE_ROOT})
    else()
        set(JUCE_ROOT "/home/carlo/SDKs/JUCE")
    endif()
endif()

message(STATUS "JUCE_ROOT: ${JUCE_ROOT}")

# Check JUCE exists
if(NOT EXISTS ${JUCE_ROOT}/CMakeLists.txt)
    message(FATAL_ERROR "JUCE not found at ${JUCE_ROOT}. Set JUCE_ROOT environment variable or pass -DJUCE_ROOT=/path/to/JUCE")
endif()

# Add JUCE
add_subdirectory(${JUCE_ROOT} JUCE)

# External dependencies
set(OSCPACK_ROOT "/home/carlo/SDKs/oscpack" CACHE PATH "oscpack root directory")
set(NLOHMANN_JSON_ROOT "/home/carlo/SDKs/nlohmann_json" CACHE PATH "nlohmann_json root directory")

# oscpack static library
add_library(oscpack STATIC
    ${OSCPACK_ROOT}/osc/OscOutboundPacketStream.cpp
    ${OSCPACK_ROOT}/osc/OscReceivedElements.cpp
    ${OSCPACK_ROOT}/osc/OscTypes.cpp
    ${OSCPACK_ROOT}/osc/OscPrintReceivedElements.cpp
    ${OSCPACK_ROOT}/ip/IpEndpointName.cpp
    ${OSCPACK_ROOT}/ip/posix/NetworkingUtils.cpp
    ${OSCPACK_ROOT}/ip/posix/UdpSocket.cpp
)
target_include_directories(oscpack PUBLIC ${OSCPACK_ROOT})

# nlohmann_json (header-only)
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${NLOHMANN_JSON_ROOT}/single_include)

# Plugin Definition
juce_add_plugin(OpenClawVSTPlugin
    COMPANY_NAME "OpenClaw"
    PLUGIN_MANUFACTURER_CODE OpCl
    PLUGIN_CODE OcAI
    
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    
    FORMATS VST3 AU Standalone
    
    PRODUCT_NAME "OpenClaw VST Bridge AI"
    
    COPY_PLUGIN_AFTER_BUILD FALSE
)

# Source files
target_sources(OpenClawVSTPlugin
    PRIVATE
        src/core/PluginProcessor.cpp
        src/core/PluginProcessor.h
        src/core/PluginEditor.cpp
        src/core/PluginEditor.h
)

# Link libraries
target_link_libraries(OpenClawVSTPlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        oscpack
        nlohmann_json
)

# Include directories
target_include_directories(OpenClawVSTPlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/osc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(OpenClawVSTPlugin PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Print configuration
message(STATUS "")
message(STATUS "=== OpenClaw VST Plugin Configuration ===")
message(STATUS "JUCE_ROOT: ${JUCE_ROOT}")
message(STATUS "OSCPACK_ROOT: ${OSCPACK_ROOT}")
message(STATUS "NLOHMANN_JSON_ROOT: ${NLOHMANN_JSON_ROOT}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "==========================================")
message(STATUS "")