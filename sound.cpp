#include "sound.h"
#include "sounddata.h"

#include <malloc.h>
#include <iostream>
using namespace std;
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/soundcard.h>
#include <math.h> // gcc -lm


mSound::mSound():
tHandler(0),
buf16Size(512)
{
	// ----------init wavfHandler--------------
	int waveSize = 0;
	struct RIFF_HEADER  riffHeader = 
	{
		{'R','I','F','F'},
		0,
		{'W','A','V','E'}
	};
	riffHeader.dwRiffSize = waveSize + sizeof(WAVFileHeader) - 8;
	struct FMT_BLOCK  	fmtBlock = 
	{
		{'f','m','t',' '},
		16,
		{
			0x0001,
			CHANNELS,
			RATE,
			0, // 
			SIZE/8, // BytePerSample
			SIZE
		}
	};
	fmtBlock.wavFormat.dwAvgBytesPerSec = fmtBlock.wavFormat.dwSamplesPerSec * (SIZE/8);
	// struct FACT_BLOCK  	factBlock = 
	// {
	// 	{'f','a','c','t'},
	// 	4,
	// 	{'H','O','N','G'}
	// };
	// struct DATA_BLOCK	dataHeader = 
	// {
	// 	{'d','a','t','a'},
	// 	waveSize
	// };
	wavfHandler.riffHeader = riffHeader;
	wavfHandler.fmtBlock = fmtBlock;
	// wavfHandler.factBlock = factBlock;
	// wavfHandler.dataHeader = dataHeader;
	// ----------init wavfHandler end----------

	buf = (unsigned char *)malloc(512);
	buf16 = (short *)malloc(buf16Size);

	return;
}
mSound::~mSound()
{
	free(buf);
	close(tHandler);
	return;
}
int mSound::tInitDSP(int size, int channels, int rate)
{
	if (size==0 && channels == 0)
		initDSP(tHandler, wavfHandler.fmtBlock.wavFormat.wBitsPerSample, 
			wavfHandler.fmtBlock.wavFormat.wChannels, 
			wavfHandler.fmtBlock.wavFormat.dwSamplesPerSec);
	else
		initDSP(tHandler, size, channels, rate);
	return 0;
}
int mSound::initDSP(int &handler, int size, int channels, int rate)
{
	cout<<"init dsp"<<endl<<
	" size = "<<size<<endl<<
	" channels = "<<channels<<endl<<
	" rate = "<<rate<<endl<<
	endl;
	int arg;
	int status;
	handler = open("/dev/dsp",O_WRONLY);
	if (handler<0)
	{
		perror("open of /dev/dsp failed");
		return -1;
	}

	arg    = size;
	status = ioctl(handler, SOUND_PCM_WRITE_BITS, &arg);
	if(status==-1)
		perror("SOUND_PCM_WRITE_BITS ioctl failed");
	if (arg != size)
		perror("unable to set sample size");

	arg = channels;
	status = ioctl(handler, SOUND_PCM_WRITE_CHANNELS, &arg);
	if (status==-1)
		perror("SOUND_PCM_WRITE_CHANNELS ioctl failed");
	if (arg!=channels)
		perror("unable to set sample channel");

	arg = rate;
	status = ioctl(handler, SOUND_PCM_WRITE_RATE, &arg);
	if (status==-1)
		perror("SOUND_PCM_WRITE_RATE ioctl failed");
	if (arg!=rate)
		perror("unable to set sample rate");
	return 0;
}

