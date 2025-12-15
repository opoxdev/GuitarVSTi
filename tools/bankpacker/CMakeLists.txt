add_executable(bankpacker main.cpp)

target_link_libraries(bankpacker PRIVATE guitarnexus_presetengine)

target_compile_options(bankpacker PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
