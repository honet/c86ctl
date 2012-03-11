/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
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


using namespace c86ctl;
using namespace c86ctl::vis;

const char *noteStr[12] = { "C ", "C+", "D ", "D+", "E ", "F ", "F+", "G ", "G+", "A ", "A+", "B " };

void CVisC86Key::drawFMTrackView( IVisBitmap *canvas, int ltx, int lty,
								  int trNo, int fmNo, bool isMute, COPNFmCh *pFMCh )
{
	int sy = 0;
	char str[64], tmp[64];
	int cx=6, cy=8;
	CVisC86Skin *skin = &gVisSkin;

	sprintf( str, "%02d", trNo+1 );
	skin->drawNumStr1( canvas, ltx+5, lty+sy+2, str );
	sprintf( str, "FM-CH%d", fmNo+1 );
	skin->drawStr( canvas, 0, ltx+5+cx*10, lty+sy+5, str );

	if( !isMute ){
		strcpy(str, "SLOT:");
		for( int i=0; i<4; i++ ){
			int sw = pFMCh->slot[i]->isOn();
			sprintf( tmp, "%d", i );
			if( sw ) strncat( str, tmp, 64 );
			else  strncat( str, "_", 64 );
		}
		skin->drawStr( canvas, 0, ltx+5+cx*24, lty+sy+5, str );

		if( !pFMCh->getExMode() ){
			int fblock = pFMCh->getFBlock();
			int fnum = pFMCh->getFNum();
			//sprintf( str, "BLK:%d  FNUM:%04d", fblock, fnum );
			sprintf( str, "B/F:%d+%04d  NOTE:", fblock, fnum );
			skin->drawStr( canvas, 0, ltx+5+cx*35, lty+sy+5, str );
			skin->drawKeyboard( canvas, ltx, lty+sy+15 );

			if( pFMCh->isKeyOn() && pFMCh->getMixLevel() != 127 ){
				int oct, note;
				pFMCh->getNote( oct, note );
				skin->drawHilightKey( canvas, ltx, lty+sy+15, oct, note );
				sprintf( str, "O%d%s", oct, noteStr[note] );
				skin->drawStr( canvas, 0, ltx+5+cx*52, lty+sy+5, str );
			}
			skin->drawHBar( canvas, 290, lty+sy+15, pFMCh->getKeyOnLevel(), 0 );
		}else{
			int fblock = pFMCh->getFBlockEx(0);
			int fnum = pFMCh->getFNumEx(0);
			//sprintf( str, "BLK:%d  FNUM:%04d", fblock, fnum );
			sprintf( str, "B/F:%d+%04d  NOTE:", fblock, fnum );
			skin->drawStr( canvas, 0, ltx+5+cx*35, lty+sy+5, str );
			skin->drawKeyboard( canvas, ltx, lty+sy+15 );

			if( pFMCh->slot[0]->isOn() && pFMCh->slot[0]->getTotalLevel() != 127 ){
				int oct, note;
				pFMCh->getNoteEx( 0, oct, note );
				skin->drawHilightKey( canvas, ltx, lty+sy+15, oct, note );
				sprintf( str, "O%d%s", oct, noteStr[note] );
				skin->drawStr( canvas, 0, ltx+5+cx*52, lty+sy+5, str );
			}
			skin->drawHBar( canvas, 290, lty+sy+15, pFMCh->getKeyOnLevelEx(0), 0 );
		}
	}else{
		skin->drawDarkKeyboard( canvas, ltx, lty+sy+15 );
		skin->drawHBar( canvas, 290, lty+sy+15, 0, 0 );
	}
}

