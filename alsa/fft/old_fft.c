/* 
* @Author: Junyuan Hong
* @Date:   2014-11-05 19:19:38
* @Last Modified by:   Junyuan Hong
* @Last Modified time: 2014-11-05 21:27:59
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "mailbox.h"
#include "gpu_fft.h"

/**
 * get OS time in microsecond
 * @return  time in msec
 */
unsigned Microseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec*1000000 + ts.tv_nsec/1000;
}

int main(int argc, char *argv[]) {
	FILE *fp;
	fp = fopen("out.txt", "w");

    int i, j, k, ret, loops, freq, log2_N, jobs, N, mb = mbox_open();
    unsigned t[2];
    double tsq[2];

    struct GPU_FFT_COMPLEX *base, base1[4096];
    struct GPU_FFT *fft;

    log2_N = 12;//argc>1? atoi(argv[1]) : 12; // 8 <= log2_N <= 17
    jobs   = 1;//argc>2? atoi(argv[2]) : 10; // transforms per batch
    loops  =  1;//rgc>3? atoi(argv[3]) : 1;  // test repetitions

    N = 1<<log2_N; // FFT length
    ret = gpu_fft_prepare(mb, log2_N, GPU_FFT_FWD, jobs, &fft); // call once

    switch(ret) {
        case -1: printf("Unable to enable V3D. Please check your firmware is up to date.\n"); return -1;
        case -2: printf("log2_N=%d not supported.  Try between 8 and 17.\n", log2_N);         return -1;
        case -3: printf("Out of memory.  Try a smaller batch or increase GPU memory.\n");     return -1;
    }

    freq = 50;

    for (k=0; k<loops; k++) {

        for (j=0; j<jobs; j++) {
            base = fft->in + j*fft->step; // input buffer
            for (i=0; i<N/2; i++) base[i].re = base[i].im = (cos(2*GPU_FFT_PI*freq*i/N)>0)? cos(2*GPU_FFT_PI*freq*i/N):0;//0;
            // freq = j+1;
            // base[freq].re = base[N-freq].re = 0.5;
            // fprintf(fp, "%0.2g\n", );
        }

        usleep(1); // Yield to OS
        t[0] = Microseconds();
        gpu_fft_execute(fft); // call one or many times
        t[1] = Microseconds();

        tsq[0]=tsq[1]=0;
        for (j=0; j<jobs; j++) {
            base = fft->out + j*fft->step; // output buffer
            // base1 = fft->in + j*fft->step;
            // freq = j+1;
            for (i=0; i<N; i++) {
                // double re = cos(2*GPU_FFT_PI*freq*i/N);
                // tsq[0] += pow(re, 2);
                // tsq[1] += pow(re - base[i].re, 2) + pow(base[i].im, 2);
                // fprintf(fp, "%0.2g\t%0.2g\n", sqrt(pow(base[i].re,2) + pow(base[i].im,2)), base[i].im);
                base1[i].re = base[i].re;
            }
        }

    	ret = gpu_fft_prepare(mb, log2_N, GPU_FFT_REV, jobs, &fft); // call once

	    switch(ret) {
	        case -1: printf("Unable to enable V3D. Please check your firmware is up to date.\n"); return -1;
	        case -2: printf("log2_N=%d not supported.  Try between 8 and 17.\n", log2_N);         return -1;
	        case -3: printf("Out of memory.  Try a smaller batch or increase GPU memory.\n");     return -1;
	    }
	    base = fft->in;
        for (i = 0; i < N; ++i)
        {
        	base[i].re = base1[i].re/2048;
        	// fprintf(fp, "%0.2g\n", base[i].re);
        }
        gpu_fft_execute(fft); // call one or many times
        base = fft->out; // output buffer

        for (i=0; i<N; i++) {
        	fprintf(fp, "%0.2g\n", base[i].re);
        }

        // printf("rel_rms_err = %0.2g, usecs = %d, k = %d\n",
        //     sqrt(tsq[1]/tsq[0]), (t[1]-t[0])/jobs, k);
    }

    gpu_fft_release(fft); // Videocore memory lost if not freed !
    fclose(fp);
    return 0;
}