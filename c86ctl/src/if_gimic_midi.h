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
#include "chip.h"


class GimicMIDI : public GimicIF
{
private:
	GimicMIDI(HMIDIOUT h);

public:
	~GimicMIDI(void);

public:
	virtual int init(void);
	virtual void tick(void);
	virtual Chip* getChip(){ return chip; };

public:
	// IGimic
	virtual int __stdcall setSSGVolume(UCHAR vol);
	virtual int __stdcall getSSGVolume(UCHAR *vol);
	virtual int __stdcall setPLLClock(UINT clock);
	virtual int __stdcall getPLLClock(UINT *clock);
	virtual int __stdcall getMBInfo(struct Devinfo *info);
	virtual int __stdcall getModuleInfo(struct Devinfo *info);
	virtual int __stdcall getFWVer( UINT *major, UINT *minor, UINT *rev, UINT *build );

public:
	// IRealChip
	virtual int __stdcall reset(void);
	virtual void __stdcall out(UINT addr, UCHAR data);
	virtual UCHAR __stdcall in( UINT addr );

private:
	void sendSysEx( uint8_t *data, uint32_t sz );
	
private:
	HMIDIOUT hHandle;
	CRingBuff<UCHAR> rbuff;

	Chip *chip;
	ChipType chiptype;

public:
	static std::vector< std::shared_ptr<GimicIF> > CreateInstances(void);
};

typedef std::shared_ptr<GimicMIDI> GimicMIDIPtr;

#endif

