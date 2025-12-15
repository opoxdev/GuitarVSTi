# Real-time Safety Rules

- No dynamic allocation, locks, or file IO from the audio thread.
- Avoid throwing exceptions in DSP paths; operate with branch-free guards where possible.
- Run denormal mitigation: tiny values are zeroed, and waveguide buffers are preallocated.
- Sanitize every sample (NaN/Inf -> 0) and run a DC blocker before final output.
- Apply soft clipping to avoid runaway levels and keep the waveguide stable.
- Parameter changes are smoothed; `SmoothValue` uses exponential smoothing to avoid zipper noise.
