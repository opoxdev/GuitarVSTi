#pragma once

#include "MinimalJuce.h"
#include "gn/CoreVoice.h"
#include "gn/NoiseLayer.h"
#include "gnperf/PerformanceEngine.h"
#include "gnpreset/PresetEngine.h"
#include <array>
#include <vector>
#include <atomic>

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
    void setParameter(const std::string& id, float value);
    float getParameter(const std::string& id) const;
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
    gn::NoiseLayer noiseLayer;
    gnpreset::PresetDatabase presetDb;
    gnpreset::ParamSnapshot currentSnapshot;
    double sampleRate{48000.0};
    std::vector<float> renderScratch;
    std::vector<float> noiseScratch;
    int64_t sampleCounter{0};
    std::array<int, kMaxVoices> voiceNotes{};

    struct ParameterState {
        std::atomic<float> strumEnable{0.0f};
        std::atomic<float> strumDirection{0.0f};
        std::atomic<float> strumSpreadMs{10.0f};
        std::atomic<float> strumHumanizeMs{2.0f};
        std::atomic<float> strumVelCurve{0.15f};
        std::atomic<float> legatoMode{1.0f};
        std::atomic<float> legatoWindowMs{40.0f};
        std::atomic<float> slideTimeMs{45.0f};
        std::atomic<float> noiseAmount{0.25f};
        std::atomic<float> pickNoiseEnable{1.0f};
        std::atomic<float> squeakEnable{1.0f};
        std::atomic<float> muteThumpEnable{1.0f};
    } params;
};
