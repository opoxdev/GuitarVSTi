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

private:
    GuitarNexusAudioProcessor& processor;
    PresetBrowserComponent browser;
    MacroPanel macroPanel;
};
