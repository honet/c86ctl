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
#include <list>
#include "ringbuff.h"

class GimicMIDI : public GimicIF {
private:
	HMIDIOUT hHandle;
	CRingBuff<UCHAR> rbuff;

private:
	GimicMIDI(HMIDIOUT h);
	void SendSysEx( uint8_t *data, uint32_t sz );

public:
	~GimicMIDI(void);

	void Reset(void);
	void SetSSGVolume(uint8_t vol);
	void SetPLLClock(uint32_t clock);
	
	void Out(uint16_t addr, uint8_t data);
	void Tick(void);

public:
	static std::list< std::shared_ptr<GimicIF> > CreateInstances(void);
};

typedef std::shared_ptr<GimicMIDI> GimicMIDIPtr;

#endif