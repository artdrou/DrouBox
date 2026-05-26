# Tuner Effect

Chromatic tuner using the YIN pitch detection algorithm. Passes audio through unmodified.

## Signal Flow

```
Input ──► Circular Buffer (2048 samples) ──► Output
               │
               ▼ (every 10 ms)
          YIN Pitch Detection
               │
               ▼
          Closest String Match
               │
               ▼
          LED Feedback
```

## LED Feedback

| LED 1 | LED 2 | Meaning |
|---|---|---|
| Bright (× certainty) | Bright (× certainty) | In tune |
| On (× closeness) | Off | Pitch too low — tune up |
| Off | On (× closeness) | Pitch too high — tune down |

Tolerance: ±0.5 Hz from target frequency.

## Guitar String Frequencies

| String | Note | Frequency |
|---|---|---|
| 6 | E2 | 82.41 Hz |
| 5 | A2 | 110.00 Hz |
| 4 | D3 | 146.83 Hz |
| 3 | G3 | 196.00 Hz |
| 2 | B3 | 246.94 Hz |
| 1 | E4 | 329.63 Hz |

## YIN Algorithm Parameters

| Parameter | Value | Notes |
|---|---|---|
| Buffer size | 2048 samples | ~42 ms at 48 kHz |
| Decimation | 4× | Effective SR = 12 kHz |
| Min frequency | 75 Hz | Covers low E string |
| Max frequency | 15000 Hz | Covers harmonics |
| Threshold | 0.15 | CMND threshold |
| Interpolation | Cubic | Sub-sample accuracy |
| Smoothing α | 0.2 | Frequency output smoothing |

## Known Limitations

- Low E string (82 Hz) detection can be unreliable with 4× decimation (effective SR = 12 kHz gives only ~145 samples per period). Increase buffer or reduce decimation for better low-end accuracy.
- Octave errors are possible for strings with rich harmonics (e.g., open G).
- Detection takes ~10 ms per update cycle (CPU-intensive YIN computation runs in the main loop, not the audio ISR).
