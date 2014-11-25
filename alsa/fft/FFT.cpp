/* 
* @Author: Junyuan Hong
* @Date:   2014-11-25 10:30:35
* @Last Modified by:   Junyuan Hong
* @Last Modified time: 2014-11-25 19:42:01
*/

#include <iostream>
#include <stdio.h>

#include "FFT.h"
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

// #define VERB 

using namespace std;

FFT::FFT()
{
	mb = mbox_open();
    jobs   = 1;//argc>2? atoi(argv[2]) : 10; // transforms per batch
	Init(GPU_FFT_FWD);
}

FFT::~FFT()
{
	gpu_fft_release(fft); // Videocore memory lost if not freed !
}

int
FFT::Init(int direction)
{
#ifdef VERB
    cout << "Init with dir: " << direction << endl;
#endif

	dir = direction;
	// FILE *fp;
	// fp = fopen("out.txt", "w");

    // int i, j, k;
    int log2_N, ret;
    // unsigned t[2];
    // double tsq[2];

    // struct GPU_FFT_COMPLEX *base, base1[4096];

    log2_N = 12;//argc>1? atoi(argv[1]) : 12; // 8 <= log2_N <= 17
    // loops  =  1;//rgc>3? atoi(argv[3]) : 1;  // test repetitions

    N = 1<<log2_N; // FFT length
    ret = gpu_fft_prepare(mb, log2_N, GPU_FFT_FWD, jobs, &fft); // call once

    switch(ret) {
        case -1: printf("Unable to enable V3D. Please check your firmware is up to date.\n"); return -1;
        case -2: printf("log2_N=%d not supported.  Try between 8 and 17.\n", log2_N);         return -1;
        case -3: printf("Out of memory.  Try a smaller batch or increase GPU memory.\n");     return -1;
    }

#ifdef VERB
    cout << "Init with dir: " << direction << "...OK" << endl;
#endif
}

/**
 * do fft for the data in buffer
 * @param  buffer the re and im part of fft input will be set the same as buffer, which should be as long as N
 * @return status
 */
int
FFT::exe_fft(float *buffer, float *phase)
{
	int i, j;
#ifdef VERB
    unsigned t[2];
#endif

	if (dir!=GPU_FFT_FWD)
		Init(GPU_FFT_FWD);

	for (j=0; j<jobs; j++) {
        base = fft->in + j*fft->step; // input buffer
        for (i=0; i<N; i++) 
        	base[i].re = base[i].im = buffer[i];
    }

    usleep(1); // Yield to OS
#ifdef VERB
    t[0] = GetSystemMicroseconds();
#endif

    gpu_fft_execute(fft); // call one or many times

#ifdef VERB
    t[1] = GetSystemMicroseconds();

    cout << "fft used time: " << (t[1] - t[0]) << " us" << endl;
#endif

	for (j=0; j<jobs; j++) {
        base = fft->out + j*fft->step; // output buffer
        if (phase!=NULL)
	        for (i=0; i<N; i++) {
	        	buffer[i] = hypotf(base[i].im, base[i].re)/N;//base[i].re;//
	        	phase[i] = atan2f(base[i].im, base[i].re);
	        }
	    else
	        for (i=0; i<N; i++) {
	        	buffer[i] = hypotf(base[i].im, base[i].re)/N;//base[i].re;//
	        }
    }

	return 0;
}

int
FFT::exe_ifft(float *buffer, float *phase)
{
	int i, j;
    unsigned t[2];

	if (dir!=GPU_FFT_REV)
		Init(GPU_FFT_REV);

	for (j=0; j<jobs; j++) {
        base = fft->in + j*fft->step; // input buffer
        if (phase!=NULL)
	        for (i=0; i<N; i++) {
	        	base[i].re = buffer[i] * cosf(phase[i]);
	        	base[i].im = buffer[i] * sinf(phase[i]);
	        }
	    else
	        for (i=0; i<N; i++) {
	        	base[i].re = base[i].im = buffer[i];
	        }
    }

    usleep(1); // Yield to OS
#ifdef VERB
    t[0] = GetSystemMicroseconds();
#endif
    gpu_fft_execute(fft); // call one or many times

#ifdef VERB
    t[1] = GetSystemMicroseconds();

    cout << "ifft used time: " << (t[1] - t[0]) << " us" << endl;
#endif

	for (j=0; j<jobs; j++) {
        base = fft->out + j*fft->step; // output buffer
        for (i=0; i<N; i++) 
        	buffer[i] = base[i].re;
        	// phase[i] = base[i].im; // im is not changed
    }

	return 0;
}

/**
 * get OS time in microsecond
 * @return  time in msec
 */
unsigned 
FFT::GetSystemMicroseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec*1000000 + ts.tv_nsec/1000;
}
