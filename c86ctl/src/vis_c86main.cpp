/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include "module.h"
#include "resource.h"
#include "vis_c86sub.h"
#include "vis_c86main.h"
#include "config.h"
#include "version.h"


#define WINDOW_WIDTH  400
#define WINDOW_HEIGHT 180

const struct {
	int left;
	int top;
	int btnid;
} btnlist[] = {
	{  10, 90, BUTTON_KEY },
	{ 110, 90, BUTTON_REG },
	{  10,115, BUTTON_FM1 },
	{  60,115, BUTTON_FM2 },
	{ 110,115, BUTTON_FM3 },
	{ 160,115, BUTTON_FM4 },
	{ 210,115, BUTTON_FM5 },
	{ 260,115, BUTTON_FM6 }
};


bool CVisC86Main::create()
{
	int left = gConfig.getInt( INISC_MAIN, INIKEY_WNDLEFT, INT_MIN );
	int top  = gConfig.getInt( INISC_MAIN, INIKEY_WNDTOP,  INT_MIN );
	
	if ( CVisWnd::create( TEXT("C86CTL"), TEXT("C86 CONTROL"), left, top, WINDOW_WIDTH, WINDOW_HEIGHT, 
		0, WS_POPUP | WS_CLIPCHILDREN ) ){
		::ShowWindow( CVisWnd::getFrameHWND(), SW_SHOWNOACTIVATE );
		return true;
	}
	return false;
}
void CVisC86Main::close()
{
	::SendMessage( hWnd, WM_CLOSE, 0, 0 );
}

void CVisC86Main::attach( COPNA *p )
{
	chip = p;
	regWnd.attach( chip );
	keyWnd.attach( chip );
	for( int i=0; i<6; i++ )
		fmWnd[i].attach(&chip->fm[i]);
};

COPNA* CVisC86Main::detach( void ){
	COPNA* oldchip = chip;
	regWnd.detach();
	keyWnd.detach();
	for( int i=0; i<6; i++ )
		fmWnd[i].detach();
	chip = 0;
	return oldchip;
};

void CVisC86Main::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(hWnd, &ps);
	
	RECT rc;
	::GetClientRect(hWnd, &rc);
	INT wx = rc.right-rc.left;
	INT wy = rc.bottom-rc.top;
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hMemBMP = ::CreateCompatibleBitmap(hDC, wx, wy);
	HBITMAP hOldBMP = (HBITMAP)::SelectObject(hMemDC, hMemBMP);
	vis_fill_rect( hMemDC, 0, &rc );

	HPEN hpen, holdpen;
	hpen = ::CreatePen(PS_SOLID, 1, col_mid);
	if( hpen ){
		holdpen = (HPEN)::SelectObject(hMemDC, hpen);
		if( holdpen ){
			::MoveToEx(hMemDC, 2, 28, NULL );
			::LineTo(hMemDC, rc.right-2, 28 );
			::SelectObject(hMemDC, holdpen);
		}
		::DeleteObject(hpen);
	}
	vis_draw_logo( hMemDC, hSkinDC, hSkinMaskDC, 2, 2 );
	vis_draw_tickcircle( hMemDC, hSkinDC, hSkinMaskDC, 10, 35, tick&0x7 );
	vis_draw_str( hMemDC, hSkinDC, hSkinMaskDC, 0, 130, 8, "FOR GIMIC & C86USB CONTROL" );
	vis_draw_str( hMemDC, hSkinDC, hSkinMaskDC, 0, 130, 18, "OPNA STATUS DISPLAY " VERSION_MESSAGE );

	vis_draw_str( hMemDC, hSkinDC, hSkinMaskDC, 0,   5, 78, "WINDOWS -------------------------------------------------------" );
	CVisWnd *wnd[] = { &keyWnd, &regWnd, 
		&fmWnd[0], &fmWnd[1], &fmWnd[2],
		&fmWnd[3], &fmWnd[4], &fmWnd[5] };
	for( int i=0; i<8; i++ ){
		int sw = ::IsWindowVisible( wnd[i]->getFrameHWND() ) ? 1 : 0;
		vis_draw_button( hMemDC, hSkinDC, hSkinMaskDC,  btnlist[i].left, btnlist[i].top, btnlist[i].btnid, sw );
	}

	::BitBlt(hDC, 0, 0, wx, wy, hMemDC, 0, 0, SRCCOPY);

	::SelectObject(hMemDC,hOldBMP);
	::DeleteObject(hMemBMP);
	::DeleteDC(hMemDC);
	::EndPaint(hWnd, &ps);
}

void CVisC86Main::OnLButtonUp( UINT flags, int mx, int my )
{
	CVisWnd *wnd[] = { &keyWnd, &regWnd, 
		&fmWnd[0], &fmWnd[1], &fmWnd[2],
		&fmWnd[3], &fmWnd[4], &fmWnd[5] };
	for( int i=0; i<8; i++ ){
		if( vis_hittest_button( btnlist[i].left, btnlist[i].top, btnlist[i].btnid, mx, my ) ){
			if( ::IsWindowVisible( wnd[i]->getFrameHWND() ) ){
				::ShowWindow( wnd[i]->getFrameHWND(), SW_HIDE );
			}else{
				::ShowWindow( wnd[i]->getFrameHWND(), SW_SHOW );
			}
		}
	}
	::InvalidateRect( getFrameHWND(), NULL, FALSE );
}

