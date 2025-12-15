set(PLUGIN_SOURCES
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/ui/PresetBrowserComponent.cpp
    Source/ui/MacroPanel.cpp
    Source/MinimalJuce.cpp
)

add_library(guitarnexus_vsti SHARED ${PLUGIN_SOURCES})

target_include_directories(guitarnexus_vsti
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui)

target_link_libraries(guitarnexus_vsti
    PRIVATE guitarnexus_coredsp guitarnexus_performance guitarnexus_presetengine)

target_compile_definitions(guitarnexus_vsti PRIVATE JUCE_VST3=1 GN_PLUGIN_STUB=1)

if(GUITARNEXUS_ENABLE_JUCE)
    target_link_libraries(guitarnexus_vsti PRIVATE juce::juce_audio_utils juce::juce_audio_plugin_client juce::juce_dsp)
    target_compile_definitions(guitarnexus_vsti PRIVATE JUCE_PLUGIN_NAME_STRING="GuitarNexus")
endif()
