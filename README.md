# midi-pretty-printer

A quick proof of concept project to gain familiarity parsing MIDI files.

It's not guaranteed to work with all midi files as I've only implemented the commands I need.
It should work reasonably well parsing type 0 or 1 MIDI files, hopefully ignoring commands it doesn't understand.

## MIDI status words implemented

n = channel number


Regular MIDI events

- 0x8n : Note off
- 0x9n : Note on
- 0xbn : control change
- 0xcn : program change

Sysex Events

- 0xff 03 len text : Meta - Track Name
- 0xff 02 00 : Meta - end of track
- 0xff 51 03 tt tt tt : Meta - Tempo
- 0xff 58 04 nn dd cc bb : Meta - time signature (numerator (dec), denom (dd^-2), midi clocks/metronome tick, 32nd notes per quarter)
- 0xff 59 02 sf mi : Meta - Key signature (sharp/flats major/minor -7 = 7 flats, 0 = C, 7 = 7 sharps; 0 = major 1 minor)
