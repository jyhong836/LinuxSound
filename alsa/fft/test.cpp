/* 
* @Author: Junyuan Hong
* @Date:   2014-11-25 11:09:48
* @Last Modified by:   Junyuan Hong
* @Last Modified time: 2014-11-25 12:17:22
*/

#include <iostream>
#include <math.h>
#include "FFT.h"

using namespace std;

int main(int argc, char const *argv[])
{
	FFT fft;

	int N = 1<<12;
	float buffer[(1<<12)*2];
	float freq = 50;

	for (int i = 0; i < N; ++i)
	{
		buffer[i] = cos(2*GPU_FFT_PI*freq*i/N);
	}

	fft.exe_fft(buffer);

	fft.exe_ifft(buffer);

	// for (int i = 0; i < N; ++i)
	// {
	// 	cout << buffer[i] << endl;
	// }

	return 0;
}