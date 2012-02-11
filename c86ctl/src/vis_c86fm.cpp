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

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define WINDOW_WIDTH  (316+4)
#define WINDOW_HEIGHT (75*5+4+15)


bool CVisC86OPNAFm::create( HWND parent )
{
	int left = INT_MIN;
	int top = INT_MIN;
	
	if( !CVisWnd::create(
		left, top, windowWidth, windowHeight,
		WS_EX_TOOLWINDOW, (WS_POPUP | WS_CLIPCHILDREN), parent ) )
		return false;

	COPNAFm *pOPNAFm = &pOPNA->fm[this->ch];
	// AMS
	knobAMS = CVisKnobPtr( new CVisKnob(this, 134-11, 25-10));
	knobAMS->setRange(0,3);
	knobAMS->getter = [pOPNAFm]() -> int{ return pOPNAFm->getAMS(); };
	//knobAMS->setter = [pOPNAFm](int ams) { pOPNAFm->setAMS(ams); };
	widgets.push_back(knobAMS);

	// PMS
	knobPMS = CVisKnobPtr( new CVisKnob(this, 158-11, 25-10));
	knobPMS->setRange(0,7);
	knobPMS->getter = [pOPNAFm]() -> int{ return pOPNAFm->getPMS(); };
	widgets.push_back(knobPMS);

	// FB
	knobFB = CVisKnobPtr( new CVisKnob(this, 134-11, 59-10));
	knobFB->setRange(0,7);
	knobFB->getter = [pOPNAFm]() -> int{ return pOPNAFm->getFeedback(); };
	widgets.push_back(knobFB);

	// PAN
	knobPAN = CVisKnobPtr( new CVisKnob(this, 158-11, 59-10));
	knobPAN->setRange(-1,1);
	knobPAN->getter = [pOPNAFm]() -> int{ return pOPNAFm->getPan(); };
	widgets.push_back(knobPAN);

	int x=0, y=75;
	for(int i=0; i<4; i++){
		COPNAFmSlot *slot = &pOPNA->fm[this->ch].slot[i];
		// AR
		knobAR[i] = CVisKnobPtr( new CVisKnob(this, x+173-11, y+27-10));
		knobAR[i]->setRange(0,31);
		knobAR[i]->getter = [slot, i]() -> int{ return slot->getAttackRate(); };
		widgets.push_back(knobAR[i]);
		
		// DR
		knobDR[i] = CVisKnobPtr( new CVisKnob(this, x+198-11, y+27-10));
		knobDR[i]->setRange(0,31);
		knobDR[i]->getter = [slot, i]() -> int{ return slot->getDecayRate(); };
		widgets.push_back(knobDR[i]);

		// SR
		knobSR[i] = CVisKnobPtr( new CVisKnob(this, x+223-11, y+27-10));
		knobSR[i]->setRange(0,31);
		knobSR[i]->getter = [slot, i]() -> int{ return slot->getSustainRate(); };
		widgets.push_back(knobSR[i]);

		// RR
		knobRR[i] = CVisKnobPtr( new CVisKnob(this, x+248-11, y+27-10));
		knobRR[i]->setRange(0,31);
		knobRR[i]->getter = [slot, i]() -> int{ return slot->getReleaseRate(); };
		widgets.push_back(knobRR[i]);

		// SL
		knobSL[i] = CVisKnobPtr( new CVisKnob(this, x+273-11, y+27-10));
		knobSL[i]->setRange(0,15);
		knobSL[i]->getter = [slot, i]() -> int{ return slot->getSustainLevel(); };
		widgets.push_back(knobSL[i]);

		// TL
		knobTL[i] = CVisKnobPtr( new CVisKnob(this, x+298-11, y+27-10));
		knobTL[i]->setRange(0,127);
		knobTL[i]->getter = [slot, i]() -> int{ return slot->getTotalLevel(); };
		widgets.push_back(knobTL[i]);
		
		// MUL
		knobMUL[i] = CVisKnobPtr( new CVisKnob(this, x+173-11, y+60-10));
		knobMUL[i]->setRange(0,15);
		knobMUL[i]->getter = [slot, i]() -> int{ return slot->getMultiple(); };
		widgets.push_back(knobMUL[i]);

		// DET
		knobDET[i] = CVisKnobPtr( new CVisKnob(this, x+198-11, y+60-10));
		knobDET[i]->setRange(0,7);
		knobDET[i]->getter = [slot, i]() -> int{ return slot->getDetune(); };
		widgets.push_back(knobDET[i]);
		y+=75;
	}
	
	::ShowWindow( hWnd, SW_SHOWNOACTIVATE );
	
	return true;
}

void CVisC86OPNAFm::close()
{
	// TODO: delete widgets.
	widgets.clear();
	
	CVisWnd::close();
}


