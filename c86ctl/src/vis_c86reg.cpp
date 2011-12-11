/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include "vis_c86reg.h"
#include "vis_c86sub.h"

#define WINDOW_WIDTH  284
#define WINDOW_HEIGHT 350

bool CVisC86Reg::create(
	LPCTSTR className, LPCTSTR windowName, int left, int top, HWND parent )
{
	if( !CVisWnd::create(
		className, windowName, left, top, WINDOW_WIDTH, WINDOW_HEIGHT,
		WS_EX_TOOLWINDOW, (WS_POPUP | WS_CLIPCHILDREN), parent ) )
		return false;

	//::ShowWindow( getFrameHWND(), SW_SHOWNOACTIVATE );
	return true;
}

void CVisC86Reg::close()
{
	CVisWnd::close();
}

LRESULT CALLBACK CVisC86Reg::wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	switch(msg){
	case WM_CREATE:
		//::SetTimer(hWnd, 0, 50, NULL);
		break;
//	case WM_TIMER:
//		::InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_PAINT:
		OnPaint();
		break;
	default:
		return DefWindowProc(hWnd , msg , wp , lp);
	}
	return 0;
}

void CVisC86Reg::drawRegView( HDC hdc, HDC hSkinDC, HDC hSkinMaskDC, int ltx, int lty,
							  const UCHAR *regval, const UCHAR *regatime )
{
	const int dc = 8;
	CHAR str[64];
	RECT rc;

	int ox = 0;
	int oy = 0;
	int cx=6, cy=8;
	for( int x=0; x<16; x++ ){
		sprintf( str, "+%X", x );
		int sx = ltx+ox+(cx*2+4)*(x+1);
		int sy = lty+oy;
		vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 1, sx, sy, str );
	}
	for( int y=0; y<16; y++ ){
		sprintf( str, "%02X", y*16 );
		int sx = ltx+ox;
		int sy = lty+oy+cy*(y+1);
		vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 1, sx, sy, str );
	}
	
	ox = cx*2+4;
	oy = cy;
	for( int y=0; y<16; y++ ){
		for( int x=0; x<16; x++ ){
			int a = y*16+x;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
			int v = regval[a];
			int c = regatime[a];

			sprintf( str,"%02X", v );
			int sx = ltx+ox+(cx*2+4)*x;
			int sy = lty+oy+cy*y;
			
			rc.left = sx;		rc.right = sx+cx*2;
			rc.top = sy;		rc.bottom = sy+cy;
			vis_fill_rect( hdc, RGB(c,c,0), &rc );
			vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 1, sx, sy, str );
		}
	}
}

void CVisC86Reg::OnPaint()
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


	if( pOPNA ){
		int sx=5, sy=5, cx=6, cy=8;
		vis_draw_str( hMemDC, hSkinDC, hSkinMaskDC, 1, sx, sy, "REGISTER BANK0 ------------------------------" );
		drawRegView( hMemDC, hSkinDC, hSkinMaskDC, sx, sy+cy* 1, pOPNA->reg[0], pOPNA->regATime[0] );
		vis_draw_str( hMemDC, hSkinDC, hSkinMaskDC, 1, sx, sy+cy*20, "REGISTER BANK1 ------------------------------" );
		drawRegView( hMemDC, hSkinDC, hSkinMaskDC, sx, sy+cy*21, pOPNA->reg[1], pOPNA->regATime[1] );
	}

	::BitBlt(hDC, 0, 0, wx, wy, hMemDC, 0, 0, SRCCOPY);

	::SelectObject(hMemDC,hOldBMP);
	::DeleteObject(hMemBMP);
	::DeleteDC(hMemDC);
	::EndPaint(hWnd, &ps);
}
