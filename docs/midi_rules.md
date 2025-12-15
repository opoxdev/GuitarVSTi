# MIDI to Performance Rules

- **Strum**: Chords are expanded into staggered `PlayEvent`s. Direction (up/down) sets ordering, and humanization width is defined in samples.
- **Legato**: Overlapping note-ons within `legatoSamples` distance are tagged as legato articulations for smoother transitions.
- **Mutes**: Very low velocities map to `Articulation::Dead`; external control can repurpose the articulation field for palm-mutes.
- **Slides**: `PlayEvent::slideTarget` can be used by the DSP layer to move waveguide tuning without retriggering.
