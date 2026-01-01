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
    bool isNoteOff() const { return !isOn; }
    float getFloatVelocity() const { return velocity; }
    int getChannel() const { return channel; }

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

class Component {
public:
    virtual ~Component() = default;
    void setBounds(int, int, int w, int h) { width = w; height = h; }
    void setSize(int w, int h) { width = w; height = h; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void addAndMakeVisible(Component*) {}
    void repaint() {}

protected:
    int width{0};
    int height{0};
};
class AudioProcessorEditor : public Component {
public:
    explicit AudioProcessorEditor(AudioProcessor&) {}
    virtual ~AudioProcessorEditor() = default;
};

class Colour {
public:
    explicit Colour(std::uint32_t argb = 0u) : value(argb) {}
    std::uint32_t getARGB() const { return value; }

private:
    std::uint32_t value;
};

namespace Colours {
static const Colour darkslategrey{0xFF2F4F4F};
static const Colour darkgrey{0xFF303030};
static const Colour white{0xFFFFFFFF};
} // namespace Colours

class Graphics {
public:
    void fillAll(Colour) {}
    void setColour(Colour) {}
    void drawText(const std::string&, int, int, int, int, int) {}
};

namespace Justification {
enum { centred = 0 };
}

} // namespace juce

#endif
