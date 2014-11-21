#include "keyboardctl.h"
#include <iostream>
using namespace std;
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>


int getch(void)
{
	struct termios oldt,newt;	// terminals

	int ch;
	if (!isatty(STDIN_FILENO))
	{
		perror("please run in a terminal");
		return -1;
	}

	// save terminal setting
	if(tcgetattr(STDIN_FILENO, &oldt) < 0)
	{
		perror("save the terminal setting");
		return -1;
	}

	// set terminal as need
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO,TCSANOW, &newt) < 0)
	{
		perror("set terminal");
		return -1;
	}
	ch = getchar();
	// restore terminal setting
	if (tcsetattr(STDIN_FILENO,TCSANOW, &oldt) < 0)
	{
		perror("restore the terminal setting");
		return -1;
	}

	return ch;
}