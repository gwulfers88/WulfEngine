#ifndef SYSWIN_H
#define SYSWIN_H

#include "platform.h"

struct Win32Buffer
{
	BITMAPINFO bmi;
	i32 width;
	i32 height;
	u32 bpp;
	u32 pitch;
	void* data;
};

struct Win32WndDim
{
	i32 width;
	i32 height;
};

#endif
