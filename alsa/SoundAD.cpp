/* 
* @Author: Junyuan Hong
* @Date:   2014-11-24 16:34:04
* @Last Modified by:   Junyuan Hong
* @Last Modified time: 2014-11-25 22:22:53
*/

#include <iostream>

#include "SoundAD.h"

// #define VERB
// #define CORRECT_AMP

using namespace std;

SoundAD::SoundAD():
handle(0),
// sampleRate(44100),
sampleRate(9600),
frames(204),
channels(1),
data(NULL),
buffer1(NULL),
buffer2(NULL)
{
	pcm_format = SND_PCM_FORMAT_FLOAT_LE;
	OpenDevice();
	Configure();
	// fft = new FFT();
	InitDataBuffer();
}

SoundAD::~SoundAD()
{
	CloseDevice();
}

int
SoundAD::InitDataBuffer()
{
	buffer1 = (float *)malloc(frames*DEFAULT_PERIOD_NUM*sizeof(float));
	buffer2 = (float *)malloc(frames*DEFAULT_PERIOD_NUM*sizeof(float));
	if (!buffer1 || !buffer2)
	{
		cerr << "malloc failed...exit" << endl;
		exit(-1);
	}
}

/**
 * Open pcm device and set default parameters.
 * The params initialized:
 * 		- handler
 * 		- params
 */
void 
SoundAD::OpenDevice()
{

#ifdef VERB
	cout << "Open device..." << endl;
#endif

	/* Open PCM device for recording (capture). */
	rc = snd_pcm_open(&handle, "default",
	                    SND_PCM_STREAM_CAPTURE, 0);
	if (rc < 0) {
	    fprintf(stderr,
	            "unable to open pcm device: %s\n",
	            snd_strerror(rc));
	    exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_malloc(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	// printf("%x %x\n", (int)params, (int)handle);

#ifdef VERB
	cout << "Open device...success" << endl;
#endif

}

/**
 * Display information about the PCM interface
 */
void
SoundAD::DisplayConfigure() 
{
	unsigned int val, val2;
	int dir;
	snd_pcm_uframes_t frames;

	cout << "===== DisplayConfigure =====" << endl;

	printf("PCM handle name = '%s'\n",
	         snd_pcm_name(handle));

	printf("PCM state = %s\n",
	         snd_pcm_state_name(snd_pcm_state(handle)));

	snd_pcm_hw_params_get_access(params,
	                          (snd_pcm_access_t *) &val);
	printf("access type = %s\n",
	         snd_pcm_access_name((snd_pcm_access_t)val));

	snd_pcm_hw_params_get_format(params, (snd_pcm_format_t*) &val);
	printf("format = '%s' (%s)\n",
	    snd_pcm_format_name((snd_pcm_format_t)val),
	    snd_pcm_format_description(
	                             (snd_pcm_format_t)val));

	snd_pcm_hw_params_get_subformat(params,
	                        (snd_pcm_subformat_t *)&val);
	printf("subformat = '%s' (%s)\n",
	    snd_pcm_subformat_name((snd_pcm_subformat_t)val),
	    snd_pcm_subformat_description(
	                          (snd_pcm_subformat_t)val));

	snd_pcm_hw_params_get_channels(params, &val);
	printf("channels = %d\n", val);

	snd_pcm_hw_params_get_rate(params, &val, &dir);
	printf("rate = %d bps\n", val);

	snd_pcm_hw_params_get_period_time(params,
	                                    &val, &dir);
	printf("period time = %d us\n", val);

	snd_pcm_hw_params_get_period_size(params,
	                                    &frames, &dir);
	printf("period size = %d frames\n", (int)frames);

	snd_pcm_hw_params_get_buffer_time(params,
	                                    &val, &dir);
	printf("buffer time = %d us\n", val);

	snd_pcm_hw_params_get_buffer_size(params,
	                         (snd_pcm_uframes_t *) &val);
	printf("buffer size = %d frames\n", val);

	snd_pcm_hw_params_get_periods(params, &val, &dir);
	printf("periods per buffer = %d frames\n", val);

	snd_pcm_hw_params_get_rate_numden(params,
	                                    &val, &val2);
	printf("exact rate = %d/%d bps\n", val, val2);

	val = snd_pcm_hw_params_get_sbits(params);
	printf("significant bits = %d\n", val);

	// snd_pcm_hw_params_get_tick_time(params,
	//                                   &val, &dir);
	printf("tick time = %d us\n", val);

	val = snd_pcm_hw_params_is_batch(params);
	printf("is batch = %d\n", val);

	val = snd_pcm_hw_params_is_block_transfer(params);
	printf("is block transfer = %d\n", val);

	val = snd_pcm_hw_params_is_double(params);
	printf("is double = %d\n", val);

	val = snd_pcm_hw_params_is_half_duplex(params);
	printf("is half duplex = %d\n", val);

	val = snd_pcm_hw_params_is_joint_duplex(params);
	printf("is joint duplex = %d\n", val);

	val = snd_pcm_hw_params_can_overrange(params);
	printf("can overrange = %d\n", val);

	val = snd_pcm_hw_params_can_mmap_sample_resolution(params);
	printf("can mmap = %d\n", val);

	val = snd_pcm_hw_params_can_pause(params);
	printf("can pause = %d\n", val);

	val = snd_pcm_hw_params_can_resume(params);
	printf("can resume = %d\n", val);

	val = snd_pcm_hw_params_can_sync_start(params);
	printf("can sync start = %d\n", val);

}

int 
SoundAD::Configure() 
{
#ifdef VERB
	cout << "Configure device..." << endl;
#endif

	unsigned int val, val2;
	int dir;

	// printf("%x %x\n", (int)params, (int)handle);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(handle, params, pcm_format); //(Float 32 bit Little Endian)
	                              // SND_PCM_FORMAT_S16_LE);

	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(handle, params, channels);

	/* 44100 bits/second sampling rate (CD quality) */
	// val = 44100;
	snd_pcm_hw_params_set_rate_near(handle, params, &sampleRate, &dir);

	/* Set period size to 204 frames. */
	// frames = 204;
	snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir); // frames will be updated

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
	    fprintf(stderr,
	            "unable to set hw parameters: %s\n",
	            snd_strerror(rc));
	    exit(1);
	}

	snd_pcm_hw_params_get_period_time(params, &period_time, &dir);

#ifdef VERB
	cout << "  mode:        " << snd_pcm_access_name(SND_PCM_ACCESS_RW_INTERLEAVED) << endl;
	cout << "  pcm format:  " << snd_pcm_format_name(pcm_format) << endl;
	cout << "  channels:    " << channels << "ch" << endl;
	cout << "  sample rate: " << sampleRate << " sps" << endl;
	cout << "  period size: " << frames << " frames" << endl;
	cout << "  period time: " << period_time << " us" << endl;
	cout << "Configure device...success" << endl;
#endif

	return 0;

}

