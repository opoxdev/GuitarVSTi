#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <algorithm>

namespace gn {

inline bool isDenormal(float v) {
    constexpr float tiny = 1.0e-30f;
    return std::abs(v) < tiny;
}

inline float sanitize(float v) {
    if (std::isnan(v) || std::isinf(v)) {
        return 0.0f;
    }
    return v;
}

inline float softClip(float x) {
    const float absX = std::abs(x);
    if (absX <= 1.0f) {
        return x - (x * x * x) / 3.0f;
    }
    return (x > 0.0f ? 2.0f : -2.0f) / 3.0f;
}

class DcBlocker {
public:
    DcBlocker() = default;

    void reset(float sampleRate) {
        z1 = 0.0f;
        setCutoff(5.0f, sampleRate);
    }

    void setCutoff(float cutoff, float sampleRate) {
        const float x = std::exp(-2.0f * static_cast<float>(M_PI) * cutoff / sampleRate);
        a1 = -x;
        b0 = (1.0f + a1) * 0.5f;
        b1 = -b0;
    }

    float process(float input) noexcept {
        const float y = sanitize(b0 * input + b1 * x1 - a1 * z1);
        x1 = input;
        z1 = y;
        return sanitize(y);
    }

private:
    float a1{0.0f};
    float b0{1.0f};
    float b1{0.0f};
    float x1{0.0f};
    float z1{0.0f};
};

inline float preventDenorm(float v) {
    return isDenormal(v) ? 0.0f : v;
}

struct Biquad {
    float b0{1.0f};
    float b1{0.0f};
    float b2{0.0f};
    float a1{0.0f};
    float a2{0.0f};
    std::array<float, 2> z{{0.0f, 0.0f}};

    void reset() { z = {0.0f, 0.0f}; }

    float process(float x) noexcept {
        x = sanitize(x);
        float y = b0 * x + z[0];
        z[0] = b1 * x - a1 * y + z[1];
        z[1] = b2 * x - a2 * y;
        y = sanitize(y);
        return y;
    }
};

inline Biquad designPeak(float sampleRate, float freq, float q, float gainDb) {
    const float A = std::pow(10.0f, gainDb / 40.0f);
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float alpha = std::sin(omega) / (2.0f * q);
    const float cosw = std::cos(omega);

    Biquad bi;
    bi.b0 = 1.0f + alpha * A;
    bi.b1 = -2.0f * cosw;
    bi.b2 = 1.0f - alpha * A;
    bi.a1 = -2.0f * cosw;
    bi.a2 = 1.0f - alpha / A;

    const float a0 = 1.0f + alpha / A;
    bi.b0 /= a0;
    bi.b1 /= a0;
    bi.b2 /= a0;
    bi.a1 /= a0;
    bi.a2 /= a0;
    return bi;
}

inline Biquad designLowpass(float sampleRate, float freq, float q) {
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float alpha = std::sin(omega) / (2.0f * q);
    const float cosw = std::cos(omega);
    const float a0 = 1.0f + alpha;

    Biquad bi;
    bi.b0 = (1.0f - cosw) * 0.5f / a0;
    bi.b1 = (1.0f - cosw) / a0;
    bi.b2 = bi.b0;
    bi.a1 = -2.0f * cosw / a0;
    bi.a2 = (1.0f - alpha) / a0;
    return bi;
}

inline Biquad designHighpass(float sampleRate, float freq, float q) {
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float alpha = std::sin(omega) / (2.0f * q);
    const float cosw = std::cos(omega);
    const float a0 = 1.0f + alpha;

    Biquad bi;
    bi.b0 = (1.0f + cosw) * 0.5f / a0;
    bi.b1 = -(1.0f + cosw) / a0;
    bi.b2 = bi.b0;
    bi.a1 = -2.0f * cosw / a0;
    bi.a2 = (1.0f - alpha) / a0;
    return bi;
}

} // namespace gn
