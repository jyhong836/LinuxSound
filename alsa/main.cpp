/* 
* @Author: Junyuan Hong
* @Date:   2014-11-24 16:55:23
* @Last Modified by:   Junyuan Hong
* @Last Modified time: 2014-11-25 20:46:39
*/

#include <iostream>
#include <string.h>
#include <math.h>

#include "SoundAD.h"
#include "fft/FFT.h"

using namespace std;

/**
 * copy the data from the first size to last size inverved.
 * @param buf  [description]
 * @param size [description]
 */
void revCopy(float *buf, size_t size)
{
	float *ptr = buf+size;
	for (int i = 0; i < size; ++i)
	{
		ptr[size - i-1] = buf[i];
	}
}

/**
 * Usage:
 * 		./SoundADMain m
 * 			display the messages
 * 		./SoundADMain <period num>
 * 			record numbers of period data(1 period is 21333 us)
 */
int main(int argc, char const *argv[])
{
	SoundAD sad;
	if (argc > 1)
		if (argv[1][0]=='m')
		{
			sad.DisplayALSAEnabled();
			sad.DisplayConfigure();
		}
		else if (argv[1][0]=='f') {
			// FFT fft;
		#ifdef VERB
			unsigned t1 = FFT::GetSystemMicroseconds();
		#endif
			size_t buffer_sz = 204*21*sizeof(float);
			float *buffer = (float *) malloc(buffer_sz);
			float *phase  = (float *) malloc(buffer_sz);
			float *buffer2  = (float *) malloc(buffer_sz);
			if (!buffer || !phase)
			{
				cerr << "malloc failed" << endl;
			}
			int N = sad.GetData(buffer, phase);
			// memcpy(buffer2, buffer, buffer_sz);
			// fft.exe_fft(buffer, phase);
			// fft.exe_ifft(buffer, phase);

			float rms[2] = {0};
			float maxbuf[2];
			for (int i = 0; i < 4096; ++i)
			{
				// cout << buffer[i] << "\t" << phase[i] << endl;
				
				rms[0] += pow(buffer2[i], 2);
				rms[1] += pow(buffer2[i] - buffer[i], 2);
				if (buffer[i]>maxbuf[0])
					maxbuf[0] = buffer[i];
				if (buffer2[i]>maxbuf[1])
					maxbuf[1] = buffer2[i];
			}

			free(buffer);
			free(buffer2);
			free(phase);
		#ifdef VERB
			cout << "totoal time: " << (FFT::GetSystemMicroseconds() - t1)/1000 << " ms" << endl;
			cout << "rms error = " << sqrt(rms[1]/rms[0]) << endl;
			cout << "max diff  = " << (maxbuf[1] - maxbuf[0])/maxbuf[1] << endl;
		#endif
		} else  if (argv[1][0]=='t')
		{
		#ifdef VERB
			unsigned t1 = FFT::GetSystemMicroseconds();
		#endif
			size_t buffer_sz = 204*21*sizeof(float);
			float *buffer = (float *) malloc(buffer_sz);
			float *freq  = (float *) malloc(buffer_sz);
			if (!buffer || !freq)
			{
				cerr << "malloc failed" << endl;
			}
			int N = sad.GetData(buffer, freq);
			for (int i = 0; i < 4096; ++i)
				cout << buffer[i] << "\t" << freq[i] << endl;
		#ifdef VERB
			cout << "totoal time: " << (FFT::GetSystemMicroseconds() - t1)/1000 << " ms" << endl;
		#endif
		} else
			sad.Record(atoi(argv[1]));
	else
		sad.Record(1);
    
    return 0;
}