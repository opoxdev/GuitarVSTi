#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <array>

#if defined(GN_USE_JUCE)
 #include <juce_audio_basics/juce_audio_basics.h>
 #include <juce_audio_processors/juce_audio_processors.h>
 #include <juce_audio_utils/juce_audio_utils.h>
 #include <juce_gui_basics/juce_gui_basics.h>
#else

namespace juce {

class String {
public:
    String() = default;
    String(const char* c) : value(c) {}
    String(const std::string& s) : value(s) {}
    operator std::string() const { return value; }
    const std::string& toStdString() const { return value; }

private:
    std::string value;
};

class MemoryBlock {
public:
    void setSize(std::size_t newSize, bool initialiseToZero = false) {
        data.resize(newSize, initialiseToZero ? 0u : 0u);
    }

    void append(const void* srcData, std::size_t numBytes) {
        const auto* bytes = static_cast<const std::uint8_t*>(srcData);
        data.insert(data.end(), bytes, bytes + numBytes);
    }

    [[nodiscard]] const void* getData() const { return data.data(); }
    [[nodiscard]] std::size_t getSize() const { return data.size(); }

private:
    std::vector<std::uint8_t> data;
};

class AudioBuffer {
public:
    AudioBuffer(int channels, int samples)
        : data(channels, std::vector<float>(samples, 0.0f)) {}

    int getNumSamples() const { return data.empty() ? 0 : static_cast<int>(data[0].size()); }
    int getNumChannels() const { return static_cast<int>(data.size()); }
    float* getWritePointer(int channel) { return data[channel].data(); }

private:
    std::vector<std::vector<float>> data;
};

class MidiMessage {
public:
    static MidiMessage noteOn(int channel, int note, float velocity) {
        MidiMessage m; m.note = note; m.velocity = velocity; m.isOn = true; m.channel = channel; return m;
    }
    int getNoteNumber() const { return note; }
    float getVelocity() const { return velocity; }
    bool isNoteOn() const { return isOn; }

private:
    int note{0};
    float velocity{0.0f};
    bool isOn{false};
    int channel{1};
};

class MidiBuffer {
public:
    void addEvent(const MidiMessage& msg, int samplePos) { events.push_back({msg, samplePos}); }
    struct Item { MidiMessage message; int samplePosition; };
    std::vector<Item>::const_iterator begin() const { return events.begin(); }
    std::vector<Item>::const_iterator end() const { return events.end(); }

private:
    std::vector<Item> events;
};

class AudioProcessor {
public:
    virtual ~AudioProcessor() = default;
    virtual void prepareToPlay(double sampleRate, int blockSize) = 0;
    virtual void processBlock(AudioBuffer& buffer, MidiBuffer& midiMessages) = 0;
};

class Component { };
class AudioProcessorEditor : public Component {
public:
    explicit AudioProcessorEditor(AudioProcessor&) {}
    virtual ~AudioProcessorEditor() = default;
};

} // namespace juce

#endif
