/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <stdio.h>
#include "vis_c86key.h"
#include "vis_c86sub.h"

#define WINDOW_WIDTH  334
#define WINDOW_HEIGHT 530


bool CVisC86Key::create(
	LPCTSTR className, LPCTSTR windowName, int left, int top, HWND parent )
{
	if( !CVisWnd::create(
		className, windowName, left, top, WINDOW_WIDTH, WINDOW_HEIGHT,
		WS_EX_TOOLWINDOW, (WS_POPUP | WS_CLIPCHILDREN), parent ) )
		return false;

	//::ShowWindow( getFrameHWND(), SW_SHOWNOACTIVATE );
	return true;
}

void CVisC86Key::close()
{
}

LRESULT CALLBACK CVisC86Key::wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	switch(msg){
	case WM_PAINT:
		OnPaint();
		break;
	default:
		return DefWindowProc(hWnd , msg , wp , lp);
	}
	return 0;
}


void CVisC86Key::OnPaint()
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

	// keyboard
	if( pOPNA ){
		int sx=5, sy=5, cx=6, cy=8;
		int tr=0;
		for( int i=0; i<3; i++ ){
			drawFMTrackView( hMemDC, sx, sy, tr, i );
			sy+=35; tr++;
		}
		for( int i=0; i<3; i++ ){
			drawFM3EXTrackView( hMemDC, sx, sy, tr, 2, i );
			sy+=35; tr++;
		}
		for( int i=3; i<6; i++ ){
			drawFMTrackView( hMemDC, sx, sy, tr, i );
			sy+=35; tr++;
		}
		for( int i=0; i<3; i++ ){
			drawSSGTrackView( hMemDC, sx, sy, tr, i );
			sy+=35; tr++;
		}
		drawADPCMTrackView( hMemDC, sx, sy, tr );
		sy+=35; tr++;
		drawRhythmTrackView( hMemDC, sx, sy, tr );
	}

	::BitBlt(hDC, 0, 0, wx, wy, hMemDC, 0, 0, SRCCOPY);

	::SelectObject(hMemDC,hOldBMP);
	::DeleteObject(hMemBMP);
	::DeleteDC(hMemDC);
	::EndPaint(hWnd, &ps);
}



void CVisC86Key::drawFMTrackView( HDC hdc, int ltx, int lty,
								  int trNo, int fmNo )
{
	int sy = 0;
	char str[64], tmp[64];
	int cx=6, cy=8;
	
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5, lty+sy+5, "TRACK" );
	sprintf( str, "%02d", trNo+1 );
	vis_draw_numstr1( hdc, hSkinDC, hSkinMaskDC, ltx+5+cx*6, lty+sy+2, str );
	sprintf( str, "FM-CH%d", fmNo+1 );
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*10, lty+sy+5, str );

	strcpy(str, "SLOT:");
	for( int i=0; i<4; i++ ){
		int sw = pOPNA->fm[fmNo].slot[i].isOn();
		sprintf( tmp, "%d", i );
		if( sw ) strncat( str, tmp, 64 );
		else  strncat( str, "_", 64 );
	}
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*24, lty+sy+5, str );

	int fblock = pOPNA->fm[fmNo].getFBlock();
	int fnum = pOPNA->fm[fmNo].getFNum();
	sprintf( str, "BLK:%d  FNUM:%04d", fblock, fnum );
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*35, lty+sy+5, str );
	vis_draw_keyboard( hdc, hSkinDC, hSkinMaskDC, ltx, lty+sy+15 );

	if( pOPNA->fm[fmNo].isKeyOn() && pOPNA->fm[fmNo].getMixLevel() ){
		int oct, note;
		pOPNA->fm[fmNo].getNote( oct, note );
		vis_draw_hilight_key( hdc, hSkinDC, hSkinMaskDC, ltx, lty+sy+15, oct, note );
	}
}

