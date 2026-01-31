#include "stdafx.h"
#include "SPU.h"

#include "ps3audio.h"

#define AUDIO_BUFFER_SIZE 512
static uint16_t audio_buffer[AUDIO_BUFFER_SIZE * 2];
static uint32_t audio_buffer_pos = 0;

void SoundFeedSample(uint16_t left, uint16_t right)
{
	audio_buffer[audio_buffer_pos * 2] = left;
	audio_buffer[audio_buffer_pos * 2 + 1] = right;
	audio_buffer_pos++;

	if (audio_buffer_pos >= AUDIO_BUFFER_SIZE)
	{
		cellAudioPortWrite(audio_buffer, AUDIO_BUFFER_SIZE * 2);
		audio_buffer_pos = 0;
	}
}
