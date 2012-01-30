/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <process.h>
#include <mmsystem.h>

#include <string>
#include <list>
#include <vector>
#include <algorithm>



#include "c86ctl.h"
#include "module.h"
#include "config.h"
#include "vis_c86main.h"
#include "ringbuff.h"
#include "if.h"
#include "if_gimic_hid.h"
#include "if_gimic_midi.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

// ------------------------------------------------------------------


// ------------------------------------------------------------------
BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call){
	case DLL_PROCESS_ATTACH:
		initializeWndManager();
		gModule = hModule;
		gConfig.init(hModule);
		break;
	case DLL_PROCESS_DETACH:
		uninitializeWndManager();
		gModule = NULL;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


class C86Ctl : public IRealChipBase
{
public:
	C86Ctl(){
		isInitialized = false;
		gMainThread = 0;
		gSenderThread = 0;
		mainThreadID = 0;
		senderThreadID = 0;
		timerPeriod = 0;
		terminateFlag = 0;
	};
	~C86Ctl(){};

public:
	// IUnknown
	virtual HRESULT __stdcall QueryInterface( REFIID riid, LPVOID *ppvObj );
	virtual ULONG __stdcall AddRef(VOID);
	virtual ULONG __stdcall Release(VOID);

public:
	// IRealChipBase
	virtual int __stdcall initialize(void);
	virtual int __stdcall deinitialize(void);
	virtual int __stdcall getNumberOfChip(void);
	virtual HRESULT __stdcall getChipInterface( int id, REFIID riid, void** ppi );

public:
	int reset(void);
	void out( UINT addr, UCHAR data );
	UCHAR in( UINT addr );

public:
	static bool terminateFlag;
	static std::vector< std::shared_ptr<GimicIF> > gGIMIC;

protected:
	static unsigned int WINAPI threadMain(LPVOID param);
	static unsigned int WINAPI threadSender(LPVOID param);
	
protected:
	HANDLE gMainThread;
	HANDLE gSenderThread;
	UINT mainThreadID;
	UINT senderThreadID;
	DWORD timerPeriod;
	bool isInitialized;
};

C86Ctl gc86ctl;
bool C86Ctl::terminateFlag;
std::vector< std::shared_ptr<GimicIF> > C86Ctl::gGIMIC;




// 描画処理スレッド
// mm-timerによる60fps生成
unsigned int WINAPI C86Ctl::threadMain(LPVOID param)
{
	MSG msg;
	C86Ctl *pThis = reinterpret_cast<C86Ctl*>(param);
	CVisC86Main mainWnd;
	DWORD next;
	next = ::timeGetTime()*6 + 100;

	if( !pThis->gGIMIC.empty() )
		mainWnd.attach( (COPNA*)pThis->gGIMIC.front()->getChip() );

	mainWnd.create();
	while(1){
		if( terminateFlag )
			break;
		
		if( ::PeekMessage(&msg , NULL , 0 , 0, PM_REMOVE )) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		DWORD now = ::timeGetTime() * 6;
		if(now < next){
			if( terminateFlag ) break;
			Sleep(1);
			continue;
		}
		next += 100;

		//update();
	}
	mainWnd.close();
	mainWnd.detach();

	return (DWORD)msg.wParam;
}


// 演奏処理スレッド
// mm-timerによる1ms単位処理
// note: timeSetEvent()だと転送処理がタイマ周期より遅いときに
//       再入されるのが怖かったので自前ループにした
unsigned int WINAPI C86Ctl::threadSender(LPVOID param)
{
	const UINT period = 1;
	UINT next = ::timeGetTime() + period;

	while(1){
		if( terminateFlag )
			break;
		
		DWORD now = ::timeGetTime();
		if(now < next){
			if( terminateFlag ) break;
			Sleep(1);
			continue;
		}
		next += period;

		// update
		//for( std::list< std::shared_ptr<CGimicIF> >::iterator it = gGIMIC.begin(); it != gGIMIC.end(); it++ )
		//	(*it)->tick();
		std::for_each( gGIMIC.begin(), gGIMIC.end(), [](std::shared_ptr<GimicIF> x){ x->tick(); } );
	}
	
	return 0;
}


HRESULT C86Ctl::QueryInterface( REFIID riid, LPVOID *ppvObj )
{
	if( ::IsEqualGUID( riid, IID_IRealChipBase ) ){
		*ppvObj = (LPVOID)this;
		return NOERROR;
	}
	*ppvObj = 0;
	return E_NOINTERFACE;
}

ULONG C86Ctl::AddRef(VOID)
{
	return 1;
}

ULONG C86Ctl::Release(VOID)
{
	return 0;
}


int C86Ctl::initialize(void)
{
	if( isInitialized )
		return C86CTL_ERR_UNKNOWN;

	// インスタンス生成
	int type = gConfig.getInt(INISC_MAIN, INIKEY_GIMICIFTYPE, 0);
	if( type==0 ){
		gGIMIC = GimicHID::CreateInstances();
	}else if( type==1 ){
		gGIMIC = GimicMIDI::CreateInstances();
	}
	
	// タイマ分解能設定
	TIMECAPS timeCaps;
	if( ::timeGetDevCaps(&timeCaps, sizeof(timeCaps)) == TIMERR_NOERROR ){
		::timeBeginPeriod(timeCaps.wPeriodMin);
		timerPeriod = timeCaps.wPeriodMin;
	}

	// 描画スレッド開始
//	if( gConfig.getInt(INISC_MAIN, _T("GUI"), 1) ){
	// 2012/1/9 honet: OPMの時に激しくバグっているので一旦無効化
	if(0){
		gMainThread = (HANDLE)_beginthreadex( NULL, 0, &threadMain, NULL, 0, &mainThreadID );
		if( !gMainThread )
			return C86CTL_ERR_UNKNOWN;
	}

	// 演奏スレッド開始
	gSenderThread = (HANDLE)_beginthreadex( NULL, 0, &threadSender, NULL, 0, &senderThreadID );
	if( !gSenderThread ){
		SetThreadPriority( gSenderThread, THREAD_PRIORITY_ABOVE_NORMAL );
		terminateFlag = true;
		::WaitForSingleObject( gMainThread, INFINITE );
		return C86CTL_ERR_UNKNOWN;
	}
	isInitialized = true;
	return C86CTL_ERR_NONE;
}

int C86Ctl::deinitialize(void)
{
	if( !isInitialized )
		return C86CTL_ERR_UNKNOWN;

	c86ctl_reset();

	// 各種スレッド終了
	terminateFlag = true;
	if( gMainThread ){
		::WaitForSingleObject( gMainThread, INFINITE );
		gMainThread = NULL;
		mainThreadID = 0;
	}
	if( gSenderThread ){
		::WaitForSingleObject( gSenderThread, INFINITE );
		gSenderThread = NULL;
		senderThreadID = 0;
	}
	terminateFlag = false;

	// インスタンス削除
	// note: このタイミングで終了処理が行われる。
	gGIMIC.clear();

	// タイマ分解能設定解除
	::timeEndPeriod(timerPeriod);
	isInitialized = false;
	
	return C86CTL_ERR_NONE;
}

int C86Ctl::reset(void)
{
	std::for_each( gGIMIC.begin(), gGIMIC.end(), [](std::shared_ptr<GimicIF> x){ x->reset(); } );
	return 0;
}

int C86Ctl::getNumberOfChip(void)
{
	return gGIMIC.size();
}

HRESULT C86Ctl::getChipInterface( int id, REFIID riid, void** ppi )
{
	if( id < gGIMIC.size() ){
		return gGIMIC[id]->QueryInterface( riid, ppi );
	}
	return E_NOINTERFACE;
}

void C86Ctl::out( UINT addr, UCHAR data )
{
	if( gGIMIC.size() ){
		gGIMIC.front()->out(addr,data);
	}
}

UCHAR C86Ctl::in( UINT addr )
{
	if( gGIMIC.size() ){
		return gGIMIC.front()->in(addr);
	}else
		return 0;
}

//getModuleType()


// ----------------------------------------------------------------------
// 外部インターフェイス
HRESULT WINAPI CreateInstance( REFIID riid, void** ppi )
{
	// C86Ctlが単一インスタンスなので手抜き実装。
	return gc86ctl.QueryInterface(riid,ppi);
}

// ---------------------------------------------------
// 以下は後方互換のためのnative I/F
int WINAPI c86ctl_initialize(void)
{
	return gc86ctl.initialize();
}

int WINAPI c86ctl_deinitialize(void)
{
	return gc86ctl.deinitialize();
}

int WINAPI c86ctl_reset(void)
{
	return gc86ctl.reset();
}

void WINAPI c86ctl_out( UINT addr, UCHAR data )
{
	gc86ctl.out( addr, data );
}

UCHAR WINAPI c86ctl_in( UINT addr )
{
	return gc86ctl.in( addr );
}

