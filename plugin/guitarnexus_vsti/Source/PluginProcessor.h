#pragma once

#include "MinimalJuce.h"
#include "gn/CoreVoice.h"
#include "gnperf/PerformanceEngine.h"
#include "gnpreset/PresetEngine.h"
#include <array>
#include <vector>

class GuitarNexusAudioProcessor : public juce::AudioProcessor {
public:
    GuitarNexusAudioProcessor();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer& buffer, juce::MidiBuffer& midiMessages) override;
    gnpreset::PresetDatabase& getPresetDatabase() { return presetDb; }

private:
    static constexpr int kMaxVoices = 8;
    std::array<gn::CoreVoice, kMaxVoices> voices;
    gnperf::PerformanceEngine performance;
    gnpreset::PresetDatabase presetDb;
    gnpreset::ParamSnapshot currentSnapshot;
    double sampleRate{48000.0};
    std::vector<float> renderScratch;
};
