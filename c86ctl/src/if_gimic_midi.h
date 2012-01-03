/***
	c86ctl
	gimic コントロール MIDI版(実験コード)
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
	Thanks to Nagai "Guu" Osamu 2011/12/08 for his advice.
 */

#pragma once

#include "if.h"

#ifdef SUPPORT_MIDI

#include <mmsystem.h>
#include <vector>
#include "ringbuff.h"

class GimicMIDI : public GimicIF
{
private:
	GimicMIDI(HMIDIOUT h);

public:
	~GimicMIDI(void);

public:
	// IGimicModule
	virtual int __stdcall setSSGVolume(UCHAR vol);
	virtual int __stdcall setPLLClock(UINT clock);

public:
	// IRealChip
	virtual int __stdcall reset(void);
	virtual void __stdcall out(UINT addr, UCHAR data);
	virtual void __stdcall tick(void);

private:
	void sendSysEx( uint8_t *data, uint32_t sz );
	
private:
	HMIDIOUT hHandle;
	CRingBuff<UCHAR> rbuff;

public:
	static std::vector< std::shared_ptr<GimicIF> > CreateInstances(void);
};

typedef std::shared_ptr<GimicMIDI> GimicMIDIPtr;

#endif

