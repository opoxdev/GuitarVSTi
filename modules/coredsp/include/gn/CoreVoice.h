#pragma once

#include "BodyResonator.h"
#include "PickExciter.h"
#include "WaveguideString.h"

namespace gn {

struct VoiceParams {
    float damping{0.995f};
    float noise{0.1f};
    float releaseNoise{0.05f};
    float toneHp{60.0f};
    float toneLp{8000.0f};
    float presenceDb{3.0f};
};

class CoreVoice {
public:
    void prepare(double sampleRate, int maxDelay);
    void start(float frequency, float velocity, const VoiceParams& params);
    void render(float* buffer, int numSamples);
    bool isActive() const { return active; }

private:
    WaveguideString string;
    PickExciter exciter;
    ModalResonatorBank body;
    BodyFilter tone;
    DcBlocker dc;
    SmoothValue outputGain;
    bool active{false};
};

} // namespace gn
