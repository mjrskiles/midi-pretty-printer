//
// Created by Michael Skiles on 10/6/21.
//

#include "midi_file_printer.h"

uint8_t header_chunk_byte_pattern[] {0x4d, 0x54, 0x68, 0x64};
uint8_t track_chunk_byte_pattern[] {0x4d, 0x54, 0x72, 0x6b};

bool compareArrays(uint8_t *a1, uint8_t *a2, int commonSize) {
    for (int i = 0; i < commonSize; i++) {
        if (a1[i] != a2[i]) {
            return false;
        }
    }
    return true;
}

void printBuffer(uint8_t *buf, int size) {
    printf("Buffer:\n");
    for (int i = 0; i < size; i++) {
        buf[i] > 15 ? printf("%x ", buf[i]) : printf("0%x ", buf[i]);
        if ((i+1) % 4 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void MidiChunk::gobble(FILE *filePtr) {
    uint8_t chunk_type_bytes[4];
    uint8_t *bytes_ptr = chunk_type_bytes;
    uint8_t *hpattern_ptr = header_chunk_byte_pattern;
    uint8_t *tpattern_ptr = track_chunk_byte_pattern;
    // buffer, byte size, num bytes, file pointer
//    fread(chunk_type_bytes, sizeof(uint8_t), 4, filePtr);
    fread(chunk_type_bytes, 4, 1, filePtr);
    if (compareArrays(bytes_ptr, hpattern_ptr, 4)) {
        _chunkType = CHUNK_TYPE_HEADER;
    } else if (compareArrays(bytes_ptr, tpattern_ptr, 4)) {
        _chunkType = CHUNK_TYPE_TRACK;
    }

    uint8_t chunk_length_bytes[4];
    fread(chunk_length_bytes, 4, 1, filePtr);
    _chunkLength |= chunk_length_bytes[0] << 24;
    _chunkLength |= chunk_length_bytes[1] << 16;
    _chunkLength |= chunk_length_bytes[2] << 8;
    _chunkLength |= chunk_length_bytes[3] << 0;

    printf("Read %d as length\n", _chunkLength);

    uint8_t *buffer = new uint8_t[_chunkLength];
    fread(buffer, _chunkLength, 1, filePtr);
    _buffer = buffer;
    printBuffer(_buffer, _chunkLength);
}

const HeaderChunk &MidiFile::getHeader() const {
    return _header;
}

void HeaderChunk::processHeader() {
    if (_chunkLength != 6) {
        printf("Header chunk was larger than 6.");
    } else {
        _format |= _buffer[0] << 8;
        _format |= _buffer[1];
        _numTracks |= _buffer[2] << 8;
        _numTracks |= _buffer[3];
        _division |= _buffer[4] << 8;
        _division |= _buffer[5];
    }
}

uint16_t HeaderChunk::getFormat() const {
    return _format;
}

uint16_t HeaderChunk::getNumTracks() const {
    return _numTracks;
}

uint16_t HeaderChunk::getDivision() const {
    return _division;
}

TrackChunk *MidiFile::getTracks() const {
    return _tracks;
}

void TrackChunk::processTrack() {

}

TrackEvent TrackChunk::readTrackEvent() {
    uint32_t time = MidiFile::parseVariableLengthQuantity(_buffer, _bufferOffset);
    uint8_t status = readByte();
    MidiMessage message;
    if (status == 0xff) {
        message = readMetaEvent(status);
    }
    if (status == 0xf0) {
        message = readSysexEvent(status);
    } else {
        message = readMidiEvent(status);
    }
    TrackEvent trackEvent = TrackEvent();
    trackEvent.deltaT = time;
    trackEvent.event = message;
    return trackEvent;
}

/*
msm_note_off = 0x80
msm_note_on  = 0x90
msm_control_change = 0xb0
msm_program_change = 0xc0
msm_sysex = 0xf0

# sysex messages
sysex_meta_event = 0xff
meta_track_name = 0xff_03 len text
meta_end_of_track = 0xff_02_00
meta_set_tempo = 0xff_51_03 # tt_tt_tt time in microseconds per quarter note
meta_time_sig = 0xff_58_04 # nn dd cc bb : numerator (dec), denom (dd^-2), midi clocks/metronome tick, 32nd notes per quarter
meta_key_sig = 0xff_59_02 # sf mi : sharp/flats major/minor -7 = 7 flats, 0 = C, 7 = 7 sharps; 0 = major 1 minor
 */

MidiMessage TrackChunk::readMidiEvent(uint8_t command) {
    return MidiMessage();
}

MidiMessage TrackChunk::readSysexEvent(uint8_t command) {
    return MidiMessage();
}

MidiMessage TrackChunk::readMetaEvent(uint8_t command) {
    uint8_t metaCommand = readByte();
    MidiMessage message = MidiMessage();
    switch (metaCommand) {
        case 0x02:
            if (readByte() == 0) {
                _reachedEOT = true;
            }
            break;
        case 0x03:
            uint32_t len = MidiFile::parseVariableLengthQuantity(_buffer, _bufferOffset);
            uint8_t *eventBytes = readBytes(len);
    }
}

uint8_t TrackChunk::readByte() {
    return _buffer[_bufferOffset++];
}

uint8_t *TrackChunk::readBytes(uint32_t amount) {
    uint8_t *temp = new uint8_t [amount];
    for (int i = 0; i < amount; i++) {
        temp[i] = readByte();
    }
    return temp;
}

void MidiFile::setHeader(const HeaderChunk &header) {
    _header = header;
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

MidiFile::MidiFile(const char* fileName) {
    _filePtr = fopen(fileName, "r");
    printf("Opened file at %x\n", _filePtr);
}

FILE *MidiFile::getFilePtr() const {
    return _filePtr;
}

uint32_t MidiFile::parseVariableLengthQuantity(uint8_t *buffer, uint32_t& offset) {
    uint8_t buf[4]; // max size of a VLQ is going to be 4 bytes
    uint8_t tempBuf[1];
    int numRead = 0;
    uint8_t lastRead = 0;
    do {
        lastRead = buffer[offset];
        buf[numRead] = lastRead & 127;
        numRead++;
        offset++;
    } while (lastRead & 128);
    printf("Read %d byte VLQ\n", numRead);
    uint32_t vlq = 0;
    for (int i = 0; i < numRead; i++) {
        vlq |= buf[i] << ((numRead - i - 1) * 7);
    }
    return vlq;
}
