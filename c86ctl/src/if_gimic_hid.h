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
#include "ringbuff.h"
#include <list>


class GimicHID : public GimicIF
{
private:
	HANDLE hHandle;
	CRingBuff<UCHAR> rbuff;
private:
	GimicHID(HANDLE h);

public:
	~GimicHID(void);
	
	void Reset(void);
	void SetSSGVolume(uint8_t vol);
	void SetPLLClock(uint32_t clock);

	void Out(uint16_t addr, uint8_t data);
	void Tick(void);

public:
	static std::list< std::shared_ptr<GimicIF> > CreateInstances(void);
};

typedef std::shared_ptr<GimicHID> GimicHIDPtr;

