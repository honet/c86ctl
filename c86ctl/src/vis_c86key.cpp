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

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

bool CVisC86Key::create( HWND parent )
{
	int left = INT_MIN;
	int top = INT_MIN;
	
	if( !CVisWnd::create(
		left, top, windowWidth, windowHeight,
		WS_EX_TOOLWINDOW, (WS_POPUP | WS_CLIPCHILDREN), parent ) )
		return false;

	::ShowWindow( hWnd, SW_SHOWNOACTIVATE );
	
	return true;
}

void CVisC86Key::close()
{
	CVisWnd::close();
}



void CVisC86OPNAKey::onPaintClient(void)
{
	visFillRect( clientCanvas, 0, 0, clientCanvas->getWidth(), clientCanvas->getHeight(), ARGB(255,0,0,0) );
	
	if( pOPNA ){
		int sx=5, sy=5, cx=6, cy=8;
		int tr=0;
		for( int i=0; i<3; i++ ){
			drawFMTrackView( clientCanvas, sx, sy, tr, i );
			sy+=35; tr++;
		}
		for( int i=0; i<3; i++ ){
			drawFM3EXTrackView( clientCanvas, sx, sy, tr, 2, i );
			sy+=35; tr++;
		}
		for( int i=3; i<6; i++ ){
			drawFMTrackView( clientCanvas, sx, sy, tr, i );
			sy+=35; tr++;
		}
		for( int i=0; i<3; i++ ){
			drawSSGTrackView( clientCanvas, sx, sy, tr, i );
			sy+=35; tr++;
		}
		drawADPCMTrackView( clientCanvas, sx, sy, tr );
		sy+=35; tr++;
		drawRhythmTrackView( clientCanvas, sx, sy, tr );
	}
}



void CVisC86OPNAKey::drawFMTrackView( IVisBitmap *canvas, int ltx, int lty,
								  int trNo, int fmNo )
{
	int sy = 0;
	char str[64], tmp[64];
	int cx=6, cy=8;
	CVisC86Skin *skin = &gVisSkin;

	skin->drawStr( canvas, 0, ltx+5, lty+sy+5, "TRACK" );
	sprintf( str, "%02d", trNo+1 );
	skin->drawNumStr1( canvas, ltx+5+cx*6, lty+sy+2, str );
	sprintf( str, "FM-CH%d", fmNo+1 );
	skin->drawStr( canvas, 0, ltx+5+cx*10, lty+sy+5, str );

	strcpy(str, "SLOT:");
	for( int i=0; i<4; i++ ){
		int sw = pOPNA->fm[fmNo].slot[i].isOn();
		sprintf( tmp, "%d", i );
		if( sw ) strncat( str, tmp, 64 );
		else  strncat( str, "_", 64 );
	}
	skin->drawStr( canvas, 0, ltx+5+cx*24, lty+sy+5, str );

	int fblock = pOPNA->fm[fmNo].getFBlock();
	int fnum = pOPNA->fm[fmNo].getFNum();
	sprintf( str, "BLK:%d  FNUM:%04d", fblock, fnum );
	skin->drawStr( canvas, 0, ltx+5+cx*35, lty+sy+5, str );
	skin->drawKeyboard( canvas, ltx, lty+sy+15 );

	if( pOPNA->fm[fmNo].isKeyOn() && pOPNA->fm[fmNo].getMixLevel() ){
		int oct, note;
		pOPNA->fm[fmNo].getNote( oct, note );
		skin->drawHilightKey( canvas, ltx, lty+sy+15, oct, note );
	}
}

void CVisC86OPNAKey::drawFM3EXTrackView( IVisBitmap *canvas, int ltx, int lty,
									 int trNo, int fmNo, int exNo )
{
	int sy = 0;
	int cx=6, cy=8;
	char str[64];
	CVisC86Skin *skin = &gVisSkin;
	
	skin->drawStr( canvas, 0, ltx+5, lty+sy+5, "TRACK" );
	sprintf( str, "%02d", trNo+1 );
	skin->drawNumStr1( canvas, ltx+5+34, lty+sy+2, str );
	sprintf( str, "FM-CH%dEX%d", fmNo+1, exNo );
	skin->drawStr( canvas, 0, ltx+5+60, lty+sy+5, str );
	skin->drawKeyboard( canvas, ltx, lty+sy+15 );

	if( pOPNA->fm[fmNo].getExMode() ){
		int fblock = pOPNA->fm[fmNo].getFBlockEx(exNo);
		int fnum = pOPNA->fm[fmNo].getFNumEx(exNo);
		sprintf( str, "BLK:%d  FNUM:%04d", fblock, fnum );
		skin->drawStr( canvas, 0, ltx+5+cx*35, lty+sy+5, str );

		skin->drawKeyboard( canvas, ltx, lty+sy+15 );
		if( pOPNA->fm[fmNo].slot[exNo].isOn() && pOPNA->fm[fmNo].getMixLevel() ){
			int oct, note;
			pOPNA->fm[fmNo].getNoteEx( exNo, oct, note );
			skin->drawHilightKey( canvas, ltx, lty+sy+15, oct, note );
		}
	}else{
		skin->drawDarkKeyboard( canvas, ltx, lty+sy+15 );
	}
}