LRESULT CALLBACK CVisC86Main::wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	switch (msg) {
	case WM_CREATE:
		{
			const int offsetx=20, offsety=20;
			TCHAR cname[128], wname[128];
			int left, top, enable;
			RECT rc;

//			regWnd.attach( chip );
//			keyWnd.attach( chip );
//			for( int i=0; i<6; i++ )
//				fmWnd[i].attach(&chip->fm[i]);
	
			::GetWindowRect(hWnd, &rc);
			left = gConfig.getInt( INISC_KEY, INIKEY_WNDLEFT, rc.left+offsetx );
			top  = gConfig.getInt( INISC_KEY, INIKEY_WNDTOP, rc.top+offsety );
			enable = gConfig.getInt( INISC_KEY, INIKEY_WNDVISIBLE, TRUE );
			keyWnd.create( TEXT("C86KEY"), TEXT("C86 KEYBOARD VIEW"), left, top, hWnd );
			if( enable )
				::ShowWindow( keyWnd.getFrameHWND(), SW_SHOWNOACTIVATE );
			
			left = gConfig.getInt( INISC_REG, INIKEY_WNDLEFT, left+offsetx );
			top  = gConfig.getInt( INISC_REG, INIKEY_WNDTOP,  top+offsety  );
			enable = gConfig.getInt( INISC_REG, INIKEY_WNDVISIBLE, TRUE );
			regWnd.create( TEXT("C86REG"), TEXT("C86 REGISTER VIEW"), left, top, hWnd );
			if( enable )
				::ShowWindow( regWnd.getFrameHWND(), SW_SHOWNOACTIVATE );

			for( int i=0; i<6; i++ ){
				_stprintf( cname, INISC_FMx, i+1 );
				left = gConfig.getInt( cname, INIKEY_WNDLEFT, left+offsetx );
				top  = gConfig.getInt( cname, INIKEY_WNDTOP,  top+offsety  );
				enable = gConfig.getInt( cname, INIKEY_WNDVISIBLE, TRUE );
				_stprintf( cname, TEXT("C86FM%d"), i+1 );
				_stprintf( wname, TEXT("C86 FM PARAMETER VIEW CH%d"), i+1 );
				fmWnd[i].create( cname, wname, left, top, hWnd );
				if(i==2) fmWnd[i].setExMode(true);
				if( enable )
					::ShowWindow( fmWnd[i].getFrameHWND(), SW_SHOWNOACTIVATE );
			}
			::SetTimer( hWnd, 0, 50, NULL );
		}
		break;
		
	case WM_CLOSE:
		{
			detach();
			
			RECT rc;
			TCHAR cname[128];
			::GetWindowRect( getFrameHWND(), &rc );
			gConfig.writeInt( INISC_MAIN, INIKEY_WNDLEFT, rc.left );
			gConfig.writeInt( INISC_MAIN, INIKEY_WNDTOP, rc.top );
			gConfig.writeInt( INISC_MAIN, INIKEY_WNDVISIBLE, ::IsWindowVisible(getFrameHWND())?1:0 );

			::GetWindowRect( keyWnd.getFrameHWND(), &rc );
			gConfig.writeInt( INISC_KEY, INIKEY_WNDLEFT, rc.left );
			gConfig.writeInt( INISC_KEY, INIKEY_WNDTOP, rc.top );
			gConfig.writeInt( INISC_KEY, INIKEY_WNDVISIBLE, ::IsWindowVisible(keyWnd.getFrameHWND())?1:0 );

			::GetWindowRect( regWnd.getFrameHWND(), &rc );
			gConfig.writeInt( INISC_REG, INIKEY_WNDLEFT, rc.left );
			gConfig.writeInt( INISC_REG, INIKEY_WNDTOP, rc.top );
			gConfig.writeInt( INISC_REG, INIKEY_WNDVISIBLE, ::IsWindowVisible(regWnd.getFrameHWND())?1:0 );

			for( int i=0; i<6; i++ ){
				::GetWindowRect( fmWnd[i].getFrameHWND(), &rc );
				_stprintf( cname, INISC_FMx, i+1 );
				gConfig.writeInt( cname, INIKEY_WNDLEFT, rc.left );
				gConfig.writeInt( cname, INIKEY_WNDTOP,  rc.top  );
				gConfig.writeInt( cname, INIKEY_WNDVISIBLE, ::IsWindowVisible(fmWnd[i].getFrameHWND())?1:0 );
			}
		}
		return DefWindowProc(hWnd , msg , wp , lp);

	case WM_TIMER:
		tick++;
		if(chip) chip->update();
		::InvalidateRect( hWnd, NULL, FALSE );
		regWnd.update();
		keyWnd.update();
		for( int i=0; i<6; i++ )
			fmWnd[i].update();
		break;
	case WM_PAINT:
		OnPaint();
		break;

	case WM_LBUTTONUP:
		OnLButtonUp( wp, LOWORD(lp), HIWORD(lp) );

	default:
		return DefWindowProc(hWnd , msg , wp , lp);
	}
	return 0;
}

