LinuxSound
==========

This respository is aimed to read and write data to sound card. There are two way to make this. The first one is DSP, which is used by me at my Ubuntu 10.04 Virtualbox. Another one is ALSA, which is a newer Sound Card library.

## DSP

Porocess the audio in Linux System. The DSP device in Linux has been used to do this.
To compile it, you can run ```make``` in the program folder.
Before you run it, please check if the device `/dev/dsp` exist.

#### Features

* create wav sound file according coded math rules
* play the wav file, and show the messages.

#### Reference

* about dsp program please refer to [Linux音频编程指南](http://www.ibm.com/developerworks/cn/linux/l-audio/)

## ALSA

Process the audio in Linux System, where the libasound is installed. In Debian, you can install the neccessary lib with `apt-get install libasound2-dev`, and you may also want the doc, `apt-get install libasound2-doc`, which is in html format. I have applied this part code in my `Raspberry Pi` with a USB card.

#### Features

+ read float data from the audio capture.

#### Reference

+ [【Linux&音频】Alsa音频编程【精华】](http://blog.csdn.net/tianshuai1111/article/details/8191711)