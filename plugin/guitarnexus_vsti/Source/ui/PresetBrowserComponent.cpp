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
    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient grad(juce::Colour::fromRGB(36, 43, 50), bounds.getTopLeft(),
                             juce::Colour::fromRGB(28, 32, 38), bounds.getBottomRight(), false);
    grad.addColour(0.6f, juce::Colour::fromRGB(26, 31, 37));
    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds.reduced(6.0f), 12.0f);

    g.setColour(juce::Colour::fromRGBA(255, 255, 255, 28));
    g.drawRoundedRectangle(bounds.reduced(6.0f), 12.0f, 1.0f);

    auto inner = bounds.reduced(18.0f);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText("Preset Browser", inner.removeFromTop(26.0f).toNearestInt(), juce::Justification::centredLeft);

    g.setColour(juce::Colour::fromRGB(78, 96, 116));
    auto searchBar = inner.removeFromTop(30.0f).withHeight(32.0f);
    juce::Rectangle<float> searchBox = searchBar.reduced(2.0f, -2.0f);
    g.fillRoundedRectangle(searchBox, 6.0f);
    g.setColour(juce::Colour::fromRGB(180, 198, 215));
    g.setFont(juce::Font(14.0f));
    g.drawFittedText("Search or browse tags", searchBox.toNearestInt().reduced(10, 4), juce::Justification::centredLeft, 1);

    g.setColour(juce::Colour::fromRGBA(255, 255, 255, 14));
    g.fillRect(inner.removeFromTop(1.0f));

    const auto names = listPresetNames();
    auto listArea = inner.reduced(2.0f, 10.0f);
    const int rows = 6;
    const float rowHeight = listArea.getHeight() / rows;
    juce::Colour rowBgA = juce::Colour::fromRGB(40, 48, 58);
    juce::Colour rowBgB = juce::Colour::fromRGB(46, 56, 68);
    for (int i = 0; i < rows; ++i) {
        juce::Rectangle<float> row(listArea.getX(), listArea.getY() + i * rowHeight, listArea.getWidth(), rowHeight - 4.0f);
        g.setColour((i % 2 == 0) ? rowBgA : rowBgB);
        g.fillRoundedRectangle(row, 6.0f);
        g.setColour(juce::Colour::fromRGB(210, 224, 236));
        const std::string label = i < static_cast<int>(names.size()) ? names[static_cast<std::size_t>(i)]
                                                                   : ("Factory preset " + std::to_string(i + 1));
        g.drawFittedText(label, row.toNearestInt().reduced(10, 6), juce::Justification::centredLeft, 1);
    }
}

void PresetBrowserComponent::resized() {}
#endif