int mSound::playSora()
{
	int handler(-1);
	if (sizeof(musicSora) != sizeof(pauseSora))return -1;
	cout<<"MUSIC SORA LENTH : " << sizeof(musicSora)/sizeof(int) << " tones" <<endl;
	// for(int i=0;i<10;i++)
	// {
	// 	//keybd_event(0xAF,0,0,0);//0xaf sound up;0xae sound down;0xad sound off\on
	// 	playTunes(handler, musicSora,pauseSora, 8, sizeof(musicSora)/sizeof(int),100);
	// 	sleep(4); // sleep 4 sec ==> usleep(4000000);
	// }
	const int clap = 8; // ==> a joint include 8 half clap
	#ifndef __TONE16__
		initDSP(handler);
		unsigned char * wave;
		wave = 0;
		int len = createSong(wave, musicSora, pauseSora, clap, sizeof(musicSora)/sizeof(int));
	#else  // tone int 16bit sample , 1 channel
		initDSP(handler, 16, CHANNELS, RATE);
		short *wave = 0;
		int size = createSong16(wave, musicSora, pauseSora, clap, sizeof(musicSora)/sizeof(int));
	#endif
	if (!wave)
	{
		cout<<"wave is NULL in playSora"<<endl;
	}

	#ifndef __TONE16__
		saveSong("sora.wav", wave, len, CHANNELS, RATE, SIZE);

		playSong(handler, wave, len);
	#else
		#ifdef __DEBUGE_PLAYSORA
			printf("%x\t%x\t%x\t%x\n",*((unsigned char *)&wave[size/sizeof(short)-4]),
				*((unsigned char *)&wave[size/sizeof(short)-4]+1),
				*((unsigned char *)&wave[size/sizeof(short)-3]),
				*((unsigned char *)&wave[size/sizeof(short)-3]+1));
		#endif // __DEBUGE_PLAYSORA
		saveSong("sora.wav", (unsigned char *)wave, size, CHANNELS, RATE, 16);

		playSong(handler, (unsigned char *)wave, size);
	#endif

    cout << "Fin!" << endl;  
    free(wave);     // free the memmory
    close(handler); // close dsp handler
    return 0;  
}

//#define __MCOSWAVE__
#define __COSWAVE__  // use cos shaped wave
//#define __TRIWAVE__

int mSound::playFreq(int handler, int freq, int duration, unsigned char vol)//duration:last time /ms
{
	int n = duration*RATE*CHANNELS  *SIZE*sizeof(unsigned char)/8 /1000;
	if(sizeof(buf) < n)buf = (unsigned char *) realloc(buf, n);
	int gap = (int)(RATE / freq / 2);
	#ifndef NDEBUGE
		cout<<"freq = "<<freq<<" gap = "<<gap<<endl;
	#endif
	if (!freq || !gap || !buf)
	{
		return -1;
	}


	int x(0);
	int a = gap/2;

	for (int i=0;i<n;i++)
	{
	#ifdef __MCOSWAVE__
			x = i%gap - a;
			if((i/gap)%2)
				buf[i] = vol * (2 - (3.14159 * x) * (3.14159 * x)/8/a/a);
			else
				buf[i] = vol * (3.14159 * x) * (3.14159 * x)/8/a/a;
	#else
	#ifdef __COSWAVE__
			x = i%gap - a;
			if((i/gap)%2)
				buf[i] = vol * (1 + sin((3.14159 * x)/2/a));
			else
				buf[i] = vol * (1 - sin((3.14159 * x)/2/a));
	#else
	#ifdef __TRIWAVE__
			x = i%gap - a;
			if((i/gap)%2)
				buf[i] = vol * ((1.0*x/a<0)?(0-1.*x/a):(1.0*x/a));
			else
				buf[i] = vol * (2 - (1.0*x/a<0)?(0-1.*x/a):(1.0*x/a));
	#else
			if((i/gap)%2)
			{
				buf[i] = vol;
			}
			else buf[i] = 0;
	#endif
	#endif
	#endif
	}
	int status = write(handler, buf, n);
	//status = ioctl(handler, SOUND_PCM_SYNC,0); // wait play end
	return 0;
}

int mSound::playTunes(int handler, int freq[], int pau[], int clap, int n, unsigned char vol)
{
	int i;
	int du = 1500; // a joint in Millisec
	for(i=0;i<n;i++)
	{
		cout<<"i "<<i<<endl;
		if(!freq[i])
			usleep(du*pau[i]/clap*1000); // Sleep >ms(Windows) ; sleep > s(linux)  ; usleep >us
		else 
			playFreq(handler,freq[i],du*pau[i]/clap, vol);
	}
	return 0;
}

