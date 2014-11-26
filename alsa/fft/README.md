Speed up FFT using GPU in Raspberry Pi
====

These code works well in raspbian. The code proccess GPU is created by [Andrew Holme](http://www.raspberrypi.org/tag/andrew-holme/), which is included in raspbian system `/opt/vc/src/hello_pi/hello_fft`.

### Usage

To do FFT, you can use the code below.

	#include "FFT.h"
	FFT fft; // init a FFT object
	fft.exe_fft(float *signal2freq, float *phase); // FFT
	fft.exe_ifft(float *freq2signal, float *phase); // IFFT

You should malloc a memory of `DEFAULT_FFT_LEN*sizeof(float)` size at pointer `signal2freq` and `phase`. The signal to be FFT should be copy to the `signal2freq` firstly. The frequncy domain data will return at the same pointer. The phase will return in `phase` pointer. You needn't to set any value in `phase`, it's just useful for value returning.

Similarly, IFFT will pass the signal data from the `freq2signal` pointer. The `phase` will not be changed, since it's just useful for pass the value of phase data to `exe_ifft`.

The frequncy and index in frequncy domain data has the relation like this:
	
	frequency = index / sample_time

The `sample_time` is the time length of your signal in second.

### Refference

+ visit my blog about FFT and rasp GPU in Chinese: [raspberry pi use gpu to do fft](https://jyhong.blog.ustc.edu.cn/2014/11/raspberry-pi-use-gpu-to-do-fft/).
+ [Accelerating Fourier transforms using the GPU](http://www.raspberrypi.org/accelerating-fourier-transforms-using-the-gpu/ "Accelerating Fourier transforms using the GPU")