/***
	c86ctl
	gimic コントロール WinUSB版(実験コード)
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#pragma once

#ifdef SUPPORT_WINUSB

#include <winusb.h>
#include	"if.h"

class GIMICWinUSB : public IGIMICIF
{
private:
	HANDLE hUsb;
	WINUSB_INTERFACE_HANDLE hWinUsb;
	UCHAR IntrPipeId;
	USHORT IntrPipeMaxPktSize;

private:
	BOOL GetDevicePath(LPGUID InterfaceGuid, LPTSTR DevicePath, size_t BufLen);
	HANDLE OpenDevice(BOOL bSync);
	
  public:
	GIMICWinUSB(HINSTANCE,BUFFER*);
	~GIMICWinUSB(void);

	BOOL Open(void);
	void Close(void);
	void Reset(void);
	BOOL Write(UCHAR*,int);
};

#endif