int mSound::playSong(const char *filename)
// the wave may be created by createSong function, retrun status
{
	BYTE *wave = 0;
	int size;
	int status = readWAVFile(wavfHandler, filename, wave, size);
	if(status == -1)
	{
		cerr<<"[1] error in playSong"<<endl;
		return -1;
	}
	if (tInitDSP(0,0,0)==-1)return -1;
	playSong(0, wave, size);
	// while(status!=0)
	// 	//  not eof       no error
	// {
	// 	if(status == -1)
	// 	{
	// 		cerr<<"traceBack:error in playSong"<<endl;
	// 		return -1;
	// 	}
	// 	#ifdef __DEBUGE_PLAYSONG
	// 		cout<<1<<endl;
	// 	#endif
	// 	status = exReadWAVFile((FILE *)status, wave, size);
	// 	playSong(0, wave, size);
	// }
	return 0;
}

int mSound::playSong(int handler, unsigned char wave[], int len)
/** the wave may be created by createSong function, retrun status
	if handler ==0 
	use tHandler
*/
{
	#ifndef NDEBUGE
		FILE *fp = fopen("buff","wb");
	#endif
	if (handler == 0)
		if (tHandler)
			handler = tHandler;
		else {
			perror("handler is NULL");
			return -1;
		}
	int status = write(handler, wave, len);
	if (status == -1)
	{
		 // cout<<"wave addr"<<(int)wave<<endl;
		perror("some error occured in playSong");
		return -1;
	}
	#ifndef NDEBUGE
		fwrite(wave,1,len,fp);
	#endif
	status = ioctl(handler, SOUND_PCM_SYNC,0); // wait play end
	if (status == -1)
	{
		perror("some error occured in playSong");
		return -1;
	}
	return 0;
}

int mSound::createTone(int freq, int duration, unsigned char vol)
/** similar to playFreq function ,but not play
	return by the buf
	return the size of buf
*/
{
	int n = duration*RATE*CHANNELS  *SIZE/8 /1000;
	// cout<<"**"<<n<<endl;
	if (sizeof(buf) < n)
		buf = (unsigned char *) realloc(buf, n);
	int gap = (int)(RATE / freq / 2);
	#ifndef NDEBUGE
		cout<<"freq = "<<freq<<" gap = "<<gap<<endl;
	#endif
	if (!freq || !gap || !buf)
	{
		perror("freq | gap | buf is zero");
		return -1;
	}


	int x(0);
	int a = gap/2;

	for (int i=0;i<n;i++)
	{
	#ifdef __MCOSWAVE__
			x = i%gap - a;
			if((i/gap)%2)
				buf[i] = vol * (2 - (3.14159 * x) * (3.14159 * x)/8/a/a);
			else
				buf[i] = vol * (3.14159 * x) * (3.14159 * x)/8/a/a;
	#else
	#ifdef __COSWAVE__
			x = i%gap - a;
			if((i/gap)%2)
				buf[i] = vol * (1 + sin((3.14159 * x)/2/a));
			else
				buf[i] = vol * (1 - sin((3.14159 * x)/2/a));
	#else
	#ifdef __TRIWAVE__
			x = i%gap - a;
			if((i/gap)%2)
				buf[i] = vol * ((1.0*x/a<0)?(0-1.*x/a):(1.0*x/a));
			else
				buf[i] = vol * (2 - (1.0*x/a<0)?(0-1.*x/a):(1.0*x/a));
	#else
			if((i/gap)%2)
			{
				buf[i] = vol;
			}
			else buf[i] = 0;
	#endif
	#endif
	#endif
	}
	return n;
}

int mSound::createSong(unsigned char *&wave, int freq[], int pau[], int clap, int n, unsigned char vol,
	unsigned char voice)