void CVisC86Key::drawFM3EXTrackView( IVisBitmap *canvas, int ltx, int lty,
										 int trNo, bool isMute, COPNFmCh *pFMCh )
{
	int sy = 0;
	int cx=6, cy=8;
	char str[64];
	CVisC86Skin *skin = &gVisSkin;

	drawFMTrackView( canvas, ltx, lty, trNo, 2, isMute, pFMCh );
	lty+=35; trNo++;

	for( int exNo=1; exNo<4; exNo++, trNo++, lty+=35 ){
		sprintf( str, "%02d", trNo+1 );
		skin->drawNumStr1( canvas, ltx+5, lty+sy+2, str );
		sprintf( str, "FM-CH%dEX%d", 3, exNo );
		skin->drawStr( canvas, 0, ltx+5+60, lty+sy+5, str );
		skin->drawKeyboard( canvas, ltx, lty+sy+15 );

		if( pFMCh->getExMode() && !isMute ){
			int fblock = pFMCh->getFBlockEx(exNo);
			int fnum = pFMCh->getFNumEx(exNo);
			//sprintf( str, "BLK:%d  FNUM:%04d", fblock, fnum );
			sprintf( str, "B/F:%d+%04d  NOTE:", fblock, fnum );
			skin->drawStr( canvas, 0, ltx+5+cx*35, lty+sy+5, str );

			skin->drawKeyboard( canvas, ltx, lty+sy+15 );
			if( pFMCh->slot[exNo]->isOn() && pFMCh->slot[exNo]->getTotalLevel() != 127 ){
				int oct, note;
				pFMCh->getNoteEx( exNo, oct, note );
				skin->drawHilightKey( canvas, ltx, lty+sy+15, oct, note );
				sprintf( str, "O%d%s", oct, noteStr[note] );
				skin->drawStr( canvas, 0, ltx+5+cx*52, lty+sy+5, str );
			}
			skin->drawHBar( canvas, 290, lty+sy+15, pFMCh->getKeyOnLevelEx(exNo), 0 );
		}else{
			skin->drawDarkKeyboard( canvas, ltx, lty+sy+15 );
			skin->drawHBar( canvas, 290, lty+sy+15, 0, 0 );
		}
	}
}

void CVisC86Key::drawSSGTrackView( IVisBitmap *canvas, int ltx, int lty,
									int trNo, int ssgNo, bool isMute, COPNSsg *pSsg )
{
	int sy = 0;
	int cx=6, cy=8;
	char str[64];
	CVisC86Skin *skin = &gVisSkin;
	
	sprintf( str, "%02d", trNo+1 );
	skin->drawNumStr1( canvas, ltx+5, lty+sy+2, str );
	sprintf( str, "SSG-CH%d", ssgNo+1 );
	skin->drawStr( canvas, 0, ltx+5+cx*10, lty+sy+5, str );

	int period = pSsg->getNoisePeriod();
	COPNSsgCh *pSsgCh = pSsg->ch[ssgNo];
	
	if( !isMute ){
		skin->drawKeyboard( canvas, ltx, lty+sy+15 );

		skin->drawStr( canvas, 0, ltx+5+cx*24, lty+sy+5, "NOISE:" );
		skin->drawStr( canvas, 0, ltx+5+cx*35, lty+sy+5, "TONE:" );
		skin->drawStr( canvas, 0, ltx+5+cx*47, lty+sy+5, "NOTE:" );
		
		if( pSsgCh->isNoiseOn() ){
			sprintf( str, "%04d", period );
			skin->drawStr( canvas, 0, ltx+5+cx*30, lty+sy+5, str );
		}
		if( pSsgCh->isToneOn() ){
			int tune = pSsgCh->getTune();
			sprintf( str, "%04d", tune );
			skin->drawStr( canvas, 0, ltx+5+cx*40, lty+sy+5, str );
		}
		if( pSsgCh->isOn() && pSsgCh->getLevel()!=0 ){
			int oct, note;
			pSsgCh->getNote( oct, note );
			skin->drawHilightKey( canvas, ltx, lty+sy+15, oct, note );
			sprintf( str, "O%d%s", oct, noteStr[note] );
			skin->drawStr( canvas, 0, ltx+5+cx*52, lty+sy+5, str );
		}
		skin->drawHBar( canvas, 290, lty+sy+15, pSsgCh->getKeyOnLevel(), 0 );
	}else{
		skin->drawDarkKeyboard( canvas, ltx, lty+sy+15 );
		skin->drawHBar( canvas, 290, lty+sy+15, 0, 0 );
	}
}

void CVisC86OPNAKey::drawADPCMTrackView( IVisBitmap *canvas, int ltx, int lty, int trNo )
{
	int sy = 0;
	char str[64];
	CVisC86Skin *skin = &gVisSkin;

	COPNAAdpcm *adpcm = pOPNA->adpcm;
	sprintf( str, "%02d", trNo+1 );
	skin->drawNumStr1( canvas, ltx+5, lty+sy+2, str );
	sprintf( str, "ADPCM" );
	skin->drawStr( canvas, 0, ltx+5+60, lty+sy+5, str );

	if( !pOPNA->getMixedMask(trNo) ){
		skin->drawKeyboard( canvas, ltx, lty+sy+15 );
		skin->drawHBar( canvas, 290, lty+sy+15, adpcm->getKeyOnLevel(), 0 );
	}else{
		skin->drawDarkKeyboard( canvas, ltx, lty+sy+15 );
		skin->drawHBar( canvas, 290, lty+sy+15, 0, 0 );
	}
	
}

