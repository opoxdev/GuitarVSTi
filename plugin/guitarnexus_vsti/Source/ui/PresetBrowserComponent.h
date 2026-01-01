#pragma once

#include "MinimalJuce.h"
#include "gnpreset/PresetEngine.h"
#include <vector>
#include <string>

class PresetBrowserComponent : public juce::Component {
public:
    explicit PresetBrowserComponent(gnpreset::PresetDatabase& db);
    std::vector<std::string> listPresetNames() const;

#if defined(GN_USE_JUCE)
    void paint(juce::Graphics& g) override;
    void resized() override;
#endif

private:
    gnpreset::PresetDatabase& database;
};
