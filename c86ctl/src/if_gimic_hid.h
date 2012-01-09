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
#include "chip.h"


class GimicHID : public GimicIF
{
private:
	GimicHID(HANDLE h);

public:
	~GimicHID(void);

public:
	virtual int init(void);
	virtual void tick(void);
	virtual Chip* getChip(void){ return chip; };
	
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
	virtual UCHAR __stdcall in( UINT addr );

private:
	int sendData( uint8_t *data, uint32_t sz );
	int transaction( uint8_t *txdata, uint32_t txsz,
					 uint8_t *rxdata, uint32_t rxsz );
	
private:
	HANDLE hHandle;
	CRingBuff<UCHAR> rbuff;
	uint32_t seqno;

	Chip *chip;
	ChipType chiptype;
	
public:
	static std::vector< std::shared_ptr<GimicIF> > CreateInstances(void);
};

typedef std::shared_ptr<GimicHID> GimicHIDPtr;

#endif

