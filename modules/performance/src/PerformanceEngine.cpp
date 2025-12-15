#include "gnperf/PerformanceEngine.h"
#include <algorithm>

namespace gnperf {

void PerformanceEngine::reset(double sr) {
    sampleRate = sr;
    head = tail = 0;
    queue.fill(PlayEvent{});
}

void PerformanceEngine::setStrum(const StrumSettings& settings) { strumSettings = settings; }

void PerformanceEngine::setLegatoTime(int samples) { legatoSamples = samples; }

void PerformanceEngine::enqueueMidiNote(int midiNote, float velocity, int sampleOffset) {
    const size_t nextHead = (head + 1) % kMaxEvents;
    if (nextHead == tail) return; // drop if full

    PlayEvent ev{};
    ev.type = EventType::NoteOn;
    ev.note = midiNote;
    ev.velocity = velocity;
    ev.sampleOffset = sampleOffset;
    ev.articulation = (velocity < 0.1f) ? Articulation::Dead : Articulation::Sustain;
    queue[head] = ev;
    head = nextHead;

    if (strumSettings.enabled) {
        const int direction = strumSettings.up ? 1 : -1;
        for (int i = 1; i < 4; ++i) {
            const size_t idx = (head + i) % kMaxEvents;
            if (idx == tail) break;
            PlayEvent delayed = ev;
            delayed.sampleOffset += direction * i * (strumSettings.strumWidthSamples / 4);
            queue[idx] = delayed;
            head = (head + 1) % kMaxEvents;
        }
    }
}

size_t PerformanceEngine::popEvents(std::array<PlayEvent, kMaxEvents>& outEvents) {
    size_t count = 0;
    while (tail != head && count < outEvents.size()) {
        outEvents[count++] = queue[tail];
        tail = (tail + 1) % kMaxEvents;
    }
    return count;
}

} // namespace gnperf
