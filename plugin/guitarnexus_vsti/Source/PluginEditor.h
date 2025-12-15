#pragma once

#include "MinimalJuce.h"
#include "PluginProcessor.h"
#include "ui/PresetBrowserComponent.h"
#include "ui/MacroPanel.h"

class GuitarNexusAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    explicit GuitarNexusAudioProcessorEditor(GuitarNexusAudioProcessor& p);
    GuitarNexusAudioProcessorEditor(const GuitarNexusAudioProcessorEditor&) = delete;
    GuitarNexusAudioProcessorEditor& operator=(const GuitarNexusAudioProcessorEditor&) = delete;

#if defined(GN_USE_JUCE)
    void paint(juce::Graphics& g) override;
    void resized() override;
#endif

private:
    GuitarNexusAudioProcessor& processor;
    PresetBrowserComponent browser;
    MacroPanel macroPanel;
};
