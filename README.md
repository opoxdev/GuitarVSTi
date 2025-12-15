# GuitarNexus Monorepo

GuitarNexus is a JUCE-style VST3 instrument optimized for expressive guitar performance. The repository is structured as a single monorepo containing RT-safe DSP, performance translation, preset handling, tools, and the plugin wrapper.

## Build

```bash
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

On Windows/Visual Studio 2022 you can run `scripts/build_windows_vs2022.bat`.

JUCE is optional for headless builds; enabling `-DGUITARNEXUS_ENABLE_JUCE=ON` fetches JUCE with FetchContent and produces a VST3-style shared library.
