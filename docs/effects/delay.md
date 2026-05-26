# Delay Effect

Stereo delay with LFO modulation and DC-blocked feedback path.

## Signal Flow

```
Input ──► Delay Buffer (L) ──► DC Block ──► × Feedback ──► mix ──► Output
       └──────────────────────────────────────────────────────────┘
              (same for R channel, independent write/read positions)
```

## Knob Map

| Knob | Parameter | Range |
|---|---|---|
| 1 (A1) | Delay Time Left | 1 ms – 250 ms |
| 2 (A2) | Delay Time Right | 1 ms – 250 ms |
| 3 (A3) | Feedback | 0% – 90% |
| 4 (A4) | Wet/Dry Blend | Full dry → Full wet |
| 5 (A5) | LFO Rate (modulation speed) | 0.1 Hz – 3 Hz |
| 6 (A6) | LFO Depth (modulation depth) | 0 – 10 ms |

## Footswitch / LED Behaviour

| Action | Result |
|---|---|
| FS-A tap | Bypass toggle |
| FS-A hold (1 s) | — (no effect in single-effect firmware) |
| Hold FS-A 3 s | Reboot to DFU mode |

| State | LED 1 | LED 2 |
|---|---|---|
| Active | On | Brightness = wet amount |
| Bypassed | Off | Off |

## Implementation Notes

- Maximum delay time: 500 ms (configurable in `DelayConfig::maxDelaySeconds`)
- LFO modulates both channels from the same oscillator (coherent modulation)
- DC block filter on feedback path (`R = 0.995`, cutoff ≈ 5 Hz) prevents low-frequency accumulation
- Delay time smoothing: α = 0.001 per sample (very slow, ~1s to full change)
