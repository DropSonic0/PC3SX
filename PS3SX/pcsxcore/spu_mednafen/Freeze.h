#ifndef SPU_FREEZE_H
#define SPU_FREEZE_H

#include <stdint.h>

namespace PSX {
namespace SPU {

struct Envelope_State {
    int State;
    int AttackModeExp;
    int AttackRate;
    int DecayRate;
    int SustainLevel;
    int SustainModeExp;
    int SustainIncrease;
    int SustainRate;
    int ReleaseModeExp;
    int ReleaseRate;
    int EnvelopeVol;
    int EnvelopeVol_f;
    long lVolume;
};

struct SampleStream_State {
    uint32_t Start;
    uint32_t Current;
    uint32_t Loop;
    int32_t DecodeBuffer[28];
    uint32_t Index;
    int32_t PreviousSamples[2];
};

struct Interpolate_State {
    uint32_t Index;
    uint32_t Buffer[4];
};

struct Channel_State {
    uint32_t DecodeStream;
    int HasDecodeBuffer;
    int SamplePosition;
    int SampleIncrement;
    int bReverb;
    int bRVBActive;
    int bNoise;
    int bFMod;
    uint32_t RawPitch;
    int32_t Volume[2];
    Envelope_State ADSR;
    SampleStream_State Sample;
    Interpolate_State Filter;
};

struct DiscStreamer_State {
    uint32_t Feed;
    uint32_t Play;
    uint32_t Frequency;
    double Speed;
    double Position;
    int32_t Volume[2];
    int16_t Data[65536 * 2];
};

struct NoiseGenerator_State {
    int16_t Value;
    uint32_t Count;
    uint32_t Clock;
};

struct SPU_State {
    uint32_t CycleCounter;
    uint32_t StreamIndex;
    uint16_t Control;
    uint16_t Status;
    uint32_t Address;
    uint16_t IRQAddress;
    int IRQHit;
    uint16_t RegisterArea[0x10000];
    uint8_t Memory[512 * 1024];
    Channel_State Channels[24];
    DiscStreamer_State Streamer;
    NoiseGenerator_State Noise;
};

}
}

#endif
