#pragma once
#include "stdafx.h"
#include <inttypes.h>
#include <string>
#include <iostream>
//Audio
#include <AL/al.h>
#include <AL/alc.h>
#define BACKEND "alut"
#define NUM_BUFFERS 3
#define BUFFER_SIZE 4096
using namespace std;
typedef struct {
	/* RIFF Chunk Descriptor */
	uint8_t  riff[4];//'RIFF'
	uint32_t riffSize;
	uint8_t  wave[4];//'WAVE'
	uint8_t  fmt[4];//'fmt '
	uint32_t fmtSize;
	uint16_t format; // Audio format 1 = PCM, 6 = mulaw, 7 = alaw, 257 = IBM Mu - Law, 258 = IBM A - Law, 259 = ADPCM
	uint16_t channels; // Number of channels 1=Mono 2=Sterio
	uint32_t samplesPerSec;// Sampling Frequency in Hz
	uint32_t bytesPerSec;
	uint16_t blockAlign; // 2=16-bit mono, 4=16-bit stereo
	uint16_t bitsPerSample;
	uint8_t list[4];
	uint32_t listSize;
}BasicWAVEHeader;

class Sound
{
private:
	ALCdevice *device = NULL;
	ALCcontext *context = NULL;
	uint8_t  data[4];//'data'
	uint32_t dataSize[1];
	ALuint buffer;
	ALuint source = 0;

	char* readWAV(string const & filename, BasicWAVEHeader* header);
	ALuint createBufferFromWave(char* data, BasicWAVEHeader header);
	void PlayOnThread();
public:
	void Play(bool isforcePlay = true);
	void Init(string const &path);
	void GetError();
	string GetSourceState(ALuint source);
	Sound();
	~Sound();
};

