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
#include <WinUser.h>
#include <Dbt.h>
#include <ShellAPI.h>

#include <string>
#include <list>
#include <vector>

extern "C" {
#include "hidsdi.h"
}

#include "resource.h"
#include "c86ctl.h"
#include "c86ctlmain.h"
#include "c86ctlmainwnd.h"

//#include "module.h"
#include "config.h"
#include "vis/vis_c86main.h"
#include "ringbuff.h"
#include "interface/if.h"
#include "interface/if_gimic_hid.h"
#include "interface/if_gimic_midi.h"


#pragma comment(lib,"hidclass.lib")

//#define _CRTDBG_MAP_ALLOC
//#ifdef _DEBUG
//#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
//#endif

using namespace c86ctl;
using namespace c86ctl::vis;


// ------------------------------------------------------------------
#define WM_THREADEXIT       (WM_APP+10)
#define WM_TASKTRAY_EVENT   (WM_APP+11)


C86CtlMainWnd *C86CtlMainWnd::pthis = 0;

const TCHAR szAppName[] = _T("msg-receiver");

int C86CtlMainWnd::createMainWnd(LPVOID param)
{
	WNDCLASSEX  wndclass;

	HINSTANCE hinst = C86CtlMain::getInstanceHandle();

	// メッセージ処理用ウィンドウ生成
	wndclass.cbSize        = sizeof(wndclass);
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = wndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = hinst;
	wndclass.hIcon         = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_C86CTL));
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm       = LoadIcon (NULL, IDI_APPLICATION);

	if( !RegisterClassEx(&wndclass) )
		return -1;

	hwnd = ::CreateWindowEx(
		0, szAppName, NULL, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,CW_USEDEFAULT, CW_USEDEFAULT,
		HWND_MESSAGE, NULL, hinst, NULL);
	
	if(!hwnd)
		return -1;

	// タスクトレイアイコンの登録
	notifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	notifyIcon.uID = 0;
	notifyIcon.hWnd = hwnd;
	notifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notifyIcon.hIcon = ::LoadIcon(hinst, MAKEINTRESOURCE(IDI_ICON_C86CTL));
	notifyIcon.uCallbackMessage = WM_TASKTRAY_EVENT;
	lstrcpy( notifyIcon.szTip, _T("C86CTL") );
	::Shell_NotifyIcon( NIM_ADD, &notifyIcon );


	// デバイス挿抜監視登録
	DEV_BROADCAST_DEVICEINTERFACE *pFilterData = (DEV_BROADCAST_DEVICEINTERFACE*)_alloca(sizeof(DEV_BROADCAST_DEVICEINTERFACE));
	if( pFilterData ){
		ZeroMemory(pFilterData, sizeof(DEV_BROADCAST_DEVICEINTERFACE));

		pFilterData->dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		pFilterData->dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		HidD_GetHidGuid(&pFilterData->dbcc_classguid);

		hNotifyDevNode = ::RegisterDeviceNotification(hwnd, pFilterData, DEVICE_NOTIFY_WINDOW_HANDLE);
	}

	if( gConfig.getInt(INISC_MAIN, _T("GUI"), 1) )
		startVis();


	return 0;
}

int C86CtlMainWnd::deviceUpdate()
{
	if( mainVisWnd ){
		mainVisWnd->update();
	}
	return 0;
}

int C86CtlMainWnd::destroyMainWnd(LPVOID param)
{
	if(hNotifyDevNode){
		::UnregisterDeviceNotification(hNotifyDevNode);
	}
	if(hwnd){
		::Shell_NotifyIcon( NIM_DELETE, &notifyIcon );
		::DestroyWindow(hwnd);
		HINSTANCE hinst = C86CtlMain::getInstanceHandle();
		::UnregisterClass(szAppName, hinst);
	}
	return 0;
}

