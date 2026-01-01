add_executable(presetgen main.cpp)

target_link_libraries(presetgen PRIVATE guitarnexus_presetengine)

target_compile_options(presetgen PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
