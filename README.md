LinuxSound
==========

This respository is aimed to read and write data to sound card. There are two way to make this. The first one is DSP, which is used by me at my Ubuntu 10.04 Virtualbox. Another one is ALSA, which is a newer Sound Card library.

## DSP

Porocess the audio in Linux System. The DSP device in Linux has been used to do this.
To compile it, you can run `make` in the code folder (`alsa` and `dsp`) to compile them.
Before you run it, please check if the device `/dev/dsp` exist.

#### Features

* create wav sound file according coded math rules
* play the wav file, and show the messages.

#### Reference

* about dsp program please refer to [Linux音频编程指南](http://www.ibm.com/developerworks/cn/linux/l-audio/)

## ALSA

Process the audio in Linux System, where the libasound is installed. In Debian, you can install the neccessary lib with `apt-get install libasound2-dev`, and you may also want the doc, `apt-get install libasound2-doc`, which is in html format.

I have applied this part code in my `Raspberry Pi` with a USB card. I have use the GPU of raspberry pi to speed up the fft proccess. The fft code in `alsa/fft/` is created by [Andrew Holme](http://www.raspberrypi.org/tag/andrew-holme/), which is included in raspbian system `/opt/vc/src/hello_pi/hello_fft`.

#### Features

+ read float data from the audio capture.
+ use Andrew Holme's code to speed up my fft proccess with GPU.

### About FFT in Raspberry Pi

You can read my introduction to FFT with GPU in Rasp-Pi by visiting my [blog](http://jyhong.blog.ustc.edu.cn) page, [raspberry pi use GPU to do FFT](https://jyhong.blog.ustc.edu.cn/2014/11/raspberry-pi-use-gpu-to-do-fft/) in Chinese, or [Eben Upton](http://www.raspberrypi.org/author/eben/)'s post [Accelerating Fourier transforms using the GPU](http://www.raspberrypi.org/accelerating-fourier-transforms-using-the-gpu/ "Accelerating Fourier transforms using the GPU") in English.

You can run the program in `alsa/` with command `./SoundADMain` in bash, the help message will be printed.

#### Reference

+ [【Linux&音频】Alsa音频编程【精华】](http://blog.csdn.net/tianshuai1111/article/details/8191711)
+ [Accelerating Fourier transforms using the GPU](http://www.raspberrypi.org/accelerating-fourier-transforms-using-the-gpu/ "Accelerating Fourier transforms using the GPU")