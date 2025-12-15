#pragma once

#include "MinimalJuce.h"
#include "gn/CoreVoice.h"
#include "gnperf/PerformanceEngine.h"
#include "gnpreset/PresetEngine.h"
#include <array>
#include <vector>

#if defined(GN_USE_JUCE)
using GnAudioBuffer = juce::AudioBuffer<float>;
#else
using GnAudioBuffer = juce::AudioBuffer;
#endif
using GnMidiBuffer = juce::MidiBuffer;

class GuitarNexusAudioProcessor : public juce::AudioProcessor {
public:
    GuitarNexusAudioProcessor();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(GnAudioBuffer& buffer, GnMidiBuffer& midiMessages) override;
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
