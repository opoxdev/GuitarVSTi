#pragma once

#include <cstdint>
#include <array>

namespace gnperf {

enum class EventType : uint8_t { None, NoteOn, NoteOff, Slide, Mute };

enum class Articulation : uint8_t { Sustain, PalmMute, Dead, Legato };

struct PlayEvent {
    EventType type{EventType::None};
    int sampleOffset{0};
    int note{0};
    float velocity{0.0f};
    Articulation articulation{Articulation::Sustain};
    int slideTarget{-1};
    float muteAmount{0.0f};
};

} // namespace gnperf
