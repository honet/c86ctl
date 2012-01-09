/***
	c86ctl
	I/Fアクセスインタフェース定義 for GIMIC
	
	Copyright (c) 2009-2010, honet. All rights reserved.
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

class GimicIF : public IRealChip, public IGimic
{
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
		}else if( ::IsEqualIID( riid, IID_IGimic ) ){
			*ppvObj = static_cast<IGimic*>(this);
			return NOERROR;
		}
		*ppvObj = NULL;
		return E_NOINTERFACE;
	};

	// TODO: こいつら真面目に書く。
	virtual ULONG __stdcall AddRef(VOID){ return 1; };
	virtual ULONG __stdcall Release(VOID){ return 0; };

public:
	// IRealChip
	virtual int __stdcall reset(void){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual void __stdcall out( UINT addr, UCHAR data){};
	virtual UCHAR __stdcall in( UINT addr ){ return 0; };

public:
	// IGimic
	int __stdcall setSSGVolume(UCHAR vol){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	int __stdcall getSSGVolume(UCHAR *vol){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	int __stdcall setPLLClock(UINT clock){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	int __stdcall getPLLClock(UINT *clock){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall getFWVer( UINT *major, UINT *minor, UINT *rev, UINT *build ){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall getMBInfo(struct Devinfo *info){ return C86CTL_ERR_NOT_IMPLEMENTED; };
	virtual int __stdcall getModuleInfo(struct Devinfo *info){ return C86CTL_ERR_NOT_IMPLEMENTED; };
};

typedef std::shared_ptr<GimicIF> GimicIFPtr;