void CVisC86Key::drawRhythmTrackView( IVisBitmap *canvas, int ltx, int lty,
										  int trNo, bool isMute, COPNRhythm *rhythm )
{
	int sy = 0;
	int cx=6, cy=8;
	char str[64];
	CVisC86Skin *skin = &gVisSkin;

	UINT col_mid = skin->getPal(CVisC86Skin::IDCOL_MID);
	
	sprintf( str, "%02d", trNo+1 );
	skin->drawNumStr1( canvas, ltx+5, lty+sy+2, str );
	sprintf( str, "RHYTHM" );
	skin->drawStr( canvas, 0, ltx+5+60, lty+sy+5, str );

	visDrawLine( canvas, ltx    , lty+sy+5+10, ltx+280, lty+sy+5+10, col_mid );
	visDrawLine( canvas, ltx+280, lty+sy+5+10, ltx+280, lty+sy+5+30, col_mid );
	visDrawLine( canvas, ltx+280, lty+sy+5+30, ltx    , lty+sy+5+30, col_mid );
	visDrawLine( canvas, ltx    , lty+sy+5+30, ltx    , lty+sy+5+10, col_mid );

	if( !isMute ){
		if( rhythm->rim->isOn() )
			skin->drawStr( canvas, 0, ltx+5+cx*9, lty+sy+5+18, "RIM" );
		if( rhythm->tom->isOn() )
			skin->drawStr( canvas, 0, ltx+5+cx*14, lty+sy+5+18, "TOM" );
		if( rhythm->hh->isOn() )
			skin->drawStr( canvas, 0, ltx+5+cx*19, lty+sy+5+18, "HH" );
		if( rhythm->top->isOn() )
			skin->drawStr( canvas, 0, ltx+5+cx*24, lty+sy+5+18, "TOP" );
		if( rhythm->sd->isOn() )
			skin->drawStr( canvas, 0, ltx+5+cx*29, lty+sy+5+18, "SD" );
		if( rhythm->bd->isOn() )
			skin->drawStr( canvas, 0, ltx+5+cx*34, lty+sy+5+18, "BD");
		
		skin->drawHBar( canvas, 290, lty+sy+16, rhythm->getKeyOnLevel(), 0 );
	}else{
		skin->drawHBar( canvas, 290, lty+sy+16, 0, 0 );
	}
}

// --------------------------------------------------------
bool CVisC86OPNAKey::create( HWND parent )
{
	if( !CVisWnd::create(
		WS_EX_TOOLWINDOW, (WS_POPUP | WS_CLIPCHILDREN), parent ) )
		return false;

	::ShowWindow( hWnd, SW_SHOWNOACTIVATE );

	for( int i=0; i<14; i++ ){
		if( 3<=i && i<=5 ) continue;
		muteSw[i] = CVisMuteSwPtr( new CVisMuteSw( this, 5+28, 7+i*35 ) );
		muteSw[i]->getter = [this, i]()-> int { return this->pOPNA->getPartMask(i);};
		muteSw[i]->setter = [this, i](int mask) { this->pOPNA->setPartMask(i, mask ? true : false); };
		widgets.push_back(muteSw[i]);

		soloSw[i] = CVisSoloSwPtr( new CVisSoloSw( this, 5+28+16, 7+i*35 ) );
		soloSw[i]->getter = [this, i]()-> int { return this->pOPNA->getPartSolo(i);};
		soloSw[i]->setter = [this, i](int mask) { this->pOPNA->setPartSolo(i, mask ? true : false); };
		widgets.push_back(soloSw[i]);
	}
	
	
	return true;
}