/**
 * 
 * @return 		the number of frames
 */
int
SoundAD::Record(int period) {

	unsigned int val;
	int dir;
	char *buffer;
	size_t size;
	long loops;

	if (this->data==NULL) {
		perror("the buffer memory has not been malloced");
		return -1;
	}

	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	size = frames * 4; /* 2 bytes/sample, 2 channels */
	// buffer = (char *) malloc(size);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params, &val, &dir); // period time: 21333 us
	// loops = msec*1000 / val;
	loops = period;
	// record_max = 0;

	for (int i = 0; i < loops; i++) {
	// while (loops > 0) {

	    // loops--;
	    buffer = this->data + i*size;
	    rc = snd_pcm_readi(handle, buffer, frames);

	    if (rc == -EPIPE) {

			/* EPIPE means overrun */
			fprintf(stderr, "overrun occurred\n"); 
			snd_pcm_prepare(handle);

	    } else if (rc < 0) {

	    	fprintf(stderr,  "error from read: %s\n", snd_strerror(rc));

	    } else if (rc != (int)frames) {

	    	fprintf(stderr, "short read, read %d frames\n", rc);

	    }

	    // rc = write(1, buffer, size); // write to stdout
	    // rc = PrintBuffer(buffer, size);
	    // if (rc != size) 
	    // 	fprintf(stderr, "short write: wrote %d bytes\n", rc);
	}

	// free(buffer);

	// cout << "max: "<<record_max <<endl;

	return period*frames;

}

int
SoundAD::GetData(float *&sig, float *&freq)
{
	int ret = GetData(buffer1, DEFAULT_PERIOD_NUM, buffer2);
	sig = buffer1;
	freq = buffer2;
	return ret;
}

int
SoundAD::GetData(float *sig)
{
	return this->GetData(sig, DEFAULT_PERIOD_NUM); // 4284 samples
}

int
SoundAD::GetData(float *sig, int period, float *freq)
{
	this->GetData(sig, period);
	memcpy(freq, sig, frames*period*sizeof(float));
	fft.exe_fft(freq, NULL);
	return 0;
}

/**
 * 
 * @param  buffer [description]
 * @param  period [description]
 * @return        the number of frames or samples.
 */
int 
SoundAD::GetData(float *buffer, int period)
{
	this->data = (char *)buffer;
	int ret = 0;
	ret = Record(period);

#ifdef CORRECT_AMP
	for (int i = 0; i < 204*21; ++i)
	{
		buffer[i] /= SOUND_AMP_RATE;
	}
#endif
	// buffer = (float *)(this->data);
	return ret;
}

float 
SoundAD::GetValue()
{
	return this->GetValue(DEFAULT_FIELT_FREQ_MIN, DEFAULT_FIELT_FREQ_MAX);
}

