#ifndef _SOUNDAD_
#define _SOUNDAD_ 

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include "fft/FFT.h"

#ifndef SOUND_AMP_RATE
#define SOUND_AMP_RATE 6.63
#endif

#ifndef DEFAULT_PERIOD_NUM
#define DEFAULT_PERIOD_NUM  21
#endif

class SoundAD
{
private:
	int rc;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_format_t pcm_format;
	// int dir;
	unsigned int channels;
	// float record_max;
	char *data;
	FFT  fft;
	int sampleNum;

public:
	unsigned int sampleRate;
	snd_pcm_uframes_t frames;

public:
	SoundAD();
	~SoundAD();
	void DisplayConfigure();
	void OpenDevice();
	int  Configure();
	void CloseDevice();
	int  Record(int);
	int  DisplayALSAEnabled();
	int  PrintBuffer(char *, size_t);
	int GetData(float *, int);
	int GetData(float *, int, float *);
	int GetData(float *, float *);
	int GetData(float *);
	
};

#endif //~_SOUNDAD_ 