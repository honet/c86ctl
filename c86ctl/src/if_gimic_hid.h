/***
	c86ctl
	gimic コントロール HID版(実験コード)
	
	Copyright (c) 2009-2012, honet. All rights reserved.
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
public:
	struct MSG{
		// なんとなく合計2-DWORDになるようにしてみた。
		UCHAR len;
		UCHAR dat[7];	// 最大メッセージ長は今のところ6byte.
	};
	
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
	virtual int __stdcall getFWVer( UINT *major, UINT *minor, UINT *rev, UINT *build );
public:
	// IGimic2
	virtual int __stdcall getModuleType(enum ChipType *type);

public:
	// IRealChip
	virtual int __stdcall reset(void);
	virtual void __stdcall out(UINT addr, UCHAR data);
	virtual UCHAR __stdcall in( UINT addr );

public:
	// IRealChip2
	virtual int __stdcall getChipStatus( UINT addr, UCHAR *status );
	virtual int __stdcall adpcmZeroClear(void);
	virtual int __stdcall adpcmWrite( UINT startAddr, UINT size, UCHAR *data );
	virtual int __stdcall adpcmRead( UINT startAddr, UINT size, UCHAR *data );

public:
	virtual UINT getCPS(void){ return cps; };
	virtual void update(void);
	
private:
	int sendMsg( MSG *data );
	int transaction( MSG *txdata, uint8_t *rxdata, uint32_t rxsz );
	virtual void directOut(UINT addr, UCHAR data);
	
private:
	HANDLE hHandle;
	CRITICAL_SECTION csection;
	CRingBuff<MSG> rbuff;
	uint32_t seqno;
	UINT cps, cal, calcount;

	Chip *chip;
	ChipType chiptype;
	
public:
	static std::vector< std::shared_ptr<GimicIF> > CreateInstances(void);
};

typedef std::shared_ptr<GimicHID> GimicHIDPtr;

#endif

