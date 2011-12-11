/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#include "stdafx.h"
#include "opna.h"
#include "vis_c86fm.h"
#include "vis_c86sub.h"

#define WINDOW_WIDTH  (316+4)
#define WINDOW_HEIGHT (75*5+4+15)

bool CVisC86Fm::create(
	LPCTSTR className, LPCTSTR windowName, int left, int top, HWND parent )
{
	if( !CVisWnd::create(
		className, windowName, left, top, WINDOW_WIDTH, WINDOW_HEIGHT,
		WS_EX_TOOLWINDOW, (WS_POPUP | WS_CLIPCHILDREN), parent ) )
		return false;

	//::ShowWindow( getFrameHWND(), SW_SHOWNOACTIVATE );
	return true;
}

void CVisC86Fm::close()
{
	CVisWnd::close();
}

LRESULT CALLBACK CVisC86Fm::wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	switch(msg){
	case WM_CREATE:
//		::SetTimer(hWnd, 0, 50, NULL);
		break;
//	case WM_TIMER:
//		::InvalidateRect(hWnd, NULL, FALSE);
//		break;
	case WM_PAINT:
		OnPaint();
		break;
	default:
		return DefWindowProc(hWnd , msg , wp , lp);
	}
	return 0;
}

void CVisC86Fm::OnPaint()
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

	// slot view
	if( pFM ){
		int sx=5, sy=5, cx=6, cy=8;
		vis_draw_fm_view( hMemDC, hSkinDC, hSkinMaskDC, 0, 0, pFM );
		for( int i=0; i<4; i++ )
			vis_draw_fmslot_view( hMemDC, hSkinDC, hSkinMaskDC, 0, 75*(1+i), &pFM->slot[i], i );
	}
	
	::BitBlt(hDC, 0, 0, wx, wy, hMemDC, 0, 0, SRCCOPY);

	::SelectObject(hMemDC,hOldBMP);
	::DeleteObject(hMemBMP);
	::DeleteDC(hMemDC);
	::EndPaint(hWnd, &ps);
}


