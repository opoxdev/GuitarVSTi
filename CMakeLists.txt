cmake_minimum_required(VERSION 3.20)
project(GuitarNexus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(GUITARNEXUS_ENABLE_JUCE "Enable JUCE-based plugin build" OFF)
option(GUITARNEXUS_BUILD_DEMO "Build offline render demo" ON)
option(GUITARNEXUS_USE_FETCHCONTENT_JUCE "Download JUCE with FetchContent when JUCE_DIR is not provided" OFF)
set(JUCE_DIR "" CACHE PATH "Path to a local JUCE checkout (root directory containing CMakeLists.txt)")

if(GUITARNEXUS_ENABLE_JUCE)
    if(JUCE_DIR AND EXISTS "${JUCE_DIR}/CMakeLists.txt")
        add_subdirectory("${JUCE_DIR}" ${CMAKE_BINARY_DIR}/juce)
    elseif(GUITARNEXUS_USE_FETCHCONTENT_JUCE)
        include(FetchContent)
        FetchContent_Declare(
            juce
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG 7.0.12
        )
        set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(juce)
    else()
        message(FATAL_ERROR "GUITARNEXUS_ENABLE_JUCE=ON but JUCE_DIR is not set. Provide JUCE_DIR (e.g. third_party/juce) or enable GUITARNEXUS_USE_FETCHCONTENT_JUCE to download JUCE.")
    endif()
endif()

add_subdirectory(modules/coredsp)
add_subdirectory(modules/performance)
add_subdirectory(modules/presetengine)
add_subdirectory(plugin/guitarnexus_vsti)
add_subdirectory(tools/presetgen)
add_subdirectory(tools/bankpacker)

if(GUITARNEXUS_BUILD_DEMO)
    add_executable(guitarnexus_offline_demo tools/offline_demo.cpp)
    target_link_libraries(guitarnexus_offline_demo PRIVATE guitarnexus_coredsp guitarnexus_performance guitarnexus_presetengine)
    target_compile_definitions(guitarnexus_offline_demo PRIVATE GN_HEADLESS_DEMO=1)
endif()
