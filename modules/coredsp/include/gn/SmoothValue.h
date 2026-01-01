#pragma once

#include <algorithm>
#include <cmath>

namespace gn {

class SmoothValue {
public:
    SmoothValue() = default;

    void reset(float value) {
        current = target = value;
    }

    void setTime(float timeMs, float sampleRate) {
        const float tau = std::max(timeMs, 0.1f) * 0.001f;
        const float coeff = std::exp(-1.0f / (tau * sampleRate));
        alpha = coeff;
    }

    void setTarget(float value) { target = value; }

    float getCurrent() const { return current; }

    float process() {
        current = target + alpha * (current - target);
        return current;
    }

private:
    float current{0.0f};
    float target{0.0f};
    float alpha{0.0f};
};

} // namespace gn
