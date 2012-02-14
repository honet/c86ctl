/***
	c86ctl
	I/Fアクセスインタフェース定義 for GIMIC
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
	Thanks to Nagai "Guu" Osamu 2011/12/08 for his advice.
 */
#pragma once
#include <memory>
#include "c86ctl.h"
#include "chip.h"

//#define SUPPORT_WINUSB
#define SUPPORT_MIDI
#define SUPPORT_HID

class GimicIF : public IRealChip2, public IGimic
{
public:
	GimicIF() : refcount(0){ };
	virtual ~GimicIF(){};

public:
	virtual int init(void){ return 0; };
	virtual void tick(void){};
	virtual Chip* getChip(){ return 0; };

public:
	// IUnknown
	virtual HRESULT __stdcall QueryInterface( REFIID riid, LPVOID *ppvObj ){
		if( !ppvObj )
			return ERROR_INVALID_PARAMETER;
		
		if( ::IsEqualIID( riid, IID_IRealChip ) ){
			*ppvObj = static_cast<IRealChip*>(this);
			return NOERROR;
		}else if( ::IsEqualIID( riid, IID_IRealChip2 ) ){
			*ppvObj = static_cast<IRealChip2*>(this);
			return NOERROR;
		}else if( ::IsEqualIID( riid, IID_IGimic ) ){
			*ppvObj = static_cast<IGimic*>(this);
			return NOERROR;
		}
		*ppvObj = NULL;
		return E_NOINTERFACE;
	};

	virtual ULONG __stdcall AddRef(VOID){ return ++refcount; };
	virtual ULONG __stdcall Release(VOID){ return --refcount; };

public:
	// IRealChip
	virtual int __stdcall reset(void){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual void __stdcall out( UINT addr, UCHAR data){};
	virtual UCHAR __stdcall in( UINT addr ){ return 0; };

public:
	// IRealChip2
	virtual int __stdcall getChipStatus( UINT addr, UCHAR *status ){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall adpcmZeroClear(void){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall adpcmWrite( UINT startAddr, UINT size, UCHAR *data ){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall adpcmRead( UINT startAddr, UINT size, UCHAR *data ){ return C86CTL_ERR_NOT_IMPLEMENTED; };

public:
	// IGimic
	virtual int __stdcall setSSGVolume(UCHAR vol){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall getSSGVolume(UCHAR *vol){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall setPLLClock(UINT clock){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall getPLLClock(UINT *clock){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall getFWVer( UINT *major, UINT *minor, UINT *rev, UINT *build ){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall getMBInfo(struct Devinfo *info){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall getModuleInfo(struct Devinfo *info){ return C86CTL_ERR_NOT_IMPLEMENTED; };
public:
	// IGimic2
	virtual int __stdcall getModuleType(enum ChipType *type){ return C86CTL_ERR_NOT_IMPLEMENTED; };

public:
	// 非公開
	virtual UINT getCPS(void){ return 0; };
	virtual void update(void){ };
	virtual void directOut(UINT addr, UCHAR data){};

protected:
	int refcount;
};

typedef std::shared_ptr<GimicIF> GimicIFPtr;
