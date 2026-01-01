#include "gn/BodyResonator.h"
#include <algorithm>

namespace gn {

void ModalResonatorBank::prepare(double newSampleRate, int modalCount) {
    sampleRate = newSampleRate;
    modes.assign(static_cast<size_t>(modalCount), Biquad{});
}

void ModalResonatorBank::setMode(int index, float freq, float q, float gainDb) {
    if (index < 0 || index >= static_cast<int>(modes.size())) return;
    modes[static_cast<size_t>(index)] = designPeak(static_cast<float>(sampleRate), freq, q, gainDb);
}

float ModalResonatorBank::process(float input) noexcept {
    float acc = 0.0f;
    for (auto& m : modes) {
        acc += m.process(input);
    }
    return softClip(acc * 0.5f);
}

void ModalResonatorBank::resetState() {
    for (auto& m : modes) m.reset();
}

void BodyFilter::prepare(double newSampleRate) {
    sampleRate = newSampleRate;
    hp = designHighpass(static_cast<float>(sampleRate), 40.0f, 0.707f);
    lp = designLowpass(static_cast<float>(sampleRate), 8000.0f, 0.707f);
    presence = designPeak(static_cast<float>(sampleRate), 3000.0f, 1.0f, 0.0f);
}

void BodyFilter::setTone(float hpHz, float lpHz, float presenceDb) {
    hp = designHighpass(static_cast<float>(sampleRate), hpHz, 0.707f);
    lp = designLowpass(static_cast<float>(sampleRate), lpHz, 0.707f);
    presence = designPeak(static_cast<float>(sampleRate), 3200.0f, 1.2f, presenceDb);
}

float BodyFilter::process(float input) noexcept {
    float y = hp.process(input);
    y = lp.process(y);
    y = presence.process(y);
    return softClip(y);
}

void BodyFilter::resetState() {
    hp.reset();
    lp.reset();
    presence.reset();
}

} // namespace gn
