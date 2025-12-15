add_library(guitarnexus_coredsp STATIC
    src/SmoothValue.cpp
    src/WaveguideString.cpp
    src/PickExciter.cpp
    src/BodyResonator.cpp
    src/CoreVoice.cpp
    src/DspUtilities.cpp
)

target_include_directories(guitarnexus_coredsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(guitarnexus_coredsp
    PUBLIC GN_CORE_DSP=1
    PRIVATE JUCE_DISABLE_ASSERTIONS=1
)

target_compile_options(guitarnexus_coredsp PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

target_link_libraries(guitarnexus_coredsp PUBLIC)
