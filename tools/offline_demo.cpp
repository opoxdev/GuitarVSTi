#include "gn/CoreVoice.h"
#include "gnperf/PerformanceEngine.h"
#include "gnpreset/PresetEngine.h"
#include <fstream>
#include <iostream>
#include <vector>

int main() {
    const double sampleRate = 48000.0;
    const int numSamples = static_cast<int>(sampleRate * 1.0);
    gn::CoreVoice voice;
    voice.prepare(sampleRate, static_cast<int>(sampleRate));
    gn::VoiceParams params;
    params.toneHp = 80.0f;
    params.toneLp = 8000.0f;
    params.presenceDb = 2.0f;
    params.noise = 0.1f;
    voice.start(110.0f, 0.8f, params);

    std::vector<float> buffer(numSamples, 0.0f);
    voice.render(buffer.data(), numSamples);

    std::ofstream out("demo.raw", std::ios::binary);
    for (float sample : buffer) {
        out.write(reinterpret_cast<const char*>(&sample), sizeof(sample));
    }
    std::cout << "Rendered demo.raw with " << numSamples << " samples" << std::endl;
    return 0;
}
