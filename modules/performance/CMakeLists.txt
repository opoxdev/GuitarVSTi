add_library(guitarnexus_performance STATIC
    src/PerformanceEngine.cpp
)

target_include_directories(guitarnexus_performance PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(guitarnexus_performance PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

