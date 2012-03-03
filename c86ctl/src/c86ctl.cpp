/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <process.h>
#include <mmsystem.h>

#include <string>
#include <list>
#include <vector>


#include "c86ctl.h"
#include "module.h"
#include "config.h"
#include "vis_c86main.h"
#include "ringbuff.h"
#include "if.h"
#include "if_gimic_hid.h"
#include "if_gimic_midi.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef _MANAGED
#pragma managed(push, off)
#endif

using namespace c86ctl;
using namespace c86ctl::vis;

// ------------------------------------------------------------------
// グローバル変数
ULONG_PTR gdiToken = 0;
Gdiplus::GdiplusStartupInput gdiInput;


// ------------------------------------------------------------------
// DLL Main
BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call){
	case DLL_PROCESS_ATTACH:
		gModule = hModule;
		gConfig.init(hModule);
		break;
	case DLL_PROCESS_DETACH:
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


// -----------------------------------------------------------------------
namespace c86ctl{
class C86Ctl : public IRealChipBase
{
public:
	C86Ctl() : 
	  isInitialized(false),
		  gMainThread(0),
		  mainThreadID(0),
		  gSenderThread(0),
		  senderThreadID(0),
		  timerPeriod(0),
		  terminateFlag(false),
		  refCount(0)
	{
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

protected:
	bool terminateFlag;
	std::vector< std::shared_ptr<GimicIF> > gGIMIC;

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
protected:
	UINT refCount;
};
};


C86Ctl gc86ctl;


// ---------------------------------------------------------
// 描画処理スレッド
// mm-timerによる60fps生成
unsigned int WINAPI C86Ctl::threadMain(LPVOID param)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	try{
		C86Ctl *pThis = reinterpret_cast<C86Ctl*>(param);
		if( Gdiplus::Ok != Gdiplus::GdiplusStartup(&gdiToken, &gdiInput, NULL) )
			throw "failed to initialize GDI+";

		//	if( !pThis->gGIMIC.empty() )
		//		mainWnd.attach( (COPNA*)pThis->gGIMIC.front()->getChip() );

		CVisManager *wm = new CVisManager();
		CVisC86Main *mainWnd = new CVisC86Main();
		
		mainWnd->attach( pThis->gGIMIC );
		wm->add( mainWnd );
		mainWnd->create();

		DWORD next = ::timeGetTime()*6 + 100;
		while(1){
			if( pThis->terminateFlag )
				break;
		
			// message proc
			if( ::PeekMessage(&msg , NULL , 0 , 0, PM_REMOVE )) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}

			// fps management
			DWORD now = ::timeGetTime() * 6;
			if(now < next){
				if( pThis->terminateFlag ) break;
				Sleep(1);
				continue;
			}
			next += 100;
			if( next < now ){
				//next = now;
				while(next<now) next += 100;
			}

			//update
			wm->draw();
		}
		wm->del( mainWnd );
		mainWnd->close();

		delete mainWnd;
		delete wm;
		
		Gdiplus::GdiplusShutdown(gdiToken);
	}catch(...){
	}
	
	return (DWORD)msg.wParam;
}


// ---------------------------------------------------------
// 演奏処理スレッド
// mm-timerによる1ms単位処理
// note: timeSetEvent()だと転送処理がタイマ周期より遅いときに
//       再入されるのが怖かったので自前ループにした
unsigned int WINAPI C86Ctl::threadSender(LPVOID param)
{
	try{
		const UINT period = 1;
		UINT now = ::timeGetTime();
		UINT next = now + period;
		UINT nextSec10 = now + 50;
		C86Ctl *pThis = reinterpret_cast<C86Ctl*>(param);

		while(1){
			if( pThis->terminateFlag )
				break;
		
			now = ::timeGetTime();
			if(now < next){
				if( pThis->terminateFlag ) break;
				Sleep(1);
				continue;
			}
			next += period;

			// update
			std::for_each( pThis->gGIMIC.begin(), pThis->gGIMIC.end(), [](std::shared_ptr<GimicIF> x){ x->tick(); } );
			
			if( nextSec10 < now ){
				nextSec10 += 50;
				std::for_each( pThis->gGIMIC.begin(), pThis->gGIMIC.end(), [](std::shared_ptr<GimicIF> x){ x->update(); } );
			}
		}
	}catch(...){
	}
	
	return 0;
}


// ---------------------------------------------------------
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
	return ++refCount;
}

ULONG C86Ctl::Release(VOID)
{
	return --refCount;
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
	if( gConfig.getInt(INISC_MAIN, _T("GUI"), 1) ){
		gMainThread = (HANDLE)_beginthreadex( NULL, 0, &threadMain, this, 0, &mainThreadID );
		if( !gMainThread )
			return C86CTL_ERR_UNKNOWN;
	}

	// 演奏スレッド開始
	gSenderThread = (HANDLE)_beginthreadex( NULL, 0, &threadSender, this, 0, &senderThreadID );
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

	reset();

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