// return  the length of the song = (real) sizeof(wave) which maybe changed
// clap is the min of clap
{
	int i, j;
	int du = 1500; // a jiont length

	// init wave
	int len = 0,l;
	for (i=0;i<n;i++)
		len += pau[i]*du*RATE/clap/1000;
	if (!wave || sizeof(wave)!=len)
		wave = (unsigned char *)realloc(wave, len);
	if (!wave)
		cerr<<"realloc wave failed"<<endl;
	#ifndef NDEBUGE
		cout<<"n = "<<n<<" len = "<<len<<endl;
	#endif // NDEBUGE
	cout<<"realloc wave size : "<<len/1024.0<<" kByte"<<endl;

	len = 0; // save the addr of wave index
	for(i=0;i<n;i++)
	{
	#ifndef NDEBUGE
		cout<<"i "<<i<<endl;
	#endif // NDEBUGE
		if(!freq[i])
		{
			l = pau[i]*du*RATE/clap/1000;
			for (j=0;j<l;j++)
			{
				wave[len + j] = voice;
			}
			len += l;//pau[i]*du*RATE/clap/1000;
		}
		else 
		{
			if((l = createTone(freq[i], du*pau[i]/clap, vol))>0)
			{
				smoothTone(l, vol, voice);
				for (j=0;j<l;j++)
				{
					wave[len+j] = buf[j];
				}
				len += l;//pau[i]*du*RATE/clap/1000;
			}
			else
			{
				cerr<<"some occured in createSong"<<endl;
				return -1;
			}
		}
	}
	// smoothSong(len,wave);
	cout<<"create song success : lenth = "<<len/1000.0<<"k samples"<<endl;
	return len;
}

int mSound::createTone16(int freq, int duration, short vol)
/** similar to playFreq function ,but not play,duration :last time in ms
	return by the buf16
	return the size of buf16
*/
{
	const int BitsPerSample = 16;//sizeof(short);
	int size = duration*RATE*CHANNELS  *BitsPerSample / 8 / 1000;
	if(buf16Size < size){ // new size is larger than the old
		buf16 = (short *) realloc(buf16, size);
		buf16Size = size;
	}
	if (!buf16)
		cerr<<"alloc buf16 failed in createTone16"<<endl;
	int gap = (int)(RATE / freq / 2);
	#ifndef NDEBUGE
		cout<<"freq = "<<freq<<" gap = "<<gap<<endl;
	#endif
	if (!freq || !gap || !buf16)
	{
		perror("freq | gap | buf16 is zero");
		return -1;
	}

	int x(0);
	int a = gap/2;
	int n = size / sizeof(short);

	for (int i=0;i<n;i++)
	{
	#ifdef __MCOSWAVE__
			x = i%gap - a;
			if((i/gap)%2)
				buf16[i] = (short)(vol * (2 - (3.14159 * x) * (3.14159 * x)/8/a/a));
			else
				buf16[i] = (short)(vol * (3.14159 * x) * (3.14159 * x)/8/a/a);
	#else
	#ifdef __COSWAVE__
			x = i%gap - a;
			if((i/gap)%2)
				buf16[i] = vol * (1 + sin((3.14159 * x)/2/a));
			else
				buf16[i] = vol * (1 - sin((3.14159 * x)/2/a));
	#else
	#ifdef __TRIWAVE__
			x = i%gap - a;
			if((i/gap)%2)
				buf16[i] = vol * ((1.0*x/a<0)?(0-1.*x/a):(1.0*x/a));
			else
				buf16[i] = vol * (2 - (1.0*x/a<0)?(0-1.*x/a):(1.0*x/a));
	#else
			if((i/gap)%2)
			{
				buf16[i] = vol;
			}
			else buf16[i] = 0;
	#endif
	#endif
	#endif
	}
	return size;
}

int mSound::createSong16(short *&wave, int freq[], int pau[], int clap, int n, short vol,
	short voice)
