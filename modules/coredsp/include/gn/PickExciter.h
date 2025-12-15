#pragma once

#include <array>
#include <random>
#include "SmoothValue.h"

namespace gn {

enum class ExciterMode { Pick, Finger };

class PickExciter {
public:
    void prepare(double sampleRate);
    void setMode(ExciterMode mode);
    void setNoiseLevel(float level);
    void setReleaseLevel(float level);
    float renderSample(float velocity);

private:
    double sampleRate{48000.0};
    ExciterMode mode{ExciterMode::Pick};
    SmoothValue noiseSmoother;
    SmoothValue releaseSmoother;
    float phase{0.0f};
    std::array<float, 2> noiseState{0.0f, 0.0f};
};

} // namespace gn
