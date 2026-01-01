# GuitarNexus Architecture

GuitarNexus is a monorepo that keeps DSP, performance mapping, preset handling, tools, and the plugin wrapper in a single tree. The code is intentionally RT-safe: audio threads avoid allocations, locks, file IO, and exceptions.

## Modules
- **modules/coredsp**: Waveguide string voice, pick exciter, modal body filters, tone shaping, and safety utilities (NaN guards, DC blockers, denormal suppression, soft clip).
- **modules/performance**: Translates MIDI into `PlayEvent` streams with strumming and legato-aware timing. Uses fixed-size ring buffers for RT safety.
- **modules/presetengine**: Loads and validates preset banks, produces `ParamSnapshot` objects to be shared atomically with audio.
- **plugin/guitarnexus_vsti**: JUCE-style VST3 instrument wrapper (stubbed to be buildable without JUCE). Hosts a processor, preset browser UI skeleton, and macro panel definitions.
- **tools**: `presetgen` creates bulk presets from templates; `bankpacker` converts JSON banks into a compact binary format; optional `guitarnexus_offline_demo` renders without a host.

## Data Flow
1. GUI or preset loader selects a preset; `PresetDatabase` returns a `ParamSnapshot` ready for the audio thread.
2. MIDI enters the plugin and is routed into `PerformanceEngine`, generating `PlayEvent` objects with strum offsets and articulations.
3. `CoreVoice` instances in `coredsp` render audio using waveguide strings, modal body resonance, tone filters, DC blocking, and soft clipping.
4. Output buffers include NaN/Inf guards, denormal protection, and gentle soft limiting to keep the signal stable.
