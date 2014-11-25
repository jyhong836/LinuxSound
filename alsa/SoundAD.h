#ifndef _SOUNDAD_
#define _SOUNDAD_ 

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include "fft/FFT.h"

#define SOUND_AMP_RATE 6.63

#define DEFAULT_PERIOD_NUM  21

#define DEFAULT_FIELT_FREQ_MIN  10
#define DEFAULT_FIELT_FREQ_MAX  100
#define DEFAULT_WINDOW_MIN		(DEFAULT_FFT_LEN*1/10)
#define DEFAULT_WINDOW_MAX		(DEFAULT_FFT_LEN*9/10)

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
	float *buffer1;
	float *buffer2;

public:
	unsigned int sampleRate;
	snd_pcm_uframes_t frames;
	unsigned period_time;

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
	int  InitDataBuffer();

public:
	// (float *&signal, float *&frequency); they are both DEFAULT_FFT_LEN(FFT.h) in length.
	int GetData(float *&, float *&); 
private:
	int GetData(float *, int);
	int GetData(float *, int, float *);
	int GetData(float *);

public:
	float GetValue(); // return the max value of signal
	float GetValue(int, int);
	float GetValue(int, int, int, int);
	
};

#endif //~_SOUNDAD_ 