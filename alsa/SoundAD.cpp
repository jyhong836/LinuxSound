/* 
* @Author: Junyuan Hong
* @Date:   2014-11-24 16:34:04
* @Last Modified by:   Junyuan Hong
* @Last Modified time: 2014-11-24 21:02:26
*/

#include <iostream>

#include "SoundAD.h"

// #define _VERB_

using namespace std;

SoundAD::SoundAD():
handle(0),
// sampleRate(44100),
sampleRate(9600),
frames(32),
channels(1)
{
	pcm_format = SND_PCM_FORMAT_FLOAT_LE;
	OpenDevice();
	Configure();
}

SoundAD::~SoundAD()
{
	CloseDevice();
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

#ifdef _VERB_
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

#ifdef _VERB_
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

	snd_pcm_hw_params_get_tick_time(params,
	                                  &val, &dir);
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
#ifdef _VERB_
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

	/* Set period size to 32 frames. */
	// frames = 32;
	snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
	    fprintf(stderr,
	            "unable to set hw parameters: %s\n",
	            snd_strerror(rc));
	    exit(1);
	}

#ifdef _VERB_
	cout << "Configure device...success" << endl;
#endif

	return 0;

}

int
SoundAD::Record(int period) {

	unsigned int val;
	int dir;
	char *buffer;
	size_t size;
	long loops;

	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	size = frames * 4; /* 2 bytes/sample, 2 channels */
	buffer = (char *) malloc(size);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params, &val, &dir); // period time: 21333 us
	// loops = msec*1000 / val;
	loops = period;
	// record_max = 0;

	while (loops > 0) {

	    loops--;
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
	    rc = PrintBuffer(buffer, size);
	    if (rc != size) 
	    	fprintf(stderr, "short write: wrote %d bytes\n", rc);
	}

	free(buffer);

	// cout << "max: "<<record_max <<endl;

	return loops*val/1000;

}

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