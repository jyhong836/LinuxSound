#ifndef _FFT_
#define _FFT_

extern "C"
{
	#include "mailbox.h"
	#include "gpu_fft.h"
}

#define	 DEFAULT_FFT_LEN  4096

class FFT
{
private:
    struct GPU_FFT *fft;
    int N;    // FFT length, N = 1<<log2_N;
    int jobs; // default is 1
    int dir;  // GPU_FFT_FWD: fft(); GPU_FFT_REV: ifft()
    int mb;   // mailbox
    struct GPU_FFT_COMPLEX *base;

public:
	FFT();
	~FFT();
	int Init(int);
	int exe_fft(float *, float *);
	int exe_ifft(float *, float *);
	static unsigned GetSystemMicroseconds(void);
	// int getFFTLength();
	
};


#endif // ~_FFT_
