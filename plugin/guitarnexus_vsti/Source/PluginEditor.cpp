#include "PluginEditor.h"

GuitarNexusAudioProcessorEditor::GuitarNexusAudioProcessorEditor(GuitarNexusAudioProcessor& p)
    : juce::AudioProcessorEditor(p), processor(p), browser(p.getPresetDatabase()) {
#if defined(GN_USE_JUCE)
    setSize(960, 520);
    addAndMakeVisible(browser);
    addAndMakeVisible(macroPanel);
#endif
}

#if defined(GN_USE_JUCE)
void GuitarNexusAudioProcessorEditor::paint(juce::Graphics& g) {
    const auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient bgGrad(juce::Colour::fromRGB(18, 24, 31), bounds.getTopLeft(),
                                juce::Colour::fromRGB(34, 46, 60), bounds.getBottomRight(), false);
    bgGrad.addColour(0.35f, juce::Colour::fromRGB(22, 30, 40));
    g.setGradientFill(bgGrad);
    g.fillAll();

    const auto headerHeight = 64;
    auto header = getLocalBounds().removeFromTop(headerHeight).toFloat();
    auto footerLine = header.withHeight(1.0f).translated(0, headerHeight - 1.0f);
    g.setColour(juce::Colour::fromRGBA(255, 255, 255, 24));
    g.fillRect(footerLine);

    juce::Colour accent = juce::Colour::fromRGB(0x66, 0xCC, 0xFF);
    juce::Colour accentDark = juce::Colour::fromRGB(0x23, 0x4D, 0x65);

    auto badge = header.removeFromLeft(220.0f).reduced(12.0f, 16.0f);
    g.setGradientFill(juce::ColourGradient(accent, badge.getTopLeft(), accentDark, badge.getBottomRight(), false));
    g.fillRoundedRectangle(badge, 10.0f);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawFittedText("GuitarNexus", badge.toNearestInt(), juce::Justification::centredLeft, 1);

    g.setFont(juce::Font(13.0f, juce::Font::plain));
    g.setColour(juce::Colour::fromRGB(180, 198, 215));
    auto info = header.reduced(6.0f, 8.0f);
    g.drawFittedText("Modern Nexus-style guitar engine · Factory bank ready", info.toNearestInt(),
                     juce::Justification::centredLeft, 1);

    juce::Rectangle<float> cardArea = getLocalBounds().toFloat().reduced(12.0f);
    cardArea.removeFromTop(static_cast<float>(headerHeight) + 4.0f);
    juce::DropShadow shadow(juce::Colours::black.withAlpha(0.35f), 12, {});
    shadow.drawForRectangle(g, cardArea.toNearestInt());

    g.setColour(juce::Colour::fromRGB(30, 39, 49));
    g.fillRoundedRectangle(cardArea, 12.0f);
    g.setColour(juce::Colour::fromRGBA(255, 255, 255, 32));
    g.drawRoundedRectangle(cardArea, 12.0f, 1.0f);
}

void GuitarNexusAudioProcessorEditor::resized() {
    const int padding = 18;
    const int headerHeight = 64;
    const int macroWidth = 300;
    const int contentHeight = getHeight() - padding * 2 - headerHeight;
    browser.setBounds(padding, padding + headerHeight, getWidth() - macroWidth - padding * 2 - 12, contentHeight);
    macroPanel.setBounds(getWidth() - macroWidth - padding, padding + headerHeight, macroWidth, contentHeight);
}
#endif
