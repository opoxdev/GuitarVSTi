#pragma once

#include <array>
#include <vector>
#include "PlayEvent.h"

namespace gnperf {

struct StrumSettings {
    bool enabled{true};
    bool up{false};
    int strumWidthSamples{240};
};

class PerformanceEngine {
public:
    static constexpr size_t kMaxEvents = 1024;

    void reset(double sampleRate);
    void setStrum(const StrumSettings& settings);
    void setLegatoTime(int samples);
    void enqueueMidiNote(int midiNote, float velocity, int sampleOffset);
    size_t popEvents(std::array<PlayEvent, kMaxEvents>& outEvents);

private:
    double sampleRate{48000.0};
    StrumSettings strumSettings{};
    int legatoSamples{120};
    std::array<PlayEvent, kMaxEvents> queue{};
    size_t head{0};
    size_t tail{0};
};

} // namespace gnperf
