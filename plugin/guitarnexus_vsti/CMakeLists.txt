set(PLUGIN_SOURCES
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/ui/PresetBrowserComponent.cpp
    Source/ui/MacroPanel.cpp
)

set(STUB_SOURCES Source/MinimalJuce.cpp)

if(GUITARNEXUS_ENABLE_JUCE)
    juce_add_plugin(guitarnexus_vsti
        COMPANY_NAME "GuitarNexus"
        VERSION "${PROJECT_VERSION}"
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        IS_MIDI_EFFECT FALSE
        VST3_CATEGORIES Instrument
        FORMATS VST3
        PRODUCT_NAME "GuitarNexus")

    target_sources(guitarnexus_vsti PRIVATE ${PLUGIN_SOURCES})
    target_include_directories(guitarnexus_vsti PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui)
    target_link_libraries(guitarnexus_vsti
        PRIVATE guitarnexus_coredsp guitarnexus_performance guitarnexus_presetengine
                juce::juce_audio_utils juce::juce_audio_plugin_client juce::juce_dsp)
    target_compile_definitions(guitarnexus_vsti PRIVATE GN_USE_JUCE=1 JUCE_DISPLAY_SPLASH_SCREEN=0 JUCE_VST3_CAN_REPLACE_VST2=0)
else()
    add_library(guitarnexus_vsti SHARED ${PLUGIN_SOURCES} ${STUB_SOURCES})

    target_include_directories(guitarnexus_vsti
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui)

    target_link_libraries(guitarnexus_vsti
        PRIVATE guitarnexus_coredsp guitarnexus_performance guitarnexus_presetengine)

    target_compile_definitions(guitarnexus_vsti PRIVATE JUCE_VST3=1 GN_PLUGIN_STUB=1)
endif()
