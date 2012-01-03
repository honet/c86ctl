/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */


#ifndef _C86CTL_H
#define _C86CTL_H

#include <ObjBase.h>

#ifdef __cplusplus
extern "C" {
#endif


// エラーコード定義
#define C86CTL_ERR_NONE						0
#define C86CTL_ERR_UNKNOWN					-1
#define C86CTL_ERR_NOT_IMPLEMENTED			-9999
#define C86CTL_ERR_NODEVICE					-1000

// IRealChip {F959C007-6B4D-46F3-BB60-9B0897C7E642}
static const GUID IID_IRealChip = 
{ 0xf959c007, 0x6b4d, 0x46f3, { 0xbb, 0x60, 0x9b, 0x8, 0x97, 0xc7, 0xe6, 0x42 } };

// IRealChipUnit {F959C007-6B4D-46F3-BB60-9B0897C7E642}
static const GUID IID_IRealChipBase = 
{ 0xf959c007, 0x6b4d, 0x46f3, { 0xbb, 0x60, 0x9b, 0x8, 0x97, 0xc7, 0xe6, 0x42 } };

// IGimicModule{175C7DA0-8AA5-4173-96DA-BB43B8EB8F17}
static const GUID IID_IGimicModule = 
{ 0x175c7da0, 0x8aa5, 0x4173, { 0x96, 0xda, 0xbb, 0x43, 0xb8, 0xeb, 0x8f, 0x17 } };

interface IRealChipBase : public IUnknown
{
	virtual int __stdcall initialize(void) = 0;
	virtual int __stdcall deinitialize(void) = 0;
	virtual int __stdcall getNumberOfChip(void) = 0;
	virtual HRESULT __stdcall getChipInterface( int id, REFIID riid, void** ppi ) = 0;
};

interface IRealChip : public IUnknown
{
public:
	virtual int __stdcall reset(void) = 0;
	virtual void __stdcall out( UINT addr, UCHAR data ) = 0;
	virtual UCHAR __stdcall in( UINT addr ) = 0;
	//virtual __stdcall getModuleType() = 0;
};

interface IGimicModule : public IUnknown
{
	virtual int __stdcall setSSGVolume(UCHAR vol) = 0;
	virtual int __stdcall setPLLClock(UINT clock) = 0;
};


// 公開ファンクション
HRESULT CreateInstance( REFIID riid, void** ppi );

int WINAPI c86ctl_initialize(void);
int WINAPI c86ctl_deinitialize(void);
int WINAPI c86ctl_reset(void);

void WINAPI c86ctl_out( UINT addr, UCHAR data );
UCHAR WINAPI c86ctl_in( UINT addr );

// ver1.1 追加ファンクション
//C86CTL_API INT c86ctl_get_version(UINT *ver);
//C86CTL_API INT c86ctl_out2(UINT module, UINT addr, UCHAR adata );
//C86CTL_API INT c86ctl_in2(UINT module, UINT addr, UCHAR data );
//C86CTL_API INT c86ctl_set_pll_clock(UINT module, UINT clock );
//C86CTL_API INT c86ctl_set_volume(UINT module, UINT ch, UINT vol );


	
#ifdef __cplusplus
}
#endif

#endif
