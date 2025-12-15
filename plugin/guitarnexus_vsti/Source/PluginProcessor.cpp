#include "PluginProcessor.h"
#include <algorithm>

GuitarNexusAudioProcessor::GuitarNexusAudioProcessor() = default;

void GuitarNexusAudioProcessor::prepareToPlay(double sr, int samplesPerBlock) {
    sampleRate = sr;
    performance.reset(sr);
    for (auto& v : voices) v.prepare(sampleRate, static_cast<int>(sampleRate));
    renderScratch.assign(static_cast<size_t>(samplesPerBlock), 0.0f);
    // Load factory presets lazily using embedded minimal bank
    const char* factoryJson = R"({"presets":[{"name":"Factory Clean","tags":["clean","wide"],"parameters":{"toneHp":80,"toneLp":9000,"presenceDb":2,"noise":0.05}},{"name":"Factory Crunch","tags":["drive"],"parameters":{"toneHp":120,"toneLp":6000,"presenceDb":6,"noise":0.25}}]})";
    presetDb.loadBankFromString(factoryJson);
    currentSnapshot = presetDb.makeSnapshot(0);
}

void GuitarNexusAudioProcessor::processBlock(GnAudioBuffer& buffer, GnMidiBuffer& midiMessages) {
    const int numSamples = buffer.getNumSamples();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        std::fill(buffer.getWritePointer(ch), buffer.getWritePointer(ch) + numSamples, 0.0f);
    }

    std::array<gnperf::PlayEvent, gnperf::PerformanceEngine::kMaxEvents> events{};
#if defined(GN_USE_JUCE)
    for (const auto metadata : midiMessages) {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            const int samplePos = static_cast<int>(metadata.samplePosition);
            performance.enqueueMidiNote(msg.getNoteNumber(), msg.getVelocity(), samplePos);
        }
    }
#else
    for (const auto& midi : midiMessages) {
        if (midi.message.isNoteOn()) {
            performance.enqueueMidiNote(midi.message.getNoteNumber(), midi.message.getVelocity(), midi.samplePosition);
        }
    }
#endif

    size_t count = performance.popEvents(events);
    for (size_t i = 0; i < count; ++i) {
        const auto& ev = events[i];
        if (ev.type == gnperf::EventType::NoteOn) {
            const float freq = 440.0f * std::pow(2.0f, (ev.note - 69) / 12.0f);
            gn::VoiceParams params;
            params.toneHp = currentSnapshot.values.count("toneHp") ? currentSnapshot.values.at("toneHp") : 60.0f;
            params.toneLp = currentSnapshot.values.count("toneLp") ? currentSnapshot.values.at("toneLp") : 8000.0f;
            params.presenceDb = currentSnapshot.values.count("presenceDb") ? currentSnapshot.values.at("presenceDb") : 3.0f;
            params.noise = currentSnapshot.values.count("noise") ? currentSnapshot.values.at("noise") : 0.1f;
            params.releaseNoise = 0.05f;
            for (auto& v : voices) {
                if (!v.isActive()) {
                    v.start(freq, ev.velocity, params);
                    break;
                }
            }
        }
    }

    for (auto& v : voices) {
        if (!v.isActive()) continue;
        if (static_cast<int>(renderScratch.size()) < numSamples) continue;
        std::fill(renderScratch.begin(), renderScratch.begin() + numSamples, 0.0f);
        v.render(renderScratch.data(), numSamples);
        for (int i = 0; i < numSamples; ++i) {
            buffer.getWritePointer(0)[i] += renderScratch[static_cast<size_t>(i)];
            if (buffer.getNumChannels() > 1)
                buffer.getWritePointer(1)[i] += renderScratch[static_cast<size_t>(i)];
        }
    }
}

#if defined(GN_USE_JUCE)
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new GuitarNexusAudioProcessor();
}
#endif