// return  the length of the song = (real) sizeof(wave) which maybe changed
// clap is the min of clap
{
	int i, j;
	int du = 1000; // a jiont length, millisec

	// init wave
	int len = 0, size, l;
	for (i=0;i<n;i++)
		len += pau[i]*du*RATE/clap/1000; // len is the number of samples
	cout<<"create song [16bit]"
	#ifdef __DEBUGE_CREATESONG16
		<<len * sizeof(short)
	#endif //__DEBUGE_CREATESONG16
	<<endl;
	if (!wave )//|| sizeof(wave)!=len*sizeof(short))
		wave = (short *)malloc(len * sizeof(short));
	else
		wave = (short *)realloc(wave, len);// * sizeof(short));
	if (!wave)
		cerr<<" realloc wave failed in createSong16"<<endl; 
	#ifdef __DEBUGE_CREATESONG16
		cout<<" n = "<<n<<" len = "<<len<<endl;
	#endif // __DEBUGE_CREATESONG16
	cout<<" realloc wave size : "<<2 * len/1024.0/1024<<" MByte"<<endl;

	len = 0; // save the addr of wave index
	for(i=0;i<n;i++)
	{
		#ifdef __DEBUGE_CREATESONG16
			cout<<"i "<<i<<endl;
		#endif // __DEBUGE_CREATESONG16
		if(!freq[i])
		{
			l = pau[i]*du*RATE/clap/1000; //duration*RATE*CHANNELS / 1000;
			for (j=0;j<l;j++)
			{
				wave[len + j] = voice;
			}
			len += l; // pau[i]*du*RATE/clap/1000;
		}
		else 
		{
			if((size = createTone16(freq[i], du*pau[i]/clap, vol))>0)
			{
				l = size/sizeof(short);
				smoothTone16(l, vol, voice);
				for (j=0;j<l;j++)
				{
					wave[len+j] = buf16[j];
				}
				len += l; // pau[i]*du*RATE/clap/1000;
			}
			else
			{
				cerr<<" some error occured in createSong16, i = "<<i<<endl;
				return -1;
			}
		}
	}
	// smoothSong(len,wave);
	cout<<"create song success : lenth = "<<len/1000.0<<"k samples"<<endl;
	return len*sizeof(short);
}

#define tran(x,len,a) (20.0*(x)*(a)*1.0/(len))
#define smoothfunc(x,len,a) (((tran(x,len,a)*tran(x,len,a)*tran(x,len,a)*\
exp(0-tran(x,len,a))+sin(3.1415*tran(x,len,a)/20)/2)/1.6) *\
(sin((x)*20.0)*0.3 + 1.0)) /// ******here
// (((x)*(a)*10.0/(len))*exp(0-(x)*(a)*10.0/(len))*2.7182)
//sin(a * 3.14159 * x / len) // (1. - 1./(1+(double)((a)*(x))/(len)))
#define None 0

int mSound::smoothTone(int n, unsigned char vol, unsigned char voice, unsigned char *buff)
// smooth the tone, n is the lenth of tone
// if success return n;else return -1;
{
	if (!buff)
		buff = buf;
	// double c = 1;
	// if (smoothfunc(n-1,n) < 0.9 )
	// {
	// 	c = vol / smoothfunc(n-1,n);
	// }
	#ifndef __NSMOOTHTONEDEBUG
		static int j = 0;
		if(j==6)cout<<" vol = "<<(int)vol<<
			// " c = "<<c<<
			" smooth f = "<<smoothfunc(n-1,n,3)<<
			" len = "<<n<<endl;
		j++;
	#endif

	for (int i=0;i<n;i++)
	{
		buff[i] = (unsigned char)(((int)buf[i]-vol) * 1 * smoothfunc(i,n,1) + voice);
		// * smoothfunc(i,n,9)*0.5+80);
	}
	return n;
}

int mSound::smoothTone16(int n, short vol, short voice, short *buff)
// smooth the tone16
// if success return n;else return -1;
{
	if (!buff)
		if(buf16)buff = buf16;
		else {
			cerr<<"buff input smoothTone16 is NULL"<<endl;
			return -1;
		}
	#ifndef __NSMOOTHTONEDEBUG
		static int j = 0;
		if(j==6)cout<<" vol = "<<(int)vol<<
			// " c = "<<c<<
			" smooth f = "<<smoothfunc(n-1,n,3)<<
			" len = "<<n<<endl;
		j++;
	#endif // __NSMOOTHTONEDEBUG

	for (int i=0;i<n;i++)
	{
		buff[i] = (short)(((int)buf16[i]-vol) * 1 * smoothfunc(i,n,1) + voice);
	}
	return n;
}