float
SoundAD::GetValue(int filt_freq_min, int filt_freq_max)
{
	return this->GetValue(filt_freq_min, filt_freq_max, DEFAULT_WINDOW_MIN, DEFAULT_WINDOW_MAX);
}

/**
 * @param  filt_freq_min the min frequency
 * @param  filt_freq_max the max frequency
 * @return               return the max value of signal in 21 sec, which has been filtered in 20~60hz
 */
float
SoundAD::GetValue(int filt_freq_min, int filt_freq_max, int win_min, int win_max)
{
	this->GetData(buffer1, DEFAULT_PERIOD_NUM);
	// memcpy(freq, sig, frames*period*sizeof(float));
	fft.exe_fft(buffer1, buffer2); // buffer1 is amp, buffer2 is phase

	filt_freq_min = filt_freq_min*(period_time*DEFAULT_PERIOD_NUM) / 1e6; //20Hz
	filt_freq_max = filt_freq_max*(period_time*DEFAULT_PERIOD_NUM) / 1e6;

	for (int i = 0; i <= filt_freq_min; ++i)
	{
		buffer1[i] *=1e-5;
	}
	for (int i = filt_freq_max; i <= DEFAULT_FFT_LEN/2; ++i)
	{
		buffer1[i] *=1e-5;
	}
	for (int i = 0; i <= filt_freq_min; ++i)
	{
		buffer1[DEFAULT_FFT_LEN - i] *=1e-5;
	}
	for (int i = filt_freq_max; i <= DEFAULT_FFT_LEN/2; ++i)
	{
		buffer1[DEFAULT_FFT_LEN - i] *=1e-5;
	}

	fft.exe_ifft(buffer1, buffer2);

	float maxvalue = 0;
	for (int i = win_min; i < win_max; ++i)
	{
		if (buffer1[i]>maxvalue)
			maxvalue = buffer1[i];
		// cout << buffer1[i] << endl;
	}

	return maxvalue;
}

/**
 * Print char *buffer in float format
 * @param  buffer [description]
 * @param  size   [description]
 * @return        [description]
 */
int 
SoundAD::PrintBuffer(char *buffer, size_t size)
{
	if (pcm_format==SND_PCM_FORMAT_FLOAT_LE) {
		if (channels==1)
			for (int i = 0; i < size/4; ++i)
			{
				cout << *(float *)(buffer+i*4)/SOUND_AMP_RATE << endl;
				// if (*(float *)(buffer+i*4)>record_max)
				// 	record_max = *(float *)(buffer+i*4);
			}
		else if (channels==2)
			for (int i = 0; i < size/4/2; ++i)
			{
				cout << *(float *)(buffer+(2*i)*4) << "  " << *(float *)(buffer+(2*i+1)*4)/SOUND_AMP_RATE << endl;
			}
	}
	return size;
}

void 
SoundAD::CloseDevice()
{
	free(buffer1);
	free(buffer2);
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	snd_pcm_hw_params_free(params);
}

int 
SoundAD::DisplayALSAEnabled()
{

	cout << "==== alsa enabled ====" << endl;

    int val;  
  
    printf("ALSA library version: %s\n",  
                       SND_LIB_VERSION_STR);  
  
    printf("\nPCM stream types:\n");  
    for (val = 0; val <= SND_PCM_STREAM_LAST; val++)  
            printf(" %s\n",  
                snd_pcm_stream_name((snd_pcm_stream_t)val));  
  
    printf("\nPCM access types:\n");  
    for (val = 0; val <= SND_PCM_ACCESS_LAST; val++)  
    {  
            printf(" %s\n",  
                snd_pcm_access_name((snd_pcm_access_t)val));  
    }  
  
    printf("\nPCM formats:\n");  
    for (val = 0; val <= SND_PCM_FORMAT_LAST; val++)  
        {  
        if (snd_pcm_format_name((snd_pcm_format_t)val)!= NULL)  
        {  
                printf(" %s (%s)\n",  
                    snd_pcm_format_name((snd_pcm_format_t)val),  
                    snd_pcm_format_description(  
                            (snd_pcm_format_t)val));  
        }  
    }  
    printf("\nPCM subformats:\n");  
    for (val = 0; val <= SND_PCM_SUBFORMAT_LAST;val++)  
        {  
        printf(" %s (%s)\n",  
                snd_pcm_subformat_name((  
                snd_pcm_subformat_t)val),  
                snd_pcm_subformat_description((  
                snd_pcm_subformat_t)val));  
    }  
    printf("\nPCM states:\n");  
    for (val = 0; val <= SND_PCM_STATE_LAST; val++)  
            printf(" %s\n",  
                snd_pcm_state_name((snd_pcm_state_t)val));  
  
    return 0;  

}