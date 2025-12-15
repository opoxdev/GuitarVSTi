#include "gn/PickExciter.h"
#include <cmath>

namespace gn {

void PickExciter::prepare(double newSampleRate) {
    sampleRate = newSampleRate;
    noiseSmoother.reset(0.0f);
    noiseSmoother.setTime(5.0f, static_cast<float>(sampleRate));
    releaseSmoother.reset(0.0f);
    releaseSmoother.setTime(5.0f, static_cast<float>(sampleRate));
}

void PickExciter::setMode(ExciterMode newMode) { mode = newMode; }

void PickExciter::setNoiseLevel(float level) { noiseSmoother.setTarget(level); }

void PickExciter::setReleaseLevel(float level) { releaseSmoother.setTarget(level); }

float PickExciter::renderSample(float velocity) {
    const float base = (mode == ExciterMode::Pick) ? 1.0f : 0.6f;
    phase += velocity * 4.0f;
    if (phase > 1.0f) phase -= 1.0f;
    const float pickShape = std::sin(static_cast<float>(M_PI) * phase) * base;
    const float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * noiseSmoother.process();
    const float releaseNoise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * releaseSmoother.process();
    return pickShape + noise + releaseNoise;
}

} // namespace gn
