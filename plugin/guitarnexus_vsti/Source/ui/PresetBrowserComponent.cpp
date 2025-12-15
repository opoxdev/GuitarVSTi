#include "PresetBrowserComponent.h"

PresetBrowserComponent::PresetBrowserComponent(gnpreset::PresetDatabase& db) : database(db) {}

std::vector<std::string> PresetBrowserComponent::listPresetNames() const {
    std::vector<std::string> names;
    for (const auto& preset : database.getPresets()) {
        names.push_back(preset.meta.name);
    }
    return names;
}

#if defined(GN_USE_JUCE)
void PresetBrowserComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    const auto names = listPresetNames();
    std::string combined;
    for (size_t i = 0; i < names.size(); ++i) {
        combined += names[i];
        if (i + 1 < names.size()) combined += ", ";
    }
    g.drawText(combined.empty() ? "Factory presets loaded" : combined, 8, 8, getWidth() - 16, getHeight() - 16,
               juce::Justification::centred);
}

void PresetBrowserComponent::resized() {}
#endif
