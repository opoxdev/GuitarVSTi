#pragma once

#include <array>
#include <vector>
#include "PlayEvent.h"

namespace gnperf {

enum class StrumDirection : uint8_t { Down, Up, Alternate, Random };

struct StrumSettings {
    bool enabled{false};
    StrumDirection direction{StrumDirection::Down};
    int strumWidthSamples{0};
    int humanizeSamples{0};
    float velocityCurve{0.0f};
};

enum class LegatoMode : uint8_t { Off, Auto, Force };

struct LegatoSettings {
    LegatoMode mode{LegatoMode::Auto};
    int windowSamples{0};
    int slideSamples{0};
};

class PerformanceEngine {
public:
    static constexpr size_t kMaxEvents = 1024;

    void beginBlock(int64_t blockStartSamples, int blockSize);
    void finalizeBlock();
    void reset(double sampleRate);
    void setStrum(const StrumSettings& settings);
    void setLegato(const LegatoSettings& settings);
    void enqueueMidiNote(int midiNote, float velocity, int sampleOffset, bool noteOn, int channel = 0);
    size_t popEvents(std::array<PlayEvent, kMaxEvents>& outEvents);

private:
    struct PendingNote {
        bool noteOn{false};
        int note{0};
        float velocity{0.0f};
        int64_t time{0};
        int channel{0};
    };

    double sampleRate{48000.0};
    StrumSettings strumSettings{};
    LegatoSettings legatoSettings{};
    std::array<PlayEvent, kMaxEvents> queue{};
    size_t head{0};
    size_t tail{0};
    std::array<PendingNote, kMaxEvents> pending{};
    size_t pendingHead{0};
    size_t pendingTail{0};
    struct TimedEvent {
        PlayEvent event;
        int64_t time{0};
    };
    std::array<TimedEvent, kMaxEvents> scheduled{};
    size_t scheduledHead{0};
    size_t scheduledTail{0};
    int64_t currentBlockStart{0};
    int currentBlockSize{0};
    bool strumFlip{false};
    uint32_t randState{0x1234567u};
    struct LastNoteState {
        int note{-1};
        int64_t time{0};
        bool active{false};
    };
    std::array<LastNoteState, 16> lastNotes{};

    uint32_t nextRandom();
    void pushScheduled(const PlayEvent& ev, int64_t absoluteTime);
    void scheduleChord(const PendingNote* chord, size_t count);
    void pushEvent(const PlayEvent& ev);
    PlayEvent buildEvent(const PendingNote& note);
};

} // namespace gnperf
