#include "gn/NoiseLayer.h"
#include "gn/DspUtilities.h"
#include <algorithm>
#include <cstring>

namespace gn {

void NoiseLayer::prepare(double sr, int maxBurstSamples) {
    sampleRate = sr;
    maxLength = std::max(32, maxBurstSamples);
    bursts.fill(Burst{});
    head = 0;
}

void NoiseLayer::setToggles(bool pickEnabled, bool squeakEnabled, bool thumpEnabled) {
    pick = pickEnabled;
    squeak = squeakEnabled;
    thump = thumpEnabled;
}

void NoiseLayer::setAmount(float amt) { amount = std::clamp(amt, 0.0f, 1.0f); }

float NoiseLayer::nextNoise() {
    rng = rng * 1664525u + 1013904223u;
    const uint32_t bits = (rng >> 9) | 0x3f800000u;
    float val;
    std::memcpy(&val, &bits, sizeof(val));
    return (val - 1.5f) * 2.0f; // ~[-1,1]
}

void NoiseLayer::pushBurst(float amplitude, float decay, int lengthSamples) {
    const size_t next = (head + 1) % bursts.size();
    if (bursts[next].remaining > 0 && next == head) return;
    Burst b{};
    b.amplitude = amplitude * amount;
    b.decay = decay;
    b.remaining = std::min(lengthSamples, maxLength);
    b.filter = 0.0f;
    bursts[head] = b;
    head = next;
}

void NoiseLayer::triggerPick(float velocity) {
    if (!pick || amount <= 0.0001f) return;
    const float amp = std::clamp(velocity, 0.05f, 1.0f) * 0.4f;
    pushBurst(amp, 0.985f, static_cast<int>(0.02 * sampleRate));
}

void NoiseLayer::triggerSqueak(float intensity) {
    if (!squeak || amount <= 0.0001f) return;
    const float amp = std::clamp(intensity, 0.0f, 1.0f) * 0.25f;
    pushBurst(amp, 0.992f, static_cast<int>(0.05 * sampleRate));
}

void NoiseLayer::triggerThump(float velocity) {
    if (!thump || amount <= 0.0001f) return;
    const float amp = std::clamp(velocity, 0.0f, 1.0f) * 0.3f;
    pushBurst(amp, 0.97f, static_cast<int>(0.03 * sampleRate));
}

void NoiseLayer::render(float* buffer, int numSamples) {
    if (amount <= 0.0001f) return;
    for (auto& burst : bursts) {
        if (burst.remaining <= 0) continue;
        for (int i = 0; i < numSamples && burst.remaining > 0; ++i) {
            float n = nextNoise();
            burst.filter = 0.5f * burst.filter + 0.5f * n;
            buffer[i] += burst.filter * burst.amplitude;
            burst.amplitude *= burst.decay;
            --burst.remaining;
        }
    }
}

} // namespace gn

