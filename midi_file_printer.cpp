//
// Created by Michael Skiles on 10/6/21.
//

#include "midi_file_printer.h"

void MidiChunk::gobble(FILE *filePtr) {

    // buffer, byte size, num bytes, file pointer
    fread(_buffer, sizeof(uint8_t), 4, filePtr);
}

const HeaderChunk &MidiFile::getHeader() const {
    return _header;
}

void MidiFile::setHeader(const HeaderChunk &header) {
    _header = header;
}

TrackChunk *MidiFile::getTracks() const {
    return _tracks;
}

void MidiFile::setTracks(TrackChunk *tracks) {
    _tracks = tracks;
}

int MidiFile::getNumTracks() const {
    return _numTracks;
}

void MidiFile::setNumTracks(int numTracks) {
    _numTracks = numTracks;
}
