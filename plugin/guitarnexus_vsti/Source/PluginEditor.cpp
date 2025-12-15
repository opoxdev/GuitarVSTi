#include "PluginEditor.h"

GuitarNexusAudioProcessorEditor::GuitarNexusAudioProcessorEditor(GuitarNexusAudioProcessor& p)
#if defined(GN_USE_JUCE)
    : juce::AudioProcessorEditor(p), processor(p), browser(p.getPresetDatabase())
#else
    : processor(p), browser(p.getPresetDatabase())
#endif
{
#if defined(GN_USE_JUCE)
    setSize(640, 360);
#endif
}
