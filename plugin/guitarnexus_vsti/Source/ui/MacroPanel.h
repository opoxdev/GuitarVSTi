#pragma once

#include "MinimalJuce.h"
#include <array>
#include <string>

class MacroPanel : public juce::Component {
public:
    MacroPanel();
    const std::array<std::string, 8>& getMacroLabels() const { return labels; }

#if defined(GN_USE_JUCE)
    void paint(juce::Graphics& g) override;
    void resized() override;
#endif

private:
    std::array<std::string, 8> labels;
};
