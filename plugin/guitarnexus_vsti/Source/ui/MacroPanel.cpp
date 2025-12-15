#include "MacroPanel.h"

MacroPanel::MacroPanel() {
    labels = {"Bite", "Tone", "Drive", "Space", "Motion", "Damp", "Width", "LoFi"};
}

#if defined(GN_USE_JUCE)
void MacroPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkslategrey);
    g.setColour(juce::Colours::white);
    const auto labelString = std::string("Macros: ") + labels[0] + ", " + labels[1] + ", " + labels[2] +
                              ", " + labels[3] + ", " + labels[4] + ", " + labels[5] + ", " + labels[6] +
                              ", " + labels[7];
    g.drawText(labelString, 8, 8, getWidth() - 16, 24, juce::Justification::centred);
}

void MacroPanel::resized() {}
#endif
