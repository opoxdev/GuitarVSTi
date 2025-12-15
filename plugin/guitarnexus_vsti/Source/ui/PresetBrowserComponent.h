#pragma once

#include "MinimalJuce.h"
#include "gnpreset/PresetEngine.h"
#include <vector>
#include <string>

class PresetBrowserComponent : public juce::Component {
public:
    explicit PresetBrowserComponent(gnpreset::PresetDatabase& db);
    std::vector<std::string> listPresetNames() const;

private:
    gnpreset::PresetDatabase& database;
};
