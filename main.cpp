#include <iostream>
#include "midi_file_printer.h"

const char *file_path = "../midi-files/zelda-overworld.mid";

int main() {
    MidiFile mf = MidiFile(file_path);
    HeaderChunk header = HeaderChunk();
    header.gobble(mf.getFilePtr());
    header.processHeader();
    TrackChunk *tracks = new TrackChunk[header.getNumTracks()];
    for (int i = 0; i < 1; i++) {
        tracks[i].gobble(mf.getFilePtr());
    }
    uint8_t temp[8];
    fread(temp, 8, 1, mf.getFilePtr());
    fread(temp, 8, 1, mf.getFilePtr());
    MidiFile::parseVariableLengthQuantity(temp, 0);
}
