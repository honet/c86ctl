/***
	c86ctl
	gimic コントロール HID版(実験コード)
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
	Thanks to Nagai "Guu" Osamu 2011/12/08 for his advice.
 */
#pragma once

#include "if.h"

#ifdef SUPPORT_HID

#include <mmsystem.h>
#include <vector>
#include "ringbuff.h"


class GimicHID : public GimicIF
{
private:
	GimicHID(HANDLE h);

public:
	~GimicHID(void);

public:
	// IGimic
	virtual int __stdcall setSSGVolume(UCHAR vol);
	virtual int __stdcall getSSGVolume(UCHAR *vol);
	virtual int __stdcall setPLLClock(UINT clock);
	virtual int __stdcall getPLLClock(UINT *clock);
	virtual int __stdcall getMBInfo(struct Devinfo *info);
	virtual int __stdcall getModuleInfo(struct Devinfo *info);

public:
	// IRealChip
	virtual int __stdcall reset(void);
	virtual void __stdcall out(UINT addr, UCHAR data);

public:
	virtual void __stdcall tick(void);
	
private:
	HANDLE hHandle;
	CRingBuff<UCHAR> rbuff;
	
public:
	static std::vector< std::shared_ptr<GimicIF> > CreateInstances(void);
};

typedef std::shared_ptr<GimicHID> GimicHIDPtr;

#endif

