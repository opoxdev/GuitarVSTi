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
#if defined(GN_USE_JUCE)
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    void releaseResources() override;
    bool hasEditor() const override;
    juce::AudioProcessorEditor* createEditor() override;
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

private:
    static constexpr int kMaxVoices = 8;
    std::array<gn::CoreVoice, kMaxVoices> voices;
    gnperf::PerformanceEngine performance;
    gnpreset::PresetDatabase presetDb;
    gnpreset::ParamSnapshot currentSnapshot;
    double sampleRate{48000.0};
    std::vector<float> renderScratch;
};
