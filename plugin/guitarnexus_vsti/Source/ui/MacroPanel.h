#pragma once

#include "MinimalJuce.h"
#include <array>
#include <string>

class MacroPanel : public juce::Component {
public:
    MacroPanel();
    const std::array<std::string, 8>& getMacroLabels() const { return labels; }

private:
    std::array<std::string, 8> labels;
};