int mSound::smoothSong(int len, unsigned char *wave)
//return the len if success
{
	// len /=10;
	int halfFilt = 100;
	double filt = 0;
	#ifndef __NSMOOTHDEBUG
		FILE *data = fopen("smooth.dat","w");
		fprintf(data, "time\twave\tfilted wave\n");
	#endif
	for (int i = halfFilt;i<len - halfFilt;i++)
	{
		for (int j =0-halfFilt;j<halfFilt;j++) 
			filt += 1.0 * wave[i + j]*(1 - abs(j)*1.0/halfFilt)/halfFilt;
		//cout<<"filt = "<<filt<<" wave ["<<i<<"] = "<<(int)wave[i]<<endl;
	#ifndef __NSMOOTHDEBUG
			fprintf(data,"%d\t%d\t%lf\n",i,(int)wave[i],filt);
	#endif
		wave[i] = (unsigned char)filt;
		filt = 0;
		if(!(i%(len/1000)))printf("\b\b\b\b\b\b%3.1lf%%",(double)i/len*100);
	}
	#ifndef __NSMOOTHDEBUG
		fclose(data);
	#endif
	return len;
}


int mSound::saveSong(const char *filename, BYTE *wave, const int size,
	const WORD channels, const WORD rate, const WORD BitsPerSample)
{
	createWAVFile(filename, wave, size, channels, rate, BitsPerSample);
	return size;
}

int mSound::readSong(const char *filename, BYTE *&wave)
// if success return size;else return -1;
{
	int len;
	return readWAVFile(wavfHandler, filename, wave, len);
}

int mSound::createWAVFile(const char *filename, BYTE *wave, const int waveSize,
	const WORD channels, const WORD rate, const WORD BitsPerSample)
{
	cout<<"create wav file :"<<filename<<endl;
	FILE *fp = fopen(filename, "wb");
	if (!fp)
	{
		perror("[0] cannot open file in createWAVFile");
		return -1;
	}
	WAVFileHeader fh;
	//----------init file header------------
	struct RIFF_HEADER  riffHeader = 
	{
		{'R','I','F','F'},
		0,
		{'W','A','V','E'}
	};
	riffHeader.dwRiffSize = waveSize + sizeof(WAVFileHeader) - 8;
	struct FMT_BLOCK  	fmtBlock = 
	{
		{'f','m','t',' '},
		16,
		{
			0x0001, // fixed data
			channels,
			rate,
			0, // setted below
			BitsPerSample/8, // BytePerSample
			BitsPerSample
		}
	};
	fmtBlock.wavFormat.dwAvgBytesPerSec = fmtBlock.wavFormat.dwSamplesPerSec * (SIZE/8);
	fh.riffHeader = riffHeader;
	fh.fmtBlock = fmtBlock;
	fwrite(&fh, sizeof(WAVFileHeader),1,fp);
	FactBlockHeader  	fbh = 
	{
		{'f','a','c','t'},
		0,
	};
	char msg[] = "creator @jyhong";
	fbh.dwFactSize = sizeof(msg);
	fwrite(&fbh, sizeof(FactBlockHeader), 1, fp);
	fwrite(msg, sizeof(char), sizeof(msg), fp);
	WAVDataBlockHeader	dataHeader = 
	{
		{'d','a','t','a'},
		waveSize
	};
	fwrite(&dataHeader, sizeof(WAVDataBlockHeader), 1, fp);
	// fh.factBlock = factBlock;
	// fh.dataHeader = dataHeader;
	//----------init file header end--------
	int status = fwrite(wave, 1, waveSize, fp);// data size is waveSize Bytes
	if(status!=waveSize){
		if(ferror(fp))
			perror("[0] error in write wave to file @createWAVFile");
		else
			cerr<<"[0] cannot write "<<waveSize<<"B data to file"<<endl;
		return -1;
	}
	fclose(fp);
	cout<<" sizeof wavefileheader = "<<sizeof(WAVFileHeader)<<" "<<sizeof(fh)<<"B"<<endl<<
	" write "<<waveSize/1024.0/1024<<" MBytes to file \""<<filename<<"\", include "<<
	waveSize*8/BitsPerSample/1000.0<<"k ["<<BitsPerSample<<" bit] samples"<<endl;
	wavfHandler = fh; // save the fh to class
	cout<<"success"<<endl;
	return waveSize;
}

