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

//#define SUPPORT_WINUSB
#define SUPPORT_MIDI
#define SUPPORT_HID

class GimicIF : public IRealChip, public IGimicModule
{
public:
	virtual int __stdcall  init(void){ return 0; };
	virtual void __stdcall tick(void){};

public:
	// IUnknown
	virtual HRESULT __stdcall QueryInterface( REFIID riid, LPVOID *ppvObj ){
		if( ::IsEqualIID( riid, IID_IRealChip ) ){
			*ppvObj = (LPVOID)this;
			return NOERROR;
		}else if( ::IsEqualIID( riid, IID_IGimicModule ) ){
			*ppvObj = (LPVOID)this;
			return NOERROR;
		}
		*ppvObj = NULL;
		return E_NOINTERFACE;
	};
	virtual ULONG __stdcall AddRef(VOID){ return 1; };
	virtual ULONG __stdcall Release(VOID){ return 0; };

public:
	// IRealChip
	virtual int __stdcall reset(void){ return 0; };
	virtual void __stdcall out( UINT addr, UCHAR data){};
	virtual UCHAR __stdcall in( UINT addr ){ return 0; };

public:
	// IGimicModule
	int __stdcall setSSGVolume(UCHAR vol){ return 0; };
	int __stdcall setPLLClock(UINT clock){ return 0; };
};

typedef std::shared_ptr<GimicIF> GimicIFPtr;
