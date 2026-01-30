#ifndef __CELLAUDIOPORT_H
#define __CELLAUDIOPORT_H

#include <stdint.h>

// Specific to application
typedef int16_t audio_input_t;

bool cellAudioPortInit(uint64_t samplerate, uint64_t buffersize);

// Write data to buffer. Make sure that samples < buffersize.
void cellAudioPortWrite(const audio_input_t* buffer, uint64_t samples);

// How many samples can we write to buffer without blocking?
uint64_t cellAudioPortWriteAvail();
void cellAudioPortExit();

#endif

