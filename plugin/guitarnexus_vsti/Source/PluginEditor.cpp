#include "PluginEditor.h"

GuitarNexusAudioProcessorEditor::GuitarNexusAudioProcessorEditor(GuitarNexusAudioProcessor& p)
    : juce::AudioProcessorEditor(p), processor(p), browser(p.getPresetDatabase()) {
#if defined(GN_USE_JUCE)
    setSize(640, 360);
#endif
}
