#include "PluginEditor.h"

GuitarNexusAudioProcessorEditor::GuitarNexusAudioProcessorEditor(GuitarNexusAudioProcessor& p)
    : processor(p), browser(p.getPresetDatabase()) {}
