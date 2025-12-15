#include "PresetBrowserComponent.h"

PresetBrowserComponent::PresetBrowserComponent(gnpreset::PresetDatabase& db) : database(db) {}

std::vector<std::string> PresetBrowserComponent::listPresetNames() const {
    std::vector<std::string> names;
    for (const auto& preset : database.getPresets()) {
        names.push_back(preset.meta.name);
    }
    return names;
}
