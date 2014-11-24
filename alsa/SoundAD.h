#ifndef _SOUNDAD_
#define _SOUNDAD_ 

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

#ifndef SOUND_AMP_RATE
#define SOUND_AMP_RATE 6.63
#endif

class SoundAD
{
private:
	int rc;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_format_t pcm_format;
	unsigned int sampleRate;
	snd_pcm_uframes_t frames;
	// int dir;
	unsigned int channels;
	// float record_max;

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
	
};

#endif //~_SOUNDAD_ 