void CVisC86OPNAKey::drawSSGTrackView( IVisBitmap *canvas, int ltx, int lty, int trNo, int ssgNo )
{
	int sy = 0;
	int cx=6, cy=8;
	char str[64];
	CVisC86Skin *skin = &gVisSkin;
	
	skin->drawStr( canvas, 0, ltx+5, lty+sy+5, "TRACK" );
	sprintf( str, "%02d", trNo+1 );
	skin->drawNumStr1( canvas, ltx+5+34, lty+sy+2, str );
	sprintf( str, "SSG-CH%d", ssgNo+1 );
	skin->drawStr( canvas, 0, ltx+5+cx*10, lty+sy+5, str );

	int tune = pOPNA->ssg.ch[ssgNo].getTune();
	sprintf( str, "TUNE:%04d", tune );
	skin->drawStr( canvas, 0, ltx+5+cx*42, lty+sy+5, str );

	skin->drawKeyboard( canvas, ltx, lty+sy+15 );	

	if( pOPNA->ssg.ch[ssgNo].isOn() && pOPNA->ssg.ch[ssgNo].getLevel()!=0 ){
		int oct, note;
		pOPNA->ssg.ch[ssgNo].getNote( oct, note );
		skin->drawHilightKey( canvas, ltx, lty+sy+15, oct, note );
	}
}

void CVisC86OPNAKey::drawADPCMTrackView( IVisBitmap *canvas, int ltx, int lty, int trNo )
{
	int sy = 0;
	char str[64];
	CVisC86Skin *skin = &gVisSkin;
	
	skin->drawStr( canvas, 0, ltx+5, lty+sy+5, "TRACK" );
	sprintf( str, "%02d", trNo+1 );
	skin->drawNumStr1( canvas, ltx+5+34, lty+sy+2, str );
	sprintf( str, "ADPCM" );
	skin->drawStr( canvas, 0, ltx+5+60, lty+sy+5, str );
	skin->drawKeyboard( canvas, ltx, lty+sy+15 );
}

void CVisC86OPNAKey::drawRhythmTrackView( IVisBitmap *canvas, int ltx, int lty, int trNo )
{
	int sy = 0;
	int cx=6, cy=8;
	char str[64];
	CVisC86Skin *skin = &gVisSkin;
	
	skin->drawStr( canvas, 0, ltx+5, lty+sy+5, "TRACK" );
	sprintf( str, "%02d", trNo+1 );
	skin->drawNumStr1( canvas, ltx+5+34, lty+sy+2, str );
	sprintf( str, "RHYTHM" );
	skin->drawStr( canvas, 0, ltx+5+60, lty+sy+5, str );

	visDrawLine( canvas, ltx    , lty+sy+5+10, ltx+320, lty+sy+5+10, col_mid );
	visDrawLine( canvas, ltx+320, lty+sy+5+10, ltx+320, lty+sy+5+30, col_mid );
	visDrawLine( canvas, ltx+320, lty+sy+5+30, ltx    , lty+sy+5+30, col_mid );
	visDrawLine( canvas, ltx    , lty+sy+5+30, ltx    , lty+sy+5+10, col_mid );
	
	if( pOPNA->rhythm.rim.isOn() )
		skin->drawStr( canvas, 0, ltx+5+cx*12, lty+sy+5+18, "RIM" );
	if( pOPNA->rhythm.tom.isOn() )
		skin->drawStr( canvas, 0, ltx+5+cx*17, lty+sy+5+18, "TOM" );
	if( pOPNA->rhythm.hh.isOn() )
		skin->drawStr( canvas, 0, ltx+5+cx*22, lty+sy+5+18, "HH" );
	if( pOPNA->rhythm.top.isOn() )
		skin->drawStr( canvas, 0, ltx+5+cx*27, lty+sy+5+18, "TOP" );
	if( pOPNA->rhythm.sd.isOn() )
		skin->drawStr( canvas, 0, ltx+5+cx*32, lty+sy+5+18, "SD" );
	if( pOPNA->rhythm.bd.isOn() )
		skin->drawStr( canvas, 0, ltx+5+cx*37, lty+sy+5+18, "BD");

}

// --------------------------------------------------------
CVisC86KeyPtr visC86KeyViewFactory(Chip *pchip, int id)
{
	if( typeid(*pchip) == typeid(COPNA) ){
		return CVisC86KeyPtr( new CVisC86OPNAKey(dynamic_cast<COPNA*>(pchip), id ) );
//	}else if( typeid(*pchip) == typeid(COPN3L) ){
//		return CVisC86RegPtr( new CVisC86OPN3LKey(dynamic_cast<COPN3L*>(pchip), id) );
//	}else if( typeid(*pchip) == typeid(COPM) ){
//		return CVisC86RegPtr( new CVisC86OPMKey(dynamic_cast<COPM*>(pchip), id) );
	}
	return 0;
}
