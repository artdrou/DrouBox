# Looper Effect

60-second mono loop recorder with overdub and playback. Audio buffer lives in SDRAM.

## Signal Flow

```
Input ──► Record to SDRAM buffer ──►  +  ──► Output
                                      ▲
              Playback from SDRAM ────┘
```

## Controls

| Control | Action |
|---|---|
| Footswitch 1 | Toggle record (tap to start, tap again to stop and auto-play) |
| Footswitch 2 | Toggle playback |

## LED Feedback

| LED 1 | LED 2 | State |
|---|---|---|
| On | Off | Recording |
| Off | On | Playing |
| Off | Off | Idle |

## Buffer

- Allocated in SDRAM: `float DSY_SDRAM_BSS loopBuffer[MAX_SAMPLES]`
- Maximum: 60 seconds at 48 kHz = 2,880,000 samples = ~11 MB
- Cleared automatically on each new recording

## Notes

- Recording auto-stops when the buffer is full (60 s), then immediately starts playback
- Loop plays back from position 0 to `recordedLength` in a seamless loop
- No overdub in the current implementation — each new recording clears the previous loop
