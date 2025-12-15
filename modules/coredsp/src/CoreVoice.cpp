#include "gn/CoreVoice.h"
#include <algorithm>

namespace gn {

void CoreVoice::prepare(double sampleRate, int maxDelay) {
    string.prepare(sampleRate, maxDelay);
    exciter.prepare(sampleRate);
    body.prepare(sampleRate, 4);
    body.setMode(0, 110.0f, 4.0f, -6.0f);
    body.setMode(1, 220.0f, 6.0f, -8.0f);
    body.setMode(2, 440.0f, 8.0f, -10.0f);
    body.setMode(3, 880.0f, 10.0f, -12.0f);
    tone.prepare(sampleRate);
    dc.reset(static_cast<float>(sampleRate));
    outputGain.reset(1.0f);
    outputGain.setTime(2.0f, static_cast<float>(sampleRate));
}

void CoreVoice::start(float frequency, float velocity, const VoiceParams& params) {
    string.setFrequency(frequency);
    string.setDamping(params.damping);
    exciter.setMode(params.noise > 0.5f ? ExciterMode::Pick : ExciterMode::Finger);
    exciter.setNoiseLevel(params.noise);
    exciter.setReleaseLevel(params.releaseNoise);
    tone.setTone(params.toneHp, params.toneLp, params.presenceDb);
    body.resetState();
    dc.reset(48000.0f);
    string.excite(velocity);
    outputGain.setTarget(std::clamp(velocity, 0.0f, 1.0f));
    active = true;
}

void CoreVoice::render(float* buffer, int numSamples) {
    if (!active) return;
    std::fill(buffer, buffer + numSamples, 0.0f);
    string.process(buffer, numSamples);
    for (int i = 0; i < numSamples; ++i) {
        float sample = buffer[i];
        sample = body.process(sample);
        sample = tone.process(sample);
        sample = dc.process(sample);
        sample = softClip(sample * outputGain.process());
        buffer[i] = preventDenorm(sample);
    }
}

} // namespace gn
