#pragma once

#include "MinimalJuce.h"
#include "PluginProcessor.h"
#include "ui/PresetBrowserComponent.h"
#include "ui/MacroPanel.h"
#include <memory>
#include <vector>

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
#if defined(GN_USE_JUCE)
    juce::ToggleButton strumEnable;
    juce::ComboBox strumDirection;
    juce::Slider strumSpread;
    juce::Slider strumHumanize;
    juce::Slider strumVelCurve;
    juce::ComboBox legatoMode;
    juce::Slider legatoWindow;
    juce::Slider slideTime;
    juce::Slider noiseAmount;
    juce::ToggleButton pickNoise;
    juce::ToggleButton squeak;
    juce::ToggleButton thump;
    std::vector<std::unique_ptr<juce::Component>> controls;
    void buildControls();
#endif
};
