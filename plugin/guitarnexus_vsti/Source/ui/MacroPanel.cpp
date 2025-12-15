#include "MacroPanel.h"

MacroPanel::MacroPanel() {
    labels = {"Bite", "Tone", "Drive", "Space", "Motion", "Damp", "Width", "LoFi"};
}

#if defined(GN_USE_JUCE)
void MacroPanel::paint(juce::Graphics& g) {
    juce::Rectangle<float> area = getLocalBounds().toFloat().reduced(12.0f);
    juce::ColourGradient grad(juce::Colour::fromRGB(37, 49, 61), area.getTopLeft(),
                             juce::Colour::fromRGB(27, 35, 43), area.getBottomRight(), false);
    grad.addColour(0.5f, juce::Colour::fromRGB(33, 42, 52));
    g.setGradientFill(grad);
    g.fillRoundedRectangle(area, 10.0f);

    g.setColour(juce::Colour::fromRGBA(255, 255, 255, 32));
    g.drawRoundedRectangle(area, 10.0f, 1.0f);

    g.setColour(juce::Colour::fromRGB(0x66, 0xCC, 0xFF));
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.drawFittedText("Performance Macros", area.reduced(12).toNearestInt(), juce::Justification::centredTop, 1);

    auto grid = area.reduced(14.0f);
    grid.removeFromTop(24.0f);
    const int columns = 2;
    const int rows = 4;
    const float cellW = grid.getWidth() / static_cast<float>(columns);
    const float cellH = grid.getHeight() / static_cast<float>(rows);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            const int idx = r * columns + c;
            juce::Rectangle<float> cell(grid.getX() + c * cellW + 8.0f, grid.getY() + r * cellH + 8.0f,
                                        cellW - 16.0f, cellH - 16.0f);
            juce::Colour cellBg = juce::Colour::fromRGB(44, 56, 69);
            g.setColour(cellBg); g.fillRoundedRectangle(cell, 8.0f);
            g.setColour(juce::Colour::fromRGBA(255, 255, 255, 24));
            g.drawRoundedRectangle(cell, 8.0f, 1.0f);

            auto knob = cell.withSizeKeepingCentre(44.0f, 44.0f);
            juce::Colour knobFill = juce::Colour::fromRGB(84, 180, 220);
            g.setColour(knobFill.darker(0.35f));
            g.fillEllipse(knob);
            g.setColour(knobFill); g.drawEllipse(knob, 2.0f);

            g.setColour(juce::Colour::fromRGB(210, 224, 236));
            g.setFont(juce::Font(14.0f, juce::Font::plain));
            juce::Rectangle<float> textArea = cell.removeFromBottom(22.0f);
            g.drawFittedText(labels[idx], textArea.toNearestInt(), juce::Justification::centred, 1);
        }
    }
}

void MacroPanel::resized() {}
#endif
