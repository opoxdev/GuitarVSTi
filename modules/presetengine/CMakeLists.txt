add_library(guitarnexus_presetengine STATIC
    src/PresetEngine.cpp
    src/Json.cpp
)

target_include_directories(guitarnexus_presetengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(guitarnexus_presetengine PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

