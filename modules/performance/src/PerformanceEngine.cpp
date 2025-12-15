#include "gnperf/PerformanceEngine.h"
#include <algorithm>
#include <cstdlib>

namespace gnperf {

void PerformanceEngine::reset(double sr) {
    sampleRate = sr;
    head = tail = 0;
    pendingHead = pendingTail = 0;
    scheduledHead = scheduledTail = 0;
    queue.fill(PlayEvent{});
    pending.fill(PendingNote{});
    scheduled.fill(TimedEvent{});
    currentBlockStart = 0;
    currentBlockSize = 0;
    strumFlip = false;
}

void PerformanceEngine::beginBlock(int64_t blockStartSamples, int blockSize) {
    currentBlockStart = blockStartSamples;
    currentBlockSize = blockSize;
}

void PerformanceEngine::setStrum(const StrumSettings& settings) { strumSettings = settings; }

void PerformanceEngine::setLegato(const LegatoSettings& settings) { legatoSettings = settings; }

uint32_t PerformanceEngine::nextRandom() {
    randState = randState * 1664525u + 1013904223u;
    return randState;
}

PlayEvent PerformanceEngine::buildEvent(const PendingNote& note) {
    PlayEvent ev{};
    ev.type = note.noteOn ? EventType::NoteOn : EventType::NoteOff;
    ev.note = note.note;
    ev.velocity = note.velocity;
    if (note.noteOn) {
        auto& last = lastNotes[static_cast<size_t>(note.channel) % lastNotes.size()];
        const int64_t delta = note.time - last.time;
        const bool withinWindow = last.active && delta >= 0 && delta <= legatoSettings.windowSamples;
        if (legatoSettings.mode == LegatoMode::Force || (legatoSettings.mode == LegatoMode::Auto && withinWindow)) {
            ev.articulation = Articulation::Legato;
            ev.slideTarget = last.note;
            ev.slideTimeSamples = legatoSettings.slideSamples;
        }
        last.note = note.note;
        last.time = note.time;
        last.active = true;
    } else {
        auto& last = lastNotes[static_cast<size_t>(note.channel) % lastNotes.size()];
        last.active = false;
    }
    return ev;
}

void PerformanceEngine::enqueueMidiNote(int midiNote, float velocity, int sampleOffset, bool noteOn, int channel) {
    const size_t nextHead = (pendingHead + 1) % kMaxEvents;
    if (nextHead == pendingTail) return;
    PendingNote note;
    note.noteOn = noteOn;
    note.note = midiNote;
    note.velocity = velocity;
    note.time = currentBlockStart + sampleOffset;
    note.channel = channel;
    pending[pendingHead] = note;
    pendingHead = nextHead;
}

void PerformanceEngine::pushScheduled(const PlayEvent& ev, int64_t absoluteTime) {
    const size_t next = (scheduledHead + 1) % kMaxEvents;
    if (next == scheduledTail) return;
    scheduled[scheduledHead] = TimedEvent{ev, absoluteTime};
    scheduledHead = next;
}

void PerformanceEngine::scheduleChord(const PendingNote* chord, size_t count) {
    if (count == 0) return;
    std::array<PendingNote, kMaxEvents> ordered{};
    for (size_t i = 0; i < count && i < ordered.size(); ++i) ordered[i] = chord[i];
    switch (strumSettings.direction) {
    case StrumDirection::Down:
        std::sort(ordered.begin(), ordered.begin() + static_cast<long>(count),
                  [](const PendingNote& a, const PendingNote& b) { return a.note > b.note; });
        break;
    case StrumDirection::Up:
        std::sort(ordered.begin(), ordered.begin() + static_cast<long>(count),
                  [](const PendingNote& a, const PendingNote& b) { return a.note < b.note; });
        break;
    case StrumDirection::Alternate:
        if (strumFlip) {
            std::sort(ordered.begin(), ordered.begin() + static_cast<long>(count),
                      [](const PendingNote& a, const PendingNote& b) { return a.note < b.note; });
        } else {
            std::sort(ordered.begin(), ordered.begin() + static_cast<long>(count),
                      [](const PendingNote& a, const PendingNote& b) { return a.note > b.note; });
        }
        strumFlip = !strumFlip;
        break;
    case StrumDirection::Random:
        for (std::size_t i = count; i > 1; --i) {
            const std::size_t j = nextRandom() % i;
            std::swap(ordered[i - 1], ordered[j]);
        }
        break;
    }

    const int width = strumSettings.strumWidthSamples;
    const int human = strumSettings.humanizeSamples;
    const float velCurve = strumSettings.velocityCurve;
    for (std::size_t i = 0; i < count; ++i) {
        const auto& note = ordered[i];
        PlayEvent ev = buildEvent(note);
        const float ramp = count > 1 ? static_cast<float>(i) / static_cast<float>(count - 1) : 0.5f;
        const float curve = 1.0f + velCurve * (0.5f - ramp);
        ev.velocity = std::clamp(note.velocity * curve, 0.0f, 1.0f);
        int64_t time = note.time + static_cast<int64_t>((width * static_cast<int>(i)) / std::max<int>(1, static_cast<int>(count - 1)));
        if (human > 0) {
            int jitter = static_cast<int>(static_cast<int32_t>(nextRandom() % (human * 2)) - human);
            time += jitter;
        }
        pushScheduled(ev, time);
    }
}

void PerformanceEngine::finalizeBlock() {
    std::array<PendingNote, kMaxEvents> chord{};
    size_t chordCount = 0;
    int64_t lastTime = -1;
    while (pendingTail != pendingHead) {
        const auto note = pending[pendingTail];
        pendingTail = (pendingTail + 1) % kMaxEvents;
        if (!strumSettings.enabled) {
            pushScheduled(buildEvent(note), note.time);
            continue;
        }

        if (lastTime < 0) lastTime = note.time;
        const int64_t delta = note.time - lastTime;
        if (std::llabs(delta) <= 4 && chordCount < chord.size()) {
            chord[chordCount++] = note;
        } else {
            scheduleChord(chord.data(), chordCount);
            chordCount = 0;
            chord[chordCount++] = note;
        }
        lastTime = note.time;
    }
    if (chordCount > 0) scheduleChord(chord.data(), chordCount);

    const int64_t blockEnd = currentBlockStart + currentBlockSize;
    size_t outCount = 0;
    while (scheduledTail != scheduledHead && outCount < kMaxEvents) {
        const auto pendingEv = scheduled[scheduledTail];
        if (pendingEv.time >= blockEnd) break;
        scheduledTail = (scheduledTail + 1) % kMaxEvents;
        PlayEvent ev = pendingEv.event;
        ev.sampleOffset = static_cast<int>(pendingEv.time - currentBlockStart);
        pushEvent(ev);
        ++outCount;
    }
}

void PerformanceEngine::pushEvent(const PlayEvent& ev) {
    const size_t next = (head + 1) % kMaxEvents;
    if (next == tail) return;
    queue[head] = ev;
    head = next;
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
