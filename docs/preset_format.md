# Preset Format

Preset banks are JSON files shaped for fast parsing and validation.

```json
{
  "presets": [
    {
      "name": "Studio Clean",
      "tags": ["clean", "studio"],
      "parameters": {
        "toneHp": 70,
        "toneLp": 9500,
        "presenceDb": 1.5,
        "noise": 0.05
      }
    }
  ]
}
```

- `name`: User-visible string.
- `tags`: Free-form labels used by the preset browser.
- `parameters`: Numeric map applied to `ParamSnapshot` and consumed in the audio thread.
