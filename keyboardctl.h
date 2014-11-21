#ifndef __KEYBOARDCTL_H__
#define __KEYBOARDCTL_H__

int getch(void);

#define PWM_IOCTL_SET_FREQ	1
#define PWM_IOCTL_STOP		0
#define ESC_KEY 			0x1b	// esc key

#endif // __KEYBOARDCTL_H__