int mSound::readWAVFile(WAVFileHeader &fh, const char *filename, BYTE *&wave, 
	int &waveSize)
/**
	if read file completely return 0,
	if read file not complete return the FILE *,else failed return -1
*/
{
	FILE *fp = fopen(filename, "rb");
	if (!fp)
	{
		cout<<"file : "<<filename<<endl;
		perror("cannot open file in readWAVFile");
		return -1;
	}
	size_t readsize = fread(&fh, sizeof(WAVFileHeader),1,fp);
	if (!readsize)
	{
		cerr<<"read file "<<filename<<" error"<<endl;
		cout<<"maybe it is format error";
		return -1;
	}
	if (fh.riffHeader.szRiffID[0]!='R' ||
		fh.riffHeader.szRiffID[1]!='I' ||
		fh.riffHeader.szRiffID[2]!='F' ||
		fh.riffHeader.szRiffID[3]!='F' ||
		fh.riffHeader.szRiffFormat[0]!='W' ||
		fh.riffHeader.szRiffFormat[1]!='A' ||
		fh.riffHeader.szRiffFormat[2]!='V' ||
		fh.riffHeader.szRiffFormat[3]!='E' )
	{
		cerr<<"format error : not wav file"<<endl;
		return -1;
	}
	cout<<
	" file property:"<<endl<<
	#ifndef NDEBUGE
		"  FileHeaderSize\t"<< sizeof(WAVFileHeader)<<" B"<<endl<<
	#endif
	"  FileSize\t"     << (fh.riffHeader.dwRiffSize+8)/1024.0/1024.0<<" MB"<<endl<<
	"  Channels\t"      << fh.fmtBlock.wavFormat.wChannels       <<" ch"<<endl<<
	"  SamplesPerSec\t"<< fh.fmtBlock.wavFormat.dwSamplesPerSec/1000.0 <<" kHz"<<endl<<
	"  BitsPerSample\t" << fh.fmtBlock.wavFormat.wBitsPerSample  <<" bit"<<endl<<
	"  BlockAlign\t"    << fh.fmtBlock.wavFormat.wBlockAlign     <<" byte"<<endl;
	// "  DataSize\t"     << fh.dataHeader.dwDataSize/1024.0/1024.0<<" MB"<<endl<<
	
	riffChunkHeader rch;
	readsize = fread(&rch, sizeof(riffChunkHeader), 1, fp);
	if(!readsize)
	{
		cerr<<"[0] read riffChunkHeader error in readWAVFile"<<endl;
		return -1;
	}

	if (rch.szCHUNKID[0]=='f' &&
		rch.szCHUNKID[1]=='a' &&
		rch.szCHUNKID[2]=='c' &&
		rch.szCHUNKID[3]=='t' ) // fact
	{
		char *str = new char[rch.dwCHUNKSize+1];
		fread(str, 1, rch.dwCHUNKSize, fp);
		str[rch.dwCHUNKSize] = 0x00;// in case str is not a string
		cout<<"  FactMessage\t"<<"\""<<str<<"\""<<endl;
		delete [] str;

		readsize = fread(&rch, sizeof(riffChunkHeader), 1, fp);
		if(!readsize)
		{
			if(ferror(fp))
				perror("[0] read riffChunkHeader of data error in readWAVFile");
			else
				cerr<<"[0] unexpected file end in readWAVFile"<<endl;
			return -1;
		}
	}
	if (rch.szCHUNKID[0]!='d' ||
		rch.szCHUNKID[1]!='a' ||
		rch.szCHUNKID[2]!='t' ||
		rch.szCHUNKID[3]!='a' )
	{
		cerr<<"[0] format error : cannot find data"<<endl;
		// cout<<(int)fh.dataHeader.szDataID[0]<<
		// (int)fh.dataHeader.szDataID[1]<<
		// (int)fh.dataHeader.szDataID[2]<<
		// (int)fh.dataHeader.szDataID[3]<<"\""<<endl;
		return -1;
	}
	waveSize = rch.dwCHUNKSize;
	cout<<
	"  SoundLenth\t"   << 1.0*waveSize/fh.fmtBlock.wavFormat.dwAvgBytesPerSec<<" sec"<<
	endl<<
	endl;
	wave = (BYTE *)realloc(wave, waveSize);
	readsize = fread(wave, 1, waveSize, fp);
	if (readsize != waveSize)
	{
		if(ferror(fp))
			perror("[0] read file error in readWAVFile");
		else 
			cerr<<"[0] unexpected file end in readWAVFile"<<endl;
		return -1;
	}
	if(feof(fp))
	{
		fclose(fp);
		return 0;
	}
	else
	{
		return (int)fp;
	}
}

