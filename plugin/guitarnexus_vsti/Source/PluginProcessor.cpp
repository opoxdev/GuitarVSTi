#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>
#include <cmath>
#include <unordered_map>

#if defined(GN_USE_JUCE)
GuitarNexusAudioProcessor::GuitarNexusAudioProcessor()
    : juce::AudioProcessor(juce::AudioProcessor::BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)) {
    voiceNotes.fill(-1);
}
#else
GuitarNexusAudioProcessor::GuitarNexusAudioProcessor() {
    voiceNotes.fill(-1);
}
#endif

void GuitarNexusAudioProcessor::prepareToPlay(double sr, int samplesPerBlock) {
    sampleRate = sr;
    performance.reset(sr);
    for (auto& v : voices) v.prepare(sampleRate, static_cast<int>(sampleRate));
    noiseLayer.prepare(sampleRate, samplesPerBlock);
    renderScratch.assign(static_cast<size_t>(samplesPerBlock), 0.0f);
    noiseScratch.assign(static_cast<size_t>(samplesPerBlock), 0.0f);
    sampleCounter = 0;
    setParameter("strumEnable", 1.0f);
    setParameter("strumDirection", 0.0f);
    setParameter("strumSpreadMs", 18.0f);
    setParameter("strumHumanizeMs", 2.0f);
    setParameter("strumVelCurve", 0.15f);
    setParameter("legatoMode", 1.0f);
    setParameter("legatoWindowMs", 45.0f);
    setParameter("slideTimeMs", 60.0f);
    setParameter("noiseAmount", 0.3f);
    setParameter("pickNoiseEnable", 1.0f);
    setParameter("squeakEnable", 1.0f);
    setParameter("muteThumpEnable", 1.0f);

    const char* factoryJson = R"({"presets":[{"name":"Clean Strum","tags":["clean","strum"],"parameters":{"toneHp":80,"toneLp":9000,"presenceDb":2,"noise":0.05,"strumEnable":1,"strumSpreadMs":22,"strumDirection":0}},{"name":"Tight Funk","tags":["funk","tight"],"parameters":{"toneHp":120,"toneLp":7500,"presenceDb":3,"noise":0.12,"strumEnable":1,"strumSpreadMs":8,"muteThumpEnable":1}},{"name":"Legato Lead","tags":["lead","legato"],"parameters":{"toneHp":160,"toneLp":6500,"presenceDb":5,"noise":0.18,"legatoMode":2,"slideTimeMs":70,"strumEnable":0}},{"name":"Slide Lead","tags":["lead","slide"],"parameters":{"toneHp":140,"toneLp":5200,"presenceDb":6,"noise":0.22,"legatoMode":1,"slideTimeMs":120,"squeakEnable":1}},{"name":"Lo-fi Noisy Guitar","tags":["lofi","noisy"],"parameters":{"toneHp":60,"toneLp":4800,"presenceDb":-2,"noise":0.45,"strumEnable":1,"strumSpreadMs":26,"strumVelCurve":-0.2,"pickNoiseEnable":1,"muteThumpEnable":1}}]})";
    presetDb.loadBankFromString(factoryJson);
    currentSnapshot = presetDb.makeSnapshot(0);
    for (const auto& kv : currentSnapshot.values) setParameter(kv.first, kv.second);
}

void GuitarNexusAudioProcessor::processBlock(GnAudioBuffer& buffer, GnMidiBuffer& midiMessages) {
    const int numSamples = buffer.getNumSamples();
    performance.beginBlock(sampleCounter, numSamples);

    gnperf::StrumSettings strum{};
    strum.enabled = getParameter("strumEnable") > 0.5f;
    strum.direction = static_cast<gnperf::StrumDirection>(static_cast<int>(getParameter("strumDirection") + 0.5f));
    strum.strumWidthSamples = static_cast<int>(getParameter("strumSpreadMs") * 0.001 * sampleRate);
    strum.humanizeSamples = static_cast<int>(getParameter("strumHumanizeMs") * 0.001 * sampleRate);
    strum.velocityCurve = getParameter("strumVelCurve");
    performance.setStrum(strum);

    gnperf::LegatoSettings legato{};
    legato.mode = static_cast<gnperf::LegatoMode>(static_cast<int>(getParameter("legatoMode") + 0.5f));
    legato.windowSamples = static_cast<int>(getParameter("legatoWindowMs") * 0.001 * sampleRate);
    legato.slideSamples = static_cast<int>(getParameter("slideTimeMs") * 0.001 * sampleRate);
    performance.setLegato(legato);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        std::fill(buffer.getWritePointer(ch), buffer.getWritePointer(ch) + numSamples, 0.0f);
    }

    std::array<gnperf::PlayEvent, gnperf::PerformanceEngine::kMaxEvents> events{};
#if defined(GN_USE_JUCE)
    for (const auto metadata : midiMessages) {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn() || msg.isNoteOff()) {
            const int samplePos = static_cast<int>(metadata.samplePosition);
            const bool noteOn = msg.isNoteOn();
            const float vel = noteOn ? msg.getVelocity() : msg.getFloatVelocity();
            performance.enqueueMidiNote(msg.getNoteNumber(), vel, samplePos, noteOn, msg.getChannel() - 1);
        }
    }
#else
    for (const auto& midi : midiMessages) {
        if (midi.message.isNoteOn() || midi.message.isNoteOff()) {
            performance.enqueueMidiNote(midi.message.getNoteNumber(), midi.message.getVelocity(), midi.samplePosition,
                                        midi.message.isNoteOn(), 0);
        }
    }
