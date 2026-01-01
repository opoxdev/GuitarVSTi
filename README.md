# GuitarNexus Monorepo

GuitarNexus is a JUCE-style VST3 instrument optimized for expressive guitar performance. The repository is structured as a single monorepo containing RT-safe DSP, performance translation, preset handling, tools, and the plugin wrapper.

## Build

```bash
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

On Windows/Visual Studio 2022 you can run `scripts/build_windows_vs2022.bat`.

JUCE is optional for headless builds. For Windows VST3 builds with JUCE:

1. Download JUCE 7 and extract it to `third_party/juce` (or any path of your choice).
2. Configure with the local path: `cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DGUITARNEXUS_ENABLE_JUCE=ON -DJUCE_DIR=third_party/juce`
3. Build: `cmake --build build --config Release`

If you cannot supply a local JUCE checkout, you may opt-in to FetchContent by adding `-DGUITARNEXUS_USE_FETCHCONTENT_JUCE=ON` during configuration, but the default is to rely on `JUCE_DIR` for robustness.
