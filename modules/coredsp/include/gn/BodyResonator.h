#pragma once

#include <array>
#include <vector>
#include "DspUtilities.h"

namespace gn {

class ModalResonatorBank {
public:
    void prepare(double sampleRate, int modalCount);
    void setMode(int index, float freq, float q, float gainDb);
    float process(float input) noexcept;
    void resetState();

private:
    double sampleRate{48000.0};
    std::vector<Biquad> modes;
};

class BodyFilter {
public:
    void prepare(double sampleRate);
    void setTone(float hpHz, float lpHz, float presenceDb);
    float process(float input) noexcept;
    void resetState();

private:
    double sampleRate{48000.0};
    Biquad hp;
    Biquad lp;
    Biquad presence;
};

} // namespace gn
