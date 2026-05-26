# Distortion Effect

10 distortion algorithms selectable via DIP switches, with an optional compressor (pre or post) and shelf EQ.

## Signal Flow

```
Input ──► Compressor (optional) ──► Distortion ──► Low Shelf ──► High Shelf ──► Output
```

## Knob Map

| Knob | Parameter | Range |
|---|---|---|
| 1 (A1) | Drive | Low → High (power-mapped, 1 – 300×) |
| 2 (A2) | Tone (shelf frequency) | 20 Hz – 8 kHz |
| 3 (A3) | Level (output volume) | 0 – 2× |
| 4 (A4) | Sustain (compressor threshold + ratio) | Clean → Heavy compression |
| 5 (A5) | Attack (compressor attack time) | 5 ms – 200 ms |
| 6 (A6) | Blend (compressor wet/dry) | Dry → Full compression |

## DIP Switch: Distortion Type

| DIP value (binary) | Type |
|---|---|
| 0 | Hard Clip |
| 1 | Tanh Clip (soft saturation) |
| 2 | Arctan Clip |
| 3 | Log Clip |
| 4 | Exp Clip |
| 5 | Sin Clip |
| 6 | Half Wave |
| 7 | Full Wave Reflection |
| 8 | Bit Crush |
| 9 | Tube Amp (asymmetric saturation) |

## On-Off-On Switch 3: Compressor Position

| Position | Compressor |
|---|---|
| LEFT | Pre-distortion (squash input, then clip) |
| CENTER | Disabled |
| RIGHT | Post-distortion (compress after clipping) |

## Algorithm Details

All algorithms map `drive` knob via `powerMap(d, 0, 1, 1, 300, 1.75)` for a musical feel (more resolution at low drive). All include a `computeMakeupGain()` to compensate for level changes at high drive.

| Algorithm | Character |
|---|---|
| Hard Clip | Aggressive, squared-off waveform |
| Tanh Clip | Smooth saturation, tube-like |
| Arctan Clip | Similar to tanh, slightly brighter |
| Log Clip | Compressed dynamics, sustains long |
| Exp Clip | Asymptotic, fades in gradually |
| Sin Clip | Folds on itself, rich harmonics |
| Half Wave | Asymmetric, gated lows |
| Full Wave | All-positive rectification, octave effect |
| Bit Crush | Digital aliasing, lo-fi character |
| Tube Amp | Asymmetric soft-clip, models triode stage |