#endif

    performance.finalizeBlock();

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

            size_t voiceIndex = 0;
            bool reused = false;
            for (size_t idx = 0; idx < voices.size(); ++idx) {
                if (voices[idx].isActive() && voiceNotes[idx] == ev.slideTarget) {
                    voices[idx].legatoSlide(freq, ev.velocity, ev.slideTimeSamples);
                    voiceNotes[idx] = ev.note;
                    reused = true;
                    break;
                }
            }
            if (!reused) {
                for (size_t idx = 0; idx < voices.size(); ++idx) {
                    voiceIndex = idx;
                    if (!voices[idx].isActive()) break;
                }
                voices[voiceIndex].start(freq, ev.velocity, params);
                voiceNotes[voiceIndex] = ev.note;
            }

            noiseLayer.triggerPick(ev.velocity);
            if (ev.articulation == gnperf::Articulation::Legato && ev.slideTarget >= 0) {
                const float slideIntensity = std::min(std::abs(ev.slideTarget - ev.note) / 12.0f, 1.0f);
                noiseLayer.triggerSqueak(slideIntensity);
            }
        }
        if (ev.type == gnperf::EventType::NoteOff) {
            for (size_t idx = 0; idx < voices.size(); ++idx) {
                if (voiceNotes[idx] == ev.note) {
                    voices[idx].stop();
                    noiseLayer.triggerThump(ev.velocity);
                }
            }
        }
    }

    noiseLayer.setAmount(getParameter("noiseAmount"));
    noiseLayer.setToggles(getParameter("pickNoiseEnable") > 0.5f, getParameter("squeakEnable") > 0.5f,
                          getParameter("muteThumpEnable") > 0.5f);

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

    for (size_t idx = 0; idx < voices.size(); ++idx) {
        if (!voices[idx].isActive()) voiceNotes[idx] = -1;
    }

    if (static_cast<int>(noiseScratch.size()) >= numSamples) {
        std::fill(noiseScratch.begin(), noiseScratch.begin() + numSamples, 0.0f);
        noiseLayer.render(noiseScratch.data(), numSamples);
        for (int i = 0; i < numSamples; ++i) {
            buffer.getWritePointer(0)[i] += noiseScratch[static_cast<size_t>(i)];
            if (buffer.getNumChannels() > 1)
                buffer.getWritePointer(1)[i] += noiseScratch[static_cast<size_t>(i)];
        }
    }

    sampleCounter += numSamples;
}

void GuitarNexusAudioProcessor::setParameter(const std::string& id, float value) {
    if (id == "strumEnable") params.strumEnable.store(value);
    else if (id == "strumDirection") params.strumDirection.store(value);
    else if (id == "strumSpreadMs") params.strumSpreadMs.store(value);
    else if (id == "strumHumanizeMs") params.strumHumanizeMs.store(value);
    else if (id == "strumVelCurve") params.strumVelCurve.store(value);
    else if (id == "legatoMode") params.legatoMode.store(value);
    else if (id == "legatoWindowMs") params.legatoWindowMs.store(value);
    else if (id == "slideTimeMs") params.slideTimeMs.store(value);
    else if (id == "noiseAmount") params.noiseAmount.store(value);
    else if (id == "pickNoiseEnable") params.pickNoiseEnable.store(value);
    else if (id == "squeakEnable") params.squeakEnable.store(value);
    else if (id == "muteThumpEnable") params.muteThumpEnable.store(value);
}

float GuitarNexusAudioProcessor::getParameter(const std::string& id) const {
    if (id == "strumEnable") return params.strumEnable.load();
    if (id == "strumDirection") return params.strumDirection.load();
    if (id == "strumSpreadMs") return params.strumSpreadMs.load();
    if (id == "strumHumanizeMs") return params.strumHumanizeMs.load();
    if (id == "strumVelCurve") return params.strumVelCurve.load();
    if (id == "legatoMode") return params.legatoMode.load();
    if (id == "legatoWindowMs") return params.legatoWindowMs.load();
    if (id == "slideTimeMs") return params.slideTimeMs.load();
    if (id == "noiseAmount") return params.noiseAmount.load();
    if (id == "pickNoiseEnable") return params.pickNoiseEnable.load();
    if (id == "squeakEnable") return params.squeakEnable.load();
    if (id == "muteThumpEnable") return params.muteThumpEnable.load();
    return 0.0f;
}

#if defined(GN_USE_JUCE)
const juce::String GuitarNexusAudioProcessor::getName() const { return "GuitarNexus"; }
bool GuitarNexusAudioProcessor::acceptsMidi() const { return true; }
bool GuitarNexusAudioProcessor::producesMidi() const { return false; }
bool GuitarNexusAudioProcessor::isMidiEffect() const { return false; }
double GuitarNexusAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int GuitarNexusAudioProcessor::getNumPrograms() { return 1; }
int GuitarNexusAudioProcessor::getCurrentProgram() { return 0; }
void GuitarNexusAudioProcessor::setCurrentProgram(int) {}
const juce::String GuitarNexusAudioProcessor::getProgramName(int) { return "Default"; }
void GuitarNexusAudioProcessor::changeProgramName(int, const juce::String&) {}
void GuitarNexusAudioProcessor::releaseResources() {}
bool GuitarNexusAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* GuitarNexusAudioProcessor::createEditor() { return new GuitarNexusAudioProcessorEditor(*this); }
void GuitarNexusAudioProcessor::getStateInformation(juce::MemoryBlock& destData) { destData.setSize(0); }
void GuitarNexusAudioProcessor::setStateInformation(const void*, int) {}
bool GuitarNexusAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    auto mainOut = layouts.getMainOutputChannelSet();
    return mainOut == juce::AudioChannelSet::mono() || mainOut == juce::AudioChannelSet::stereo();
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new GuitarNexusAudioProcessor();
}
#endif