void CVisC86Key::drawFM3EXTrackView( HDC hdc, int ltx, int lty,
									 int trNo, int fmNo, int exNo )
{
	int sy = 0;
	int cx=6, cy=8;
	char str[64];
	
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5, lty+sy+5, "TRACK" );
	sprintf( str, "%02d", trNo+1 );
	vis_draw_numstr1( hdc, hSkinDC, hSkinMaskDC, ltx+5+34, lty+sy+2, str );
	sprintf( str, "FM-CH%dEX%d", fmNo+1, exNo );
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+60, lty+sy+5, str );
	vis_draw_keyboard( hdc, hSkinDC, hSkinMaskDC, ltx, lty+sy+15 );

	if( pOPNA->fm[fmNo].getExMode() ){
		int fblock = pOPNA->fm[fmNo].getFBlockEx(exNo);
		int fnum = pOPNA->fm[fmNo].getFNumEx(exNo);
		sprintf( str, "BLK:%d  FNUM:%04d", fblock, fnum );
		vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*35, lty+sy+5, str );

		vis_draw_keyboard( hdc, hSkinDC, hSkinMaskDC, ltx, lty+sy+15 );
		if( pOPNA->fm[fmNo].slot[exNo].isOn() && pOPNA->fm[fmNo].getMixLevel() ){
			int oct, note;
			pOPNA->fm[fmNo].getNoteEx( exNo, oct, note );
			vis_draw_hilight_key( hdc, hSkinDC, hSkinMaskDC, ltx, lty+sy+15, oct, note );
		}
	}else{
		vis_draw_dark_keyboard( hdc, hSkinDC, hSkinMaskDC, ltx, lty+sy+15 );
	}
}

void CVisC86Key::drawSSGTrackView( HDC hdc, int ltx, int lty, int trNo, int ssgNo )
{
	int sy = 0;
	int cx=6, cy=8;
	char str[64];
	
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5, lty+sy+5, "TRACK" );
	sprintf( str, "%02d", trNo+1 );
	vis_draw_numstr1( hdc, hSkinDC, hSkinMaskDC, ltx+5+34, lty+sy+2, str );
	sprintf( str, "SSG-CH%d", ssgNo+1 );
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*10, lty+sy+5, str );

	int tune = pOPNA->ssg.ch[ssgNo].getTune();
	sprintf( str, "TUNE:%04d", tune );
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*42, lty+sy+5, str );

	vis_draw_keyboard( hdc, hSkinDC, hSkinMaskDC, ltx, lty+sy+15 );	

	if( pOPNA->ssg.ch[ssgNo].isOn() && pOPNA->ssg.ch[ssgNo].getLevel()!=0 ){
		int oct, note;
		pOPNA->ssg.ch[ssgNo].getNote( oct, note );
		vis_draw_hilight_key( hdc, hSkinDC, hSkinMaskDC, ltx, lty+sy+15, oct, note );
	}
}

void CVisC86Key::drawADPCMTrackView( HDC hdc, int ltx, int lty, int trNo )
{
	int sy = 0;
	char str[64];
	
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5, lty+sy+5, "TRACK" );
	sprintf( str, "%02d", trNo+1 );
	vis_draw_numstr1( hdc, hSkinDC, hSkinMaskDC, ltx+5+34, lty+sy+2, str );
	sprintf( str, "ADPCM" );
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+60, lty+sy+5, str );
	vis_draw_keyboard( hdc, hSkinDC, hSkinMaskDC, ltx, lty+sy+15 );
}

void CVisC86Key::drawRhythmTrackView( HDC hdc, int ltx, int lty, int trNo )
{
	int sy = 0;
	int cx=6, cy=8;
	char str[64];
	
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5, lty+sy+5, "TRACK" );
	sprintf( str, "%02d", trNo+1 );
	vis_draw_numstr1( hdc, hSkinDC, hSkinMaskDC, ltx+5+34, lty+sy+2, str );
	sprintf( str, "RHYTHM" );
	vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+60, lty+sy+5, str );

	HPEN mypen = (HPEN)::CreatePen( PS_SOLID, 1, col_mid );
	HPEN oldpen = (HPEN)::SelectObject( hdc, mypen );
	::MoveToEx( hdc, ltx, lty+sy+5+10, NULL );
	::LineTo( hdc, ltx+320, lty+sy+5+10 );
	::LineTo( hdc, ltx+320, lty+sy+5+30 );
	::LineTo( hdc, ltx, lty+sy+5+30 );
	::LineTo( hdc, ltx, lty+sy+5+10 );
	::SelectObject( hdc, oldpen );
	::DeleteObject( mypen );
	
	if( pOPNA->rhythm.rim.isOn() )
		vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*12, lty+sy+5+18, "RIM" );
	if( pOPNA->rhythm.tom.isOn() )
		vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*17, lty+sy+5+18, "TOM" );
	if( pOPNA->rhythm.hh.isOn() )
		vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*22, lty+sy+5+18, "HH" );
	if( pOPNA->rhythm.top.isOn() )
		vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*27, lty+sy+5+18, "TOP" );
	if( pOPNA->rhythm.sd.isOn() )
		vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*32, lty+sy+5+18, "SD" );
	if( pOPNA->rhythm.bd.isOn() )
		vis_draw_str( hdc, hSkinDC, hSkinMaskDC, 0, ltx+5+cx*37, lty+sy+5+18, "BD");
}