/***
	c86ctl
	論理デバイス定義
	
	Copyright (c) 2009-2014, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once
#include <memory>
#include <vector>
#include "c86ctl.h"
#include "chip/chip.h"
#include "if.h"
#include "stream.h"

namespace c86ctl{

class LogicalDevice : public IRealChip3, public IGimic2, public IC86Box
{
public:
	LogicalDevice();
	virtual ~LogicalDevice();

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
		}else if( ::IsEqualIID( riid, IID_IRealChip3 ) ){
			*ppvObj = static_cast<IRealChip3*>(this);
			return NOERROR;
		}else if( ::IsEqualIID( riid, IID_IGimic ) ){
			*ppvObj = static_cast<IGimic*>(this);
			return NOERROR;
		}else if( ::IsEqualIID( riid, IID_IGimic2 ) ){
			*ppvObj = static_cast<IGimic2*>(this);
			return NOERROR;
		//TODO: まだ中身実装してない。
		//}else if( ::IsEqualIID( riid, IID_IC86BOX ) ){
		//	*ppvObj = static_cast<IC86Box*>(this);
		//	return NOERROR;
		}
		*ppvObj = NULL;
		return E_NOINTERFACE;
	};

	virtual ULONG __stdcall AddRef(VOID){ return ++refcount; };
	virtual ULONG __stdcall Release(VOID){ return --refcount; };


public:
	// IRealChip
	virtual int __stdcall reset(void);
	virtual void __stdcall out( UINT addr, UCHAR data);
	virtual UCHAR __stdcall in( UINT addr );
	// IRealChip2
	virtual int __stdcall getChipStatus( UINT addr, UCHAR *status );
	virtual void __stdcall directOut(UINT addr, UCHAR data);
	// IRealChip3
	virtual int __stdcall getChipType( enum ChipType *type );
	
public:
	// IGimic
	virtual int __stdcall setSSGVolume(UCHAR vol);
	virtual int __stdcall getSSGVolume(UCHAR *vol);
	virtual int __stdcall setPLLClock(UINT clock);
	virtual int __stdcall getPLLClock(UINT *clock);
	virtual int __stdcall getMBInfo(struct Devinfo *info);
	virtual int __stdcall getModuleInfo(struct Devinfo *info);
public:
	// IGimic, IC86Box
	virtual int __stdcall getFWVer( UINT *major, UINT *minor, UINT *rev, UINT *build );
public:
	// IGimic2, IC86Box
	virtual int __stdcall getModuleType(enum ChipType *type);

public:
	// IC86Box
	virtual int __stdcall getBoardType(CBUS_BOARD_TYPE *boardType);
	virtual int __stdcall getSlotIndex();
	virtual int __stdcall writeBoardControl(UINT index, UINT val);
	
	
public:
	// 非公開
	virtual int __stdcall isValid(void);

public:
	// 非公開
	virtual int init(void){ return 0; };
	virtual UINT getCPS(void){ return 0; };

	void connect(Stream *s);
	void disconnect(Stream *s);
	bool isConnectedTo(Stream *s);

private:
	std::vector<Stream*> streams;

protected:
	int refcount;
};

typedef std::shared_ptr<LogicalDevice> LogicalDevicePtr;

};

