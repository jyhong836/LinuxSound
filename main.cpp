#include <iostream>
#include <stdio.h>
using namespace std;
#include <string.h>

#include "sound.h"
#include "keyboardctl.h"

#define __PLAY
//unsigned char buf[LENGTH*RATE*CHANNELS  *SIZE/8]={100};

int sound(int handler, mSound ms);

int main(int argc, char * argv[])
/**
	parameter descripter
	- : playSora and save sora wave to "sora.wav"
	r <wav filename> : read the 16bit 1ch wave of the wav file to buf.dat 
	<wav filename> : play the wav file
*/
{
	mSound ms;
#ifdef __PLAY
	if (argc>1)
		if(argv[1][0]=='-')
			return ms.playSora();
		else
		{
			if(argc<3)
			{
				if (strlen(argv[1])>1)
				{
					cout<< "play "<<argv[1]<<endl;
					unsigned char *wave = 0;
					// int len = ms.readSong(argv[1], wave);
					// if (len<=0)return -1;
					// if (ms.tInitDSP(0,0,0)==-1)return -1;
					return ms.playSong(argv[1]); //(0, wave, len);
				}
			}
			else
			{
				cout<<"here"<<argc<<endl;
				if (argv[1][0]=='r' && strlen(argv[2])>1)
				{
					cout<< "read "<<argv[2]<<endl;
					unsigned char *wave = 0;
					int len = ms.readSong(argv[2], wave);
					if(len==-1)return -1;
					FILE *datf = fopen("buf.dat","w");
					fprintf(datf, "time\tch 1\tch 2\n");
					for(int i = 0;i<len;i++)
					{
						fprintf(datf, "%d\t", i/4);
						// fprintf(datf,"%d %d\t\t%d %d\n",(int)(char)wave[i],(int)(char)wave[++i],(int)(char)wave[++i],(int)(char)wave[++i]);
						short data = *((short *)&wave[i]);
						i += 2;
						//data += (unsigned short)wave[++i]<<8;
						fprintf(datf, "%d\t", data);
						data = *((short *)&wave[i]);
						i++;
						//data += (unsigned short)wave[++i]<<8;
						fprintf(datf, "%d\n", data);
					}
					fclose(datf);
					if (len<=0)return -1;
					if (ms.tInitDSP(0,0,0)==-1)return -1;
					return ms.playSong(0, wave, len);
				}
			}
		}
	else {
		cout<< "play "<<"sora.wav"<<endl;
		unsigned char *wave = 0;
		int len = ms.readSong("sora.wav", wave);
		if(len==-1)
			return -1;
		if (ms.tInitDSP(0,0,0)==-1)return -1;
		if (len<=0)return -1;
		return ms.playSong(0, wave, len);
	}
#else
	ms.readWAVFile();
	// int h = 0;
	// ms.initDSP(h);
	// // for(int i=1;i<100;i++)
	// // 	ms.playFreq(h,1000,1000,100);
	// sound(h, ms);
	// return 0;
#endif
}

int sound(int handler, mSound ms)
{
	int freq = 1000;
	int key;

	while(true)
	{
		ms.playFreq(handler, freq, 1000, 100);
		key = getch();
		switch(key)
		{
			case '=':
				cout<<"freq up"<<endl;
				if (freq < 20000)
					freq += 10;
				break;
			case '-':
				cout<<"freq down"<<endl;
				if (freq > 11)
					freq -= 10;
				break;
			case ' ':
				//stop_buzzer();
				break;
			case '\n':
				break;
			case ESC_KEY:
			case EOF:
				//close_buzzer();
				cout<<"exit"<<endl;
				return 0;
			default:break;
		}
	}
	return 0;
}
/*
int sound()
{
	int fd;
	int arg;
	int status;
	FILE *bf = fopen("buff","wb");
	fd = open("/dev/dsp",O_WRONLY);
	if (fd<0)
	{
		perror("open of /dev/dsp failed");
		return -1;
	}

	arg  = SIZE;
	status = ioctl(fd, SOUND_PCM_WRITE_BITS, &arg);
	if (status==-1)
		perror("SOUND_PCM_WRITE_BITS ioctl failed");
	if (arg != SIZE)
		perror("unable to set sample size");
	arg = CHANNELS;
	status = ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &arg);

	arg = RATE;
	status = ioctl(fd, SOUND_PCM_WRITE_RATE, &arg);
	if (status==-1)
		perror("SOUND_PCM_WRITE_RATE ioctl failed");
	if (arg!=RATE)
		perror("unable to set sample rate");

	int len = sizeof(buf);
	int part = len/4;
	cout<<"len of buf:"<<len<<endl;
	for (int i=0;i<4;i++)
	{
		for(int j=i*part;j<(i+1)*part;j++)
			// //if(i%part)
			// if (!(j%10))
			// 	buf[j] = 100;
		if((j/10)%2)buf[j] = 100;
	}
	fwrite(buf,1,sizeof(buf),bf);

	// while(true)
	// {
	// 	// cout<<"Say something"<<endl;
	// 	// status = read(fd,buf,sizeof(buf));
	// 	cout<<"you said:"<<endl;
	// 	status = write(fd,buf,sizeof(buf));
	// 	status = ioctl(fd,SOUND_PCM_SYNC,0);
	// 	fwrite(buf,1,sizeof(buf),bf);
	// 	// for (int i=0;i<len;i++)
	// 	// {
	// 	// 	fprintf(bf,"%x ",buf[i]);
	// 	// }
	// }
	for(int j=0;j<3;j++)
		for(int i=1;i<=7;i++)
		{
			playFreq (fd, D[j][i], 2000, 100);
		}


	return 0;
}
*/