void CVisC86OPNAFm::onPaintClient()
{
	visFillRect( clientCanvas, 0, 0, clientCanvas->getWidth(), clientCanvas->getHeight(), ARGB(255,0,0,0) );

	// slot view
	if( pOPNA ){
		int sx=5, sy=5, cx=6, cy=8;
		drawFMView( clientCanvas, 0, 0, &pOPNA->fm[ch] );
		for( int i=0; i<4; i++ )
			drawFMSlotView( clientCanvas, 0, 75*(1+i), &pOPNA->fm[ch].slot[i], i );
	}

#if 0
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
#endif
}

void CVisC86OPNAFm::drawFMView( IVisBitmap *canvas, int x, int y, COPNAFm *pFM )
{
	int exmode = pFM->getExMode();
	CVisC86Skin *skin = &gVisSkin;
	skin->drawFMSkin( canvas, x, y, exmode );
	
	int algno = pFM->getAlgorithm();
	skin->drawFMAlgorithm( canvas, x+20, y+7, algno );
	
	//slot ON/OFF
	const int lx[4] = {243, 258, 273, 288};
	for( int i=0; i<4; i++ ){
		skin->drawFMSlotLight( canvas, x+lx[i], y+56, i, pFM->slot[i].isOn() );
	}

	// freq
	double freq = pFM->getFreq();
	char str[128];
	if( !exmode ){
		sprintf( str, "% 11.1fHz", freq );
		skin->drawStr( canvas, 0, 211, 10, str );
	}else{
		const char *modestr[] = { "CSM", "EFF", "???" };
		skin->drawStr( canvas, 0, 183, 59, modestr[exmode-1] );
		
		for( int i=0; i<4; i++ ){
			freq = pFM->getFreqEx(i);
			sprintf( str, "% 11.1fHz", freq );
			skin->drawStr( canvas, 0, 211, 6+12*i, str );
		}
	}

//	knobAMS->setValue(pFM->getAMS());
//	knobPMS->setValue(pFM->getPMS());
//	knobFB->setValue(pFM->getFeedback());
//	knobPAN->setValue(pFM->getPan());

#if 0
	int exmode = pFM->getExMode();
	BMPREG reg = skinreg_view[ exmode?1:0 ];
	::BitBlt( hdc, x, y, reg.width, reg.height, hskin, reg.left, reg.top, SRCCOPY );

	int algno = pFM->getAlgorithm();
	reg = skinreg_alg[algno];
	::BitBlt( hdc, x+19, y+6, reg.width, reg.height, hskin, reg.left, reg.top, SRCCOPY );

	HPEN mypen = ::CreatePen( PS_SOLID, 2, col_high);
	HPEN oldpen = (HPEN)::SelectObject( hdc, mypen );

	int cx,cy;
	double ams = pFM->getAMS();
	cx=x+134; cy=y+25;
	draw_knob( hdc, ams/3.0, cx, cy ); // ams

	double pms = pFM->getPMS();
	cx=x+158; cy=y+25;
	draw_knob( hdc, pms/7.0, cx, cy ); // pms

	double fb = pFM->getFeedback();
	cx=x+134; cy=y+59;
	draw_knob( hdc, fb/7.0, cx, cy ); // fb

	double pan = pFM->getPan();
	cx=x+158; cy=y+59;
	draw_knob( hdc, (pan+1.0)/2.0, cx, cy ); // pan

	//slot ON/OFF
	const int lx[4] = {243, 258, 273, 288};
	for( int i=0; i<4; i++ ){
		int idx = pFM->slot[i].isOn() ? 4 : 0;
		reg = skinreg_tool[7+i+idx];
		::BitBlt( hdc, x+lx[i], y+56, reg.width, reg.height, hskin, reg.left, reg.top, SRCCOPY );
	}

	// freq
	double freq = pFM->getFreq();
	char str[128];
	if( !exmode ){
		sprintf( str, "% 11.1fHz", freq );
		vis_draw_str( hdc, hskin, hmask, 0, 211, 10, str );
	}else{
		const char *modestr[] = { "CSM", "EFF", "???" };
		vis_draw_str( hdc, hskin, hmask, 0, 183, 59, modestr[exmode-1] );
		
		for( int i=0; i<4; i++ ){
			freq = pFM->getFreqEx(i);
			sprintf( str, "% 11.1fHz", freq );
			vis_draw_str( hdc, hskin, hmask, 0, 211, 6+12*i, str );
		}
	}
	
	::SelectObject( hdc, oldpen );
	::DeleteObject( mypen );
#endif
}

