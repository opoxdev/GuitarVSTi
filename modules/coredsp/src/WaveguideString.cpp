#include "gn/WaveguideString.h"
#include <algorithm>

namespace gn {

void WaveguideString::prepare(double newSampleRate, int maxDelaySamples) {
    sampleRate = newSampleRate;
    maxDelay = maxDelaySamples;
    delayA.assign(static_cast<size_t>(maxDelay), 0.0f);
    delayB.assign(static_cast<size_t>(maxDelay), 0.0f);
    writeIndex = 0;
    freqSmoother.reset(110.0f);
    freqSmoother.setTime(2.0f, static_cast<float>(sampleRate));
    dampingSmoother.reset(0.995f);
    dampingSmoother.setTime(2.0f, static_cast<float>(sampleRate));
    dcBlock.reset(static_cast<float>(sampleRate));
}

void WaveguideString::setFrequency(float hz) { freqSmoother.setTarget(hz); }

void WaveguideString::setDamping(float value) { dampingSmoother.setTarget(value); }

void WaveguideString::excite(float velocity) {
    const float energy = std::clamp(velocity, 0.0f, 1.0f);
    for (auto& d : delayA) d = energy;
    for (auto& d : delayB) d = -energy;
}

void WaveguideString::updateDelayLine() {
    const float freq = std::max(freqSmoother.process(), 20.0f);
    const float delaySamples = static_cast<float>(sampleRate) / freq;
    const float damp = std::clamp(dampingSmoother.process(), 0.5f, 0.9999f);
    feedback = damp;
    fractionalIndex = delaySamples;
    intPart = static_cast<int>(fractionalIndex);
    frac = fractionalIndex - static_cast<float>(intPart);
}

void WaveguideString::process(float* output, int numSamples) noexcept {
    updateDelayLine();
    const int size = maxDelay;
    if (size <= 2) return;

    for (int i = 0; i < numSamples; ++i) {
        const int readIndexA = (writeIndex - intPart + size) % size;
        const int readIndexB = (readIndexA - 1 + size) % size;
        const float s0 = delayA[readIndexA];
        const float s1 = delayA[readIndexB];
        const float interp = s0 + frac * (s1 - s0);
        const float next = delayB[readIndexA];
        const float out = (interp + next) * 0.5f;
        const float clamped = softClip(dcBlock.process(out));
        output[i] = preventDenorm(clamped);

        const float reflected = -interp * feedback;
        delayB[writeIndex] = reflected;
        delayA[writeIndex] = next * feedback;
        writeIndex = (writeIndex + 1) % size;
    }
}

} // namespace gn
