#pragma once

#include <array>
#include <vector>
#include "DspUtilities.h"
#include "SmoothValue.h"

namespace gn {

class WaveguideString {
public:
    WaveguideString() = default;

    void prepare(double sampleRate, int maxDelaySamples);
    void setFrequency(float hz);
    void excite(float velocity);
    void process(float* output, int numSamples) noexcept;
    void setDamping(float value);

private:
    void updateDelayLine();

    double sampleRate{48000.0};
    int maxDelay{0};
    float fractionalIndex{0.0f};
    float frac{0.0f};
    int intPart{0};
    SmoothValue freqSmoother;
    SmoothValue dampingSmoother;
    std::vector<float> delayA;
    std::vector<float> delayB;
    int writeIndex{0};
    float feedback{0.99f};
    DcBlocker dcBlock;
};

} // namespace gn
