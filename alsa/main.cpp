/* 
* @Author: Junyuan Hong
* @Date:   2014-11-24 16:55:23
* @Last Modified by:   Junyuan Hong
* @Last Modified time: 2014-11-24 20:17:38
*/

#include <iostream>

#include "SoundAD.h"

using namespace std;

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
		else 
			sad.Record(atoi(argv[1]));
	else
		sad.Record(1);
    
    return 0;
}