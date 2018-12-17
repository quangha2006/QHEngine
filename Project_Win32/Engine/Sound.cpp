#include "Sound.h"
#include "Utils.h"

//WARNING: This Doesn't Check To See If These Pointers Are Valid
char* Sound::readWAV(string const & filename, BasicWAVEHeader* header) {
	char* buffer = 0;
	FILE* file = fopen(filename.c_str(), "rb");
	if (!file) {
		LOGE("Audio handle error : Open %s\n", filename.c_str());
		return NULL;
	}
	if (fread(header, sizeof(BasicWAVEHeader), 1, file)) {

		if (memcmp("data ", header->list, 4))
		{

			fseek(file, header->listSize, SEEK_CUR);
			fread(data, sizeof(data), 1, file);
			fread(dataSize, sizeof(dataSize), 1, file);
		}
		else dataSize[0] = header->listSize;

		//LOGI("ChunkID: %.4s\nChunkSize: %d\nFormat: %.4s\nSubchunk1ID: %4s\nSubchunk1Size: %d\nAudioFormat: %u\nNumChannels: %u\nsamplesPerSec: %d\nbytesPerSec: %d\nblockAlign: %u\nbitsPerSample: %d\nlist size: %d\ndata: %.4s\ndatasize: %d\n", header->riff, header->riffSize, header->wave, header->fmt, header->fmtSize, header->format, header->channels, header->samplesPerSec, header->bytesPerSec, header->blockAlign, header->bitsPerSample, header->listSize, data, dataSize[0]);

		if (!(memcmp("RIFF", header->riff, 4) ||
			memcmp("WAVE", header->wave, 4) ||
			memcmp("fmt ", header->fmt, 4)))
		{
			//dataSize[0] = header->riffSize - 72;
			buffer = (char*)malloc(dataSize[0]);
			if (buffer) {
				if (fread(buffer, dataSize[0], 1, file)) {
					fclose(file);
					return buffer;
				}
				free(buffer);
			}
		}
	}
	fclose(file);
	return buffer;
}
ALuint Sound::createBufferFromWave(char* data, BasicWAVEHeader header) 
{
	ALuint buffer = 0;
	ALCenum format = 0;
	switch (header.bitsPerSample) {
	case 8:
		format = (header.channels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
		break;
	case 16:
		format = (header.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		break;
	default:
		return 0;
	}

	alGenBuffers((ALuint)1, &buffer);

	alBufferData(buffer, format, data, dataSize[0], header.samplesPerSec);

	return buffer;
}
void Sound::Play()
{
	int sourceState = AL_PLAYING;
	alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
	if (sourceState != AL_PLAYING)
		alSourcePlay(source);
}
void Sound::Init(string const &path)
{

	string path_modif = Utils::getResourcesFolder() + path;

	const ALint context_attribs[] = { ALC_FREQUENCY, 44100, 0 };

	const char * devicename = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	device = alcOpenDevice(devicename);

	LOGW("======================================================\n\n");
	if (!device)
	{
		LOGE("Audio handle error: device NULL!\n");
		return;
	}

	context = alcCreateContext(device, context_attribs);

	if (!alcMakeContextCurrent(context))
	{
		ALCenum error = alGetError();
		if (error != AL_NO_ERROR)
			LOGE("Audio handle error: make context current: %d\n", error);
		return;
	}

	BasicWAVEHeader header;
	char* data = readWAV(path_modif, &header);

	if (!data)
	{
		return;
	}
	LOGI("Load audio file: %s\n", path_modif.c_str());
		//Now We've Got A Wave In Memory, Time To Turn It Into A Usable Buffer
	buffer = createBufferFromWave(data, header);
	if (!buffer) 
	{
		free(data);
		LOGE("Audio handle error: create Buffer From Wave NULL!\n");
		return;
	}

	alGenSources(1, &source);

	alSourcei(source, AL_BUFFER, buffer);
	GetError();

	// Play source
	//alSourcePlay(source);
	//alSourcei(source, AL_LOOPING, AL_FALSE);
	GetError();
}
void Sound::GetError()
{
	ALCenum error;
	error = alGetError();
	if (error != AL_NO_ERROR)
		LOGE("Audio handle error: %d\n", error);
}

string Sound::GetSourceState(ALuint source)
{
	int sourceState = 0x1014;
	string state = "Unknown";
	alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
	switch (sourceState)
	{
	case AL_INITIAL: state = "AL_INITIAL";  break;
	case AL_PLAYING: state = "AL_PLAYING"; break;
	case AL_PAUSED: state = "AL_PAUSED";  break;
	case AL_STOPPED: state = "AL_STOPPED";  break;
	default: break;
	}
	return state;
}

Sound::Sound()
{
}


Sound::~Sound()
{
	alcMakeContextCurrent(0);
	alcDestroyContext(context);
	alcCloseDevice(device);
	alDeleteBuffers(1, &buffer);
}
