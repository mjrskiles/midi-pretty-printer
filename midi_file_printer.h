//
// Created by Michael Skiles on 10/6/21.
//

#ifndef MIDI_PRINTER_MIDI_FILE_PRINTER_H

#include <iostream>
#include <fstream>

#define CHUNK_TYPE_HEADER 0
#define CHUNK_TYPE_TRACK 1

using namespace std;

struct MidiMessage {
    uint8_t status;
    uint8_t *data;
    int len;
};

struct TrackEvent {
    uint32_t deltaT;
    MidiMessage event;
};

class MidiChunk {
public:
    /*
     * Read bytes and put them in the buffer
     * the first 8 bytes of a midi chunk are in the format
     * tttt llll, aka <chunk type> <length>
     */
    void gobble(FILE *filePtr);
protected:
    int _chunkType;
    uint8_t *_buffer;
    uint32_t _size;
};

/*
<Header Chunk> = <chunk type><length><format><ntrks><division>
 */
class HeaderChunk : public MidiChunk {
public:
    HeaderChunk() : MidiChunk() {
        _chunkType = CHUNK_TYPE_HEADER;
    }

    /*
     * Take the byte array and parse it into objects
     */
    void parse();
protected:
};

/*
 *
<Track Chunk> = <chunk type><length><MTrk event>+
<MTrk event> = <delta-time><event>
<event> = <MIDI event> | <sysex event> | <meta-event>
 */
class TrackChunk : public MidiChunk {
public:
    TrackChunk() : MidiChunk(){
        _chunkType = CHUNK_TYPE_TRACK;
    }

    void parse();
protected:
};

class MidiFile {
public:
    MidiFile(string fileName);

    static uint32_t parseVariableLengthQuantity(FILE *filePtr);

    const HeaderChunk &getHeader() const;

    void setHeader(const HeaderChunk &header);

    TrackChunk *getTracks() const;

    void setTracks(TrackChunk *tracks);

    int getNumTracks() const;

    void setNumTracks(int numTracks);

protected:
    FILE *_filePtr;
    HeaderChunk _header;
    TrackChunk *_tracks;
    int _numTracks;
};

#define MIDI_PRINTER_MIDI_FILE_PRINTER_H

#endif //MIDI_PRINTER_MIDI_FILE_PRINTER_H