void CVisC86OPNAFm::drawFMSlotView( IVisBitmap *canvas, int x, int y, COPNAFmSlot *pSlot, int slotidx )
{
	CVisC86Skin *skin = &gVisSkin;
	skin->drawFMSlotSkin( canvas, x, y );
	
/*	knobAR[slotidx]->setValue(pSlot->getAttackRate());
	knobDR[slotidx]->setValue(pSlot->getDecayRate());
	knobSR[slotidx]->setValue(pSlot->getSustainRate());
	knobRR[slotidx]->setValue(pSlot->getReleaseRate());
	knobSL[slotidx]->setValue(pSlot->getSustainLevel());
	knobTL[slotidx]->setValue(pSlot->getTotalLevel());
	knobMUL[slotidx]->setValue(pSlot->getMultiple());
	knobDET[slotidx]->setValue(pSlot->getDecayRate());
*/
#if 0
	BMPREG reg = skinreg_view[2];
	::BitBlt( hdc, x, y, reg.width, reg.height, hskin, reg.left, reg.top, SRCCOPY );

	// index
	char str[32];
	sprintf(str, "%d", slotidx+1);
	vis_draw_vstr( hdc, hskin, hmask, 0, x+8, y+20, str );
		

	HPEN mypen = ::CreatePen( PS_SOLID, 2, col_high);
	HPEN oldpen = (HPEN)::SelectObject( hdc, mypen );

	int cx,cy;

	double ar = pSlot->getAttackRate();
	cx=x+173; cy=y+27;
	draw_knob( hdc, ar/31.0, cx, cy ); // AR

	double dr = pSlot->getDecayRate();
	cx=x+198; cy=y+27;
	draw_knob( hdc, dr/31.0, cx, cy ); // DR

	double sr = pSlot->getSustainRate();
	cx=x+223; cy=y+27;
	draw_knob( hdc, sr/31.0, cx, cy ); // SR

	double rr = pSlot->getReleaseRate();
	cx=x+248; cy=y+27;
	draw_knob( hdc, rr/31.0, cx, cy ); // RR

	double sl = pSlot->getSustainLevel();
	cx=x+273; cy=y+27;
	draw_knob( hdc, sl/15.0, cx, cy ); // SL

	double tl = pSlot->getTotalLevel();
	cx=x+298; cy=y+27;
	draw_knob( hdc, tl/127.0, cx, cy ); // TL

	double mul = pSlot->getMultiple();
	cx=x+173; cy=y+60;
	draw_knob( hdc, mul/15.0, cx, cy ); // MUL

	double det = pSlot->getDetune();
	cx=x+198; cy=y+60;
	draw_knob( hdc, det/7.0, cx, cy ); // DET
	
	::SelectObject( hdc, oldpen );
	::DeleteObject( mypen );

	// envelope graph
	// TODO:ここは仕様が理解出来ていないので相当適当。
	//      それっぽく見せてるだけ。後で書き直す。
	if ( 0 < ar ){
		mypen = ::CreatePen( PS_SOLID, 2, col_high);
		oldpen = (HPEN)::SelectObject( hdc, mypen );
		// note on = 0
		// note off = 32*3
		int gh = 64;
		int gw = 128;
		int at, dt1, dt2, rt;
		int al, dl1, dl2;
		al = (int)(tl*gh/127.0);
		dl1 = (dr==0) ? al : (int)(tl*sl*gh/(127.0*15.0)); // (tl/127.0)*(sl/15.0)*gh
		dl2 = (ar==0) ? dl1 : dl1*0.5;
		double ar2 = 31-ar;
		at = (ar2*gw)/(2*30);
		dt1 = at + 2 + (dr*(32*3-2)/31.0) ; // at=0で( dr=0のときdt1=32*3, dr==31のときdt1=ar+2 )になるように。
		dt2 = 32*3;
		if( dt2<dt1 ) dt1 = dt2;
		rt = dt2 + ((15-rr)*32/15.0);

		cx=x+27; cy=y+6;
		// 351, 156
		::MoveToEx( hdc, cx, cy+gh-1, NULL );
		::LineTo( hdc, cx+at, cy+gh-1-al );
		::LineTo( hdc, cx+dt1, cy+gh-1-dl1 );
		::LineTo( hdc, cx+dt2, cy+gh-1-dl2 );
		::LineTo( hdc, cx+rt, cy+gh-1 );
		
		::SelectObject( hdc, oldpen );
		::DeleteObject( mypen );
	}

	// AM
	int idx = pSlot->isAM() ? 1 : 0;
	reg = skinreg_tool[4+idx];
	::BitBlt( hdc, x+216, y+49, reg.width, reg.height, hskin, reg.left, reg.top, SRCCOPY );

	// SSGEG
	idx = pSlot->getSSGEGType();
	reg = skinreg_alg[8+idx];
	cx=x+255; cy=y+58;
	::BitBlt( hdc, cx, cy, reg.width, reg.height, hskin, reg.left, reg.top, SRCCOPY );
#endif
}

// --------------------------------------------------------
CVisC86FmPtr visC86FmViewFactory(Chip *pchip, int id, int ch)
{
	if( typeid(*pchip) == typeid(COPNA) ){
		return CVisC86FmPtr( new CVisC86OPNAFm(dynamic_cast<COPNA*>(pchip), id, ch ) );
//	}else if( typeid(*pchip) == typeid(COPN3L) ){
//		return CVisC86RegPtr( new CVisC86OPN3LKey(dynamic_cast<COPN3L*>(pchip), id) );
//	}else if( typeid(*pchip) == typeid(COPM) ){
//		return CVisC86RegPtr( new CVisC86OPMKey(dynamic_cast<COPM*>(pchip), id) );
	}
	return 0;
}