LRESULT CALLBACK C86CtlMainWnd::wndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static UINT taskbarRestartMsg=0;
	C86CtlMainWnd *pThis = C86CtlMainWnd::getInstance();

	switch(iMsg){
	case WM_CREATE:
		// タスクトレイアイコンの要再登録通知用
		taskbarRestartMsg = ::RegisterWindowMessage(_T("TaskbarCreated"));
		break;

	case WM_DESTROY:
		pThis->stopVis();
		::PostQuitMessage(0);
		break;

	case WM_DEVICECHANGE:
		::PostThreadMessage( ::GetCurrentThreadId(), WM_MYDEVCHANGE, wParam, lParam );
		break;

	case WM_TASKTRAY_EVENT:
		{
			POINT point;
			GetCursorPos(&point);

			if( lParam == WM_RBUTTONDOWN ){
				::SetForegroundWindow(hwnd);

				HMENU hMenu = ::LoadMenu(C86CtlMain::getInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_SYSPOPUP));
				if( !hMenu )
					break;
				HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
				if( !hSubMenu ){
					::DestroyMenu(hMenu);
					break;
				}
				if( pThis->mainVisWnd )
					::CheckMenuItem(hMenu, ID_POPUP_SHOWVIS, MF_BYCOMMAND | MFS_CHECKED );

				TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, point.x, point.y, 0, hwnd, NULL);
				::DestroyMenu(hMenu);
			}
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case ID_POPUP_CONFIG:
			break;
		case ID_POPUP_SHOWVIS:
			if( pThis->mainVisWnd ){
				pThis->stopVis();
			}else{
				pThis->startVis();
			}
			break;
		}

	default:
		if( iMsg == taskbarRestartMsg ){
			// タスクトレイアイコンの再登録
			::Shell_NotifyIcon( NIM_ADD, &pThis->notifyIcon );

		}else{
			return DefWindowProc(hwnd, iMsg, wParam, lParam);
		}
	}
	return 0;
}


int C86CtlMainWnd::startVis()
{
	wm = new CVisManager();
	mainVisWnd = new CVisC86Main();
	wm->add( mainVisWnd );

	mainVisWnd->create(getHWND());

	// 描画スレッド開始
	hVisThread = (HANDLE)_beginthreadex( NULL, 0, &threadVis, wm, 0, &visThreadID );
	if( !hVisThread ){
		stopVis();
	}
	return 0;
}

int C86CtlMainWnd::updateVis()
{
	return 0;
}

int C86CtlMainWnd::stopVis()
{
	// 描画スレッド終了
	if( hVisThread ){
		::PostThreadMessage( visThreadID, WM_THREADEXIT, 0, 0 );
		::WaitForSingleObject( hVisThread, INFINITE );

		hVisThread = NULL;
		visThreadID = 0;
	}
	
	if( mainVisWnd && wm ){
		wm->del( mainVisWnd );
		mainVisWnd->close();

		delete mainVisWnd;
		delete wm;
		mainVisWnd = 0;
		wm = 0;
	}
	return 0;
}


// ---------------------------------------------------------
// 描画処理スレッド
// mm-timerによる60fps生成
unsigned int WINAPI C86CtlMainWnd::threadVis(LPVOID param)
{
	MSG msg;

	ZeroMemory(&msg, sizeof(msg));
	::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

	CVisManager *pwm = reinterpret_cast<CVisManager*>(param);

	DWORD next = ::timeGetTime()*6 + 100;
	while(1){
		// message proc
		if( ::PeekMessage(&msg , NULL , 0 , 0, PM_REMOVE )) {
			if( msg.message == WM_THREADEXIT )
				break;
		}else{
			// fps management
			DWORD now = ::timeGetTime() * 6;
			if(now < next){
				Sleep(1);
				continue;
			}
			next += 100;
			if( next < now ){
				//next = now;
				while(next<now) next += 100;
			}

			//update
			pwm->draw();
		}
	}
	
	return 0;
}