void CVisC86OPNAKey::onPaintClient(void)
{
	visFillRect( clientCanvas, 0, 0, clientCanvas->getWidth(), clientCanvas->getHeight(), ARGB(255,0,0,0) );
	
	if( pOPNA ){
		int sx=5, sy=5, cx=6, cy=8;
		int tr=0;
		for( int i=0; i<2; i++ ){
			drawFMTrackView( clientCanvas, sx, sy, tr, i,
							 pOPNA->getMixedMask(tr), pOPNA->fm->ch[i] );
			sy+=35; tr++;
		}
		
		drawFM3EXTrackView( clientCanvas, sx, sy, tr,
							pOPNA->getMixedMask(tr), pOPNA->fm->ch[2] );//, 1+i );
		sy+=35*4; tr+=4;
		
		for( int i=3; i<6; i++ ){
			drawFMTrackView( clientCanvas, sx, sy, tr, i, 
							 pOPNA->getMixedMask(tr), pOPNA->fm->ch[i] );
			sy+=35; tr++;
		}
		for( int i=0; i<3; i++ ){
			drawSSGTrackView( clientCanvas, sx, sy, tr, i,
							  pOPNA->getMixedMask(tr), pOPNA->ssg );
			sy+=35; tr++;
		}
		drawADPCMTrackView( clientCanvas, sx, sy, tr );
		sy+=35; tr++;
		drawRhythmTrackView( clientCanvas, sx, sy, tr,
							 pOPNA->getMixedMask(tr), pOPNA->rhythm );
	}
}




// --------------------------------------------------------
bool CVisC86OPN3LKey::create( HWND parent )
{
	if( !CVisWnd::create(
		WS_EX_TOOLWINDOW, (WS_POPUP | WS_CLIPCHILDREN), parent ) )
		return false;

	::ShowWindow( hWnd, SW_SHOWNOACTIVATE );

	for( int i=0; i<13; i++ ){
		if( 3<=i && i<=5 ) continue;
		muteSw[i] = CVisMuteSwPtr( new CVisMuteSw( this, 5+28, 7+i*35 ) );
		muteSw[i]->getter = [this, i]()-> int { return this->pOPN3L->getPartMask(i);};
		muteSw[i]->setter = [this, i](int mask) { this->pOPN3L->setPartMask(i, mask ? true : false); };
		widgets.push_back(muteSw[i]);

		soloSw[i] = CVisSoloSwPtr( new CVisSoloSw( this, 5+28+16, 7+i*35 ) );
		soloSw[i]->getter = [this, i]()-> int { return this->pOPN3L->getPartSolo(i);};
		soloSw[i]->setter = [this, i](int mask) { this->pOPN3L->setPartSolo(i, mask ? true : false); };
		widgets.push_back(soloSw[i]);
	}
	
	
	return true;
}

void CVisC86OPN3LKey::onPaintClient(void)
{
	visFillRect( clientCanvas, 0, 0, clientCanvas->getWidth(), clientCanvas->getHeight(), ARGB(255,0,0,0) );
	
	if( pOPN3L ){
		int sx=5, sy=5, cx=6, cy=8;
		int tr=0;
		for( int i=0; i<2; i++ ){
			drawFMTrackView( clientCanvas, sx, sy, tr, i,
							 pOPN3L->getMixedMask(tr), pOPN3L->fm->ch[i] );
			sy+=35; tr++;
		}
		
		drawFM3EXTrackView( clientCanvas, sx, sy, tr,
							pOPN3L->getMixedMask(tr), pOPN3L->fm->ch[2] );//, 1+i );
		sy+=35*4; tr+=4;
		
		for( int i=3; i<6; i++ ){
			drawFMTrackView( clientCanvas, sx, sy, tr, i, 
							 pOPN3L->getMixedMask(tr), pOPN3L->fm->ch[i] );
			sy+=35; tr++;
		}
		for( int i=0; i<3; i++ ){
			drawSSGTrackView( clientCanvas, sx, sy, tr, i,
							  pOPN3L->getMixedMask(tr), pOPN3L->ssg );
			sy+=35; tr++;
		}
		drawRhythmTrackView( clientCanvas, sx, sy, tr,
							 pOPN3L->getMixedMask(tr), pOPN3L->rhythm );
	}
}

