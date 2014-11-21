#ifndef __SOUND_H__
#define __SOUND_H__

#include <stdio.h>

#define LENGTH   3     // last 3 seconds
#define RATE     44100 // 22050 //8000  // sample rate = (sample num)/1sec
#define SIZE     8     // data chunk size
#define CHANNELS 1     // channel number

class mSound
{
//--------WAV  FILE----------------
private:
	typedef unsigned int   DWORD;
	typedef unsigned short WORD;
	typedef unsigned char  BYTE;

	typedef struct RIFF_CHUNK_HEADER  {
		char szCHUNKID[4]; // ID
		DWORD dwCHUNKSize; // size
	} riffChunkHeader; // followed by chunk data
	
	struct RIFF_HEADER  {
		char  szRiffID[4];     // 'R','I','F','F'  
		DWORD dwRiffSize;      // filesize - 8
		char  szRiffFormat[4]; // 'W','A','V','E' 
	};// 12 B
	struct WAVE_FORMAT  { 
		WORD  wFormatTag;		// 0x0001
		WORD  wChannels;		// CHANNELS
		DWORD dwSamplesPerSec;  // RATE
		DWORD dwAvgBytesPerSec; //   
		WORD  wBlockAlign;		// BytePerSample
		WORD  wBitsPerSample;	// SIZE
	};// 16 B
	struct FMT_BLOCK { 
		char   szFmtID[4]; // 'f','m','t',' '   
		DWORD  dwFmtSize;  // 16
		struct WAVE_FORMAT wavFormat;  
	};// 24 B
	typedef struct FACT_BLOCK  {
		char   szFactID[4]; // 'f','a','c','t'
		DWORD  dwFactSize;  // 4
		// char   msg[4];      // some message define by creator
	}FactBlockHeader;// 12 B
	typedef struct DATA_BLOCK  {
		char  szDataID[4]; // 'd','a','t','a'
		DWORD dwDataSize;  // data size
	}WAVDataBlockHeader; // 8 B wave data will follow this block
	typedef struct tagWAVFileHeader  {
		struct RIFF_HEADER  riffHeader;
		struct FMT_BLOCK  	fmtBlock;
		// struct FACT_BLOCK  	factBlock;
		// struct DATA_BLOCK	dataHeader;
	}WAVFileHeader;// 56 B
public:
	int createWAVFile(const char *filename, BYTE *wave, const int waveSize,
		const WORD channels, const WORD rate, const WORD BitsPerSample);
	int readWAVFile(WAVFileHeader &fh, const char *filename, BYTE *&wave, int &waveSize);
	/**
		if read file completely return 0,
		if read file not complete return the FILE *,else failed return -1
	*/
	int exReadWAVFile(FILE *fp, BYTE *&wave, int &waveSize);
	//-----------WAV File end----------
private:
	unsigned char *buf; // tone buf for 8bit sample
	short         *buf16; // tone buf for 16bit sample, will be used in createTone16 fucntion
	int           buf16Size; // save the buf16 size
	int           tHandler; // temp Handler
	WAVFileHeader wavfHandler; // temp wav file handler

public:
	mSound();
	~mSound();

	int playFreq(int handler, int freq, int duration, unsigned char vol);
	int playTunes(int handler, int freq[], int pau[], int part, int n, unsigned char vol);
	int tInitDSP(int size = SIZE, int channels = CHANNELS, int rate = RATE);
	int initDSP(int &handler, int size = SIZE, int channels = CHANNELS, int rate = RATE);
	int playSora();
	// play the song,"the city of sky"

	int playSong(int handler, unsigned char wave[], int len);
	// the wave may be created by createSong function
	int playSong(const char *filename);
	int createSong(unsigned char *&wave, int freq[], int pau[], int clap, int n, 
		unsigned char vol = 20, unsigned char voice = 128);
	// return  the length of the song = (real) sizeof(wave) which maybe changed
	
	int createTone(int freq, int duration, unsigned char vol);
	/** similar to playFreq function ,but not play
		return by the buf
		return the size of buf
	*/
	int smoothTone(int n, unsigned char vol, unsigned char voice = 100, unsigned char *buff =0);
	// smooth the tone
	// if success return n;else return -1;
	int smoothSong(int len, unsigned char *wave);
	//return the len if success

	// ----------16bit sample functions-------------
	int createSong16(short *&wave, int freq[], int pau[], int clap, int n, short vol = 10000,
		short voice = 1000);
	// return  the length of the song = (real) sizeof(wave) which maybe changed
	
	int createTone16(int freq, int duration, short vol);
	/** similar to playFreq function ,but not play
		return by the buf
		return the size of buf
	*/
	int smoothTone16(int n, short vol, short voice = 100, short *buff =0);

	// ---------------My Tone file type------
	typedef struct tagFileHeader
	{
		char Format[4];// "TONE"
		unsigned int dataSize;
		unsigned int wTonesSize;
		// unsigned int dataAddr;
		unsigned int wRate;		// Hz
		//unsigned int chunkSize;	// bit
		unsigned int wChannels;	// 
		unsigned int wBitsPerSample;
	}TonFileHeader; // my file type

	// ------------------File operation------------------
	int saveSong(const char *filename, unsigned char *wave, const int size,
		const WORD channels, const WORD rate, const WORD BitsPerSample);
	int readSong(const char *filename, unsigned char *&wave);
	int saveTones(const char *filename, int Tones[], int Pause[],int len);
	int readTones(const char *filename, int *&Tones, int *&Pause);
	// if success return len;else return -1;
	// ------------------File operation end--------------

};

// controls
#define __NSMOOTHDEBUG
#define __NSMOOTHTONEDEBUG
#define __TONE16__

#define NDEBUGE
#ifndef NDEBUGE
	#define __DEBUGE_PLAYSORA // in playSora
	#define __DEBUGE_CREATESONG16 // int createSong16
#endif
	#define __DEBUGE_PLAYSONG

#endif //__SOUND_H__