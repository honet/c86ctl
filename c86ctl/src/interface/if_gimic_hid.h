/***
	c86ctl
	gimic コントロール HID版
	
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
#include "withlock.h"
#include "chip/chip.h"


namespace c86ctl{

class GimicHID : public DeviceIF
{
// ファクトリ -------------------------------------------------------
public:
	static int UpdateInstances( withlock< std::vector< std::shared_ptr<DeviceIF> > > &gimics);

// 公開インタフェイス -----------------------------------------------
public:
	// IGimic
	virtual int setSSGVolume(UCHAR idx, UCHAR vol);
	virtual int getSSGVolume(UCHAR idx, UCHAR *vol);
	virtual int setPLLClock(UCHAR idx, UINT clock);
	virtual int getPLLClock(UCHAR idx, UINT *clock);
	virtual int getMBInfo(struct Devinfo *info);
	virtual int getModuleInfo(UCHAR idx, struct Devinfo *info);
	virtual int getFWVer( UINT *major, UINT *minor, UINT *rev, UINT *build );
public:
	// IGimic2
	virtual int getModuleType(UCHAR idx, enum ChipType *type);

public:
	// IRealChip
	virtual int reset(void);
	virtual void out( UCHAR idx, UINT addr, UCHAR data);
	virtual UCHAR in( UCHAR idx, UINT addr );

public:
	// IRealChip2
	virtual int getChipStatus(UCHAR idx, UINT addr, UCHAR *status);
	virtual void directOut(UCHAR idx,UINT addr, UCHAR data);

// C86CTL内部利用 ---------------------------------------------------
private:
	GimicHID(HANDLE h);

public:
	~GimicHID(void);

public:
	// 非公開
	virtual int setDelay(int d);
	virtual int getDelay(int *d);
	virtual int isValid(void);

public:
	virtual void tick(void);
	virtual void update(void);
	virtual const GimicParam* getParam(UCHAR idx){ return &gimicParam; };
	
	virtual UINT getCPS(void){ return cps; };
	virtual void checkConnection(void);

public:
	virtual int getNumberOfModules();
	virtual bool isChipConnected(int idx);
	virtual int connectChip(int idx, IChip *dev);
	virtual int disconnectChip(int idx);
	
private:
	int init(void);

// プライベート -----------------------------------------------------
private:
	struct MSG{
		// なんとなく合計2-DWORDになるようにしてみた。
		UCHAR len;
		UCHAR dat[7];	// 最大メッセージ長は今のところ6byte.
	};
	
	struct REQ{
		UINT t;
		USHORT addr;
		UCHAR dat;
		UCHAR dummy;
	};

private:
	int sendMsg( MSG *data );
	int transaction( MSG *txdata, uint8_t *rxdata, uint32_t rxsz );
	void out2buf(UINT addr, UCHAR data);
	
	int devWrite( LPCVOID data )
	{
		if(!hHandle)
			return C86CTL_ERR_NODEVICE;
		
		DWORD len;
		int ret = WriteFile(hHandle, data, 65, &len, NULL);
		
		if(ret == 0 || 65 != len){
			CloseHandle(hHandle);
			hHandle = NULL;
			return C86CTL_ERR_UNKNOWN;
		}
		return C86CTL_ERR_NONE;
	};
	
	int devRead( LPVOID data )
	{
		if(!hHandle)
			return C86CTL_ERR_NODEVICE;
		
		DWORD len;
		if( !ReadFile( hHandle, data, 65, &len, NULL) ){
			CloseHandle(hHandle);
			hHandle = NULL;
			return C86CTL_ERR_UNKNOWN;
		}
		return C86CTL_ERR_NONE;
	}


private:
	HANDLE hHandle;
	std::basic_string<TCHAR> devPath;

	CRITICAL_SECTION csection;
	CRingBuff<MSG> rbuff;
	UINT cps, cal, calcount;

	int delay;
	CRingBuff<REQ> dqueue;
	
	IChip *chip;
	ChipType chiptype;
	GimicParam gimicParam;

protected:
	int refcount;
};

typedef std::shared_ptr<GimicHID> GimicHIDPtr;

};

#endif