// --------------------------------------------------------
bool CVisC86OPMKey::create( HWND parent )
{
	if( !CVisWnd::create(
		WS_EX_TOOLWINDOW, (WS_POPUP | WS_CLIPCHILDREN), parent ) )
		return false;

	::ShowWindow( hWnd, SW_SHOWNOACTIVATE );

	for( int i=0; i<8; i++ ){
		muteSw[i] = CVisMuteSwPtr( new CVisMuteSw( this, 5+28, 7+i*35 ) );
		muteSw[i]->getter = [this, i]()-> int { return this->pOPM->getPartMask(i);};
		muteSw[i]->setter = [this, i](int mask) { this->pOPM->setPartMask(i, mask ? true : false); };
		widgets.push_back(muteSw[i]);

		soloSw[i] = CVisSoloSwPtr( new CVisSoloSw( this, 5+28+16, 7+i*35 ) );
		soloSw[i]->getter = [this, i]()-> int { return this->pOPM->getPartSolo(i);};
		soloSw[i]->setter = [this, i](int mask) { this->pOPM->setPartSolo(i, mask ? true : false); };
		widgets.push_back(soloSw[i]);
	}
	
	
	return true;
}

void CVisC86OPMKey::onPaintClient(void)
{
	visFillRect( clientCanvas, 0, 0, clientCanvas->getWidth(), clientCanvas->getHeight(), ARGB(255,0,0,0) );
	
	if( pOPM ){
		int sx=5, sy=5, cx=6, cy=8;
		int tr=0;
		for( int i=0; i<8; i++ ){
			drawFMTrackView( clientCanvas, sx, sy, tr, i,
							 pOPM->getMixedMask(tr), pOPM->fm->ch[i] );
			sy+=35; tr++;
		}
	}
}

void CVisC86OPMKey::drawFMTrackView( IVisBitmap *canvas, int ltx, int lty,
								  int trNo, int fmNo, bool isMute, COPMFmCh *pFMCh )
{
	int sy = 0;
	char str[64], tmp[64];
	int cx=6, cy=8;
	CVisC86Skin *skin = &gVisSkin;

	sprintf( str, "%02d", trNo+1 );
	skin->drawNumStr1( canvas, ltx+5, lty+sy+2, str );
	sprintf( str, "FM-CH%d", fmNo+1 );
	skin->drawStr( canvas, 0, ltx+5+cx*10, lty+sy+5, str );

	if( !isMute ){
		strcpy(str, "SLOT:");
		for( int i=0; i<4; i++ ){
			int sw = pFMCh->slot[i]->isOn();
			sprintf( tmp, "%d", i );
			if( sw ) strncat( str, tmp, 64 );
			else  strncat( str, "_", 64 );
		}
		skin->drawStr( canvas, 0, ltx+5+cx*18, lty+sy+5, str );

		int kcoct = pFMCh->getKeyCodeOct();
		int kcnote = pFMCh->getKeyCodeNote();
		int kf = pFMCh->getKeyFraction();
		sprintf( str, "KC:%02d-%02d KF:%02d     NOTE:", kcoct, kcnote, kf );
		skin->drawStr( canvas, 0, ltx+5+cx*28, lty+sy+5, str );
		skin->drawKeyboard( canvas, ltx, lty+sy+15 );

		if( pFMCh->isKeyOn() && pFMCh->getMixLevel()!=127 ){
			int oct=0, note=0;
			pFMCh->getNote( oct, note );
			skin->drawHilightKey( canvas, ltx, lty+sy+15, oct, note );
			sprintf( str, "O%d%s", oct, noteStr[note] );
			skin->drawStr( canvas, 0, ltx+5+cx*52, lty+sy+5, str );
		}
		skin->drawHBar( canvas, 290, lty+sy+15, pFMCh->getKeyOnLevel(), 0 );
	}else{
		skin->drawDarkKeyboard( canvas, ltx, lty+sy+15 );
		skin->drawHBar( canvas, 290, lty+sy+15, 0, 0 );
	}
}


// --------------------------------------------------------
CVisC86KeyPtr c86ctl::vis::visC86KeyViewFactory(Chip *pchip, int id)
{
	if( typeid(*pchip) == typeid(COPNA) ){
		return CVisC86KeyPtr( new CVisC86OPNAKey(dynamic_cast<COPNA*>(pchip), id ) );
	}else if( typeid(*pchip) == typeid(COPN3L) ){
		return CVisC86KeyPtr( new CVisC86OPN3LKey(dynamic_cast<COPN3L*>(pchip), id) );
	}else if( typeid(*pchip) == typeid(COPM) ){
		return CVisC86KeyPtr( new CVisC86OPMKey(dynamic_cast<COPM*>(pchip), id) );
	}
	return 0;
}
