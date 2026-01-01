#include "PluginEditor.h"

GuitarNexusAudioProcessorEditor::GuitarNexusAudioProcessorEditor(GuitarNexusAudioProcessor& p)
    : juce::AudioProcessorEditor(p), processor(p), browser(p.getPresetDatabase()) {
#if defined(GN_USE_JUCE)
    setSize(960, 520);
    addAndMakeVisible(browser);
    addAndMakeVisible(macroPanel);
    buildControls();
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

    auto controlArea = macroPanel.getBounds().reduced(12);
    int y = controlArea.getY() + 80;
    const int rowH = 30;
    const int labelW = 140;
    auto placeRow = [&](juce::Component& comp) {
        comp.setBounds(controlArea.getX() + labelW, y, controlArea.getWidth() - labelW, rowH);
        y += rowH + 6;
    };
    placeRow(strumEnable);
    placeRow(strumDirection);
    placeRow(strumSpread);
    placeRow(strumHumanize);
    placeRow(strumVelCurve);
    placeRow(legatoMode);
    placeRow(legatoWindow);
    placeRow(slideTime);
    placeRow(noiseAmount);
    placeRow(pickNoise);
    placeRow(squeak);
    placeRow(thump);
}
#endif

#if defined(GN_USE_JUCE)
void GuitarNexusAudioProcessorEditor::buildControls() {
    auto configureSlider = [&](juce::Slider& slider, const std::string& id, double min, double max, double init, double step) {
        slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 64, 18);
        slider.setRange(min, max, step);
        slider.setValue(init);
        slider.onValueChange = [this, &slider, id]() { processor.setParameter(id, static_cast<float>(slider.getValue())); };
        addAndMakeVisible(slider);
    };

    auto configureToggle = [&](juce::ToggleButton& toggle, const std::string& id, bool init, const juce::String& label) {
        toggle.setButtonText(label);
        toggle.setToggleState(init, juce::dontSendNotification);
        toggle.onClick = [this, &toggle, id]() { processor.setParameter(id, toggle.getToggleState() ? 1.0f : 0.0f); };
        addAndMakeVisible(toggle);
    };

    configureToggle(strumEnable, "strumEnable", false, "Strum Enable");
    strumDirection.addItem("Down", 1);
    strumDirection.addItem("Up", 2);
    strumDirection.addItem("Alternate", 3);
    strumDirection.addItem("Random", 4);
    strumDirection.setSelectedId(1);
    strumDirection.onChange = [this]() { processor.setParameter("strumDirection", static_cast<float>(strumDirection.getSelectedId() - 1)); };
    addAndMakeVisible(strumDirection);

    configureSlider(strumSpread, "strumSpreadMs", 0.0, 60.0, 16.0, 0.5);
    configureSlider(strumHumanize, "strumHumanizeMs", 0.0, 12.0, 2.0, 0.25);
    configureSlider(strumVelCurve, "strumVelCurve", -0.6, 0.6, 0.15, 0.01);

    legatoMode.addItem("Off", 1);
    legatoMode.addItem("Auto", 2);
    legatoMode.addItem("Force", 3);
    legatoMode.setSelectedId(2);
    legatoMode.onChange = [this]() { processor.setParameter("legatoMode", static_cast<float>(legatoMode.getSelectedId() - 1)); };
    addAndMakeVisible(legatoMode);

    configureSlider(legatoWindow, "legatoWindowMs", 0.0, 120.0, 40.0, 1.0);
    configureSlider(slideTime, "slideTimeMs", 0.0, 180.0, 45.0, 1.0);
    configureSlider(noiseAmount, "noiseAmount", 0.0, 1.0, 0.25, 0.01);
    configureToggle(pickNoise, "pickNoiseEnable", true, "Pick noise");
    configureToggle(squeak, "squeakEnable", true, "Fret squeak");
    configureToggle(thump, "muteThumpEnable", true, "Mute thump");
}
#endif