int mSound::exReadWAVFile(FILE *fp, BYTE *&wave, int &waveSize)
/**
	if wav file is not eof ,read continue
*/
{
	WAVDataBlockHeader wdbh;
	size_t readsize = fread(&wdbh, sizeof(WAVDataBlockHeader), 1, fp);
	if (wdbh.szDataID[0]!='d' ||
		wdbh.szDataID[1]!='a' ||
		wdbh.szDataID[2]!='t' ||
		wdbh.szDataID[3]!='a' )
	{
		cerr<<"format error : unexpected riff chunk : "<<
		wdbh.szDataID[0]<<
		wdbh.szDataID[1]<<
		wdbh.szDataID[2]<<
		wdbh.szDataID[3]<<endl;
		return -1;
	}
	waveSize = wdbh.dwDataSize;
	wave = (unsigned char *)realloc(wave, waveSize);
	if (!wave)
	{
		perror("realloc wave error in exReadWAVFile");
	}
	readsize = fread(wave, 1, waveSize, fp);
	if (readsize != waveSize)
	{
		cerr<<"read file error in exReadWAVFile";
		return -1;
	}
	if(feof(fp))
	{
		fclose(fp);
		return 0;
	}
	else
	{
		return (int)fp;
	}
}

int mSound::saveTones(const char *filename, int Tones[], int Pause[], int len)
{
	FILE *fp = fopen(filename, "wb");
	if (!fp)
	{
		perror("cannot open file when saveTones ");
		return -1;
	}
	TonFileHeader tfh = {
		{'T','O','N','E'},
		len * sizeof(int),
		len,
		RATE,
		CHANNELS,
		SIZE
	};
	fwrite(&tfh, sizeof(TonFileHeader), 1, fp);
	int ret = fwrite(Tones, sizeof(int), len, fp);
	if (ret!=len*sizeof(int))
	{
		perror("write Tones to file failed");
		return -1;
	}
	ret = fwrite(Pause, sizeof(int), len, fp);
	if (ret!=len*sizeof(int))
	{
		perror("write Tones to file failed");
		return -1;
	}

	fclose(fp);
	return len;
}

int mSound::readTones(const char *filename, int *&Tones, int *&Pause)
{
	FILE *fp = fopen(filename, "wb");
	if (!fp)
	{
		perror("cannot open file when readTones ");
		return -1;
	}
	TonFileHeader tfh;
	fread(&tfh, sizeof(TonFileHeader), 1, fp);
	if (tfh.Format[0] != 'T' || 
		tfh.Format[1] != 'O' || 
		tfh.Format[2] != 'N' || 
		tfh.Format[3] != 'E' )
	{
		cerr<<"format error : not TONE type file"<<endl;
		return -1;
	}
	int len = tfh.wTonesSize;
	if(Tones)delete [] Tones;
	if(Pause)delete [] Pause;
	Tones = new int[len];
	Pause = new int[len];
	int ret = fread(Tones, sizeof(int), len, fp);
	if (ret!=len*4)
	{
		perror("read Tones from file failed");
		return -1;
	}
	ret = fread(Pause, sizeof(int), len, fp);
	if (ret!=len*4)
	{
		perror("read Tones from file failed");
		return -1;
	}

	fclose(fp);
	return len;
}
