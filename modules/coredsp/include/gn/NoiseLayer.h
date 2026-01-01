#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

namespace gn {

class NoiseLayer {
public:
    void prepare(double sampleRate, int maxBurstSamples);
    void setToggles(bool pickEnabled, bool squeakEnabled, bool thumpEnabled);
    void setAmount(float amount);
    void triggerPick(float velocity);
    void triggerSqueak(float intensity);
    void triggerThump(float velocity);
    void render(float* buffer, int numSamples);

private:
    struct Burst {
        float amplitude{0.0f};
        float decay{0.0f};
        int remaining{0};
        float filter{0.0f};
    };

    std::array<Burst, 8> bursts{};
    size_t head{0};
    double sampleRate{48000.0};
    int maxLength{256};
    float amount{0.0f};
    bool pick{true};
    bool squeak{true};
    bool thump{true};
    uint32_t rng{0x1337u};

    void pushBurst(float amplitude, float decay, int lengthSamples);
    float nextNoise();
};

} // namespace gn

