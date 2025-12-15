#include "PluginEditor.h"

GuitarNexusAudioProcessorEditor::GuitarNexusAudioProcessorEditor(GuitarNexusAudioProcessor& p)
    : juce::AudioProcessorEditor(p), processor(p), browser(p.getPresetDatabase()) {
#if defined(GN_USE_JUCE)
    setSize(820, 420);
    addAndMakeVisible(browser);
    addAndMakeVisible(macroPanel);
#endif
}

#if defined(GN_USE_JUCE)
void GuitarNexusAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkslategrey);
    g.setColour(juce::Colours::white);
    g.drawText("GuitarNexus VSTi", 10, 10, getWidth() - 20, 24, juce::Justification::centred);
}

void GuitarNexusAudioProcessorEditor::resized() {
    const int padding = 12;
    const int headerHeight = 32;
    const int macroWidth = 260;
    const int contentHeight = getHeight() - padding * 2 - headerHeight;
    browser.setBounds(padding, padding + headerHeight, getWidth() - macroWidth - padding * 2 - 8, contentHeight);
    macroPanel.setBounds(getWidth() - macroWidth - padding, padding + headerHeight, macroWidth, contentHeight);
}
#endif
