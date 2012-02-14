/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "module.h"
#include "resource.h"
#include "vis_c86sub.h"
#include "vis_c86skin.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if 0
#define PI 3.14159265358979

#define col_light	RGB(199,200,255)
#define col_shadow	RGB(43,44,75)
#define col_mid		RGB(68,68,119)
#define col_high	RGB(128,128,255)

static HBITMAP hSkinBMP = NULL;
static HDC hSkinDC = NULL;

struct BMPREG{
	INT left;
	INT top;
	INT width;
	INT height;
};

BMPREG skinreg_frame[] = {
	{144,83,8,15},		// window frame left
	{279,83,8,15},		// window frame right
	{152,83,129,15}		// window frame center
};
BMPREG skinreg_fonts[] = {
	{0,0,6,8},			// font1
	{97,0,6,8},			// font2
	{194,0,6,8},		// font3
	{0,143,6,8},		// vfont1
	{48,143,6,8},		// vfont2
	{96,143,6,8},		// vfont3
	{144,67,8,11},		// num font1 (mini)
	{144,48,10,18},		// num font2 (big)
};
BMPREG skinreg_alg[] = {
	{0,385,96,64},		// ALG0
	{95,385,96,64},		// ALG1
	{190,385,96,64},	// ALG2
	{285,385,96,64},	// ALG3
	{0,448,96,64},		// ALG4
	{95,448,96,64},		// ALG5
	{190,448,96,64},	// ALG6
	{285,448,96,64},	// ALG7
	{382,385,52,11},	// SSG EG0
	{382,395,52,11},	// SSG EG0
	{382,405,52,11},	// SSG EG1
	{382,415,52,11},	// SSG EG2
	{382,425,52,11},	// SSG EG3
	{382,435,52,11},	// SSG EG4
	{382,445,52,11},	// SSG EG5
	{382,455,52,11},	// SSG EG6
	{382,465,52,11},	// SSG EG7
};
BMPREG skinreg_tick[] = {
	{0,145,31,31},		// rot none
	{32,145,31,31},		// rot0
	{64,145,31,31},		// rot1
	{96,145,31,31},		// rot2
	{128,145,31,31},	// rot3
	{160,145,31,31},	// rot4
	{192,145,31,31},	// rot5
	{224,145,31,31},	// rot6
	{256,145,31,31}		// rot7
};

BMPREG skinreg_tool[] = {
	{144,99,59,20},		// 0:keyboard
	{435,385,21,96},	// 1:bar shadow
	{457,385,21,96},	// 2:bar light
	{204,99,21,20},		// 3:つまみ
	{238,99,11,20},		// 4:SW OFF
	{226,99,11,20},		// 5:SW ON
	{144,120,115,24},	// 6:logo
	{205,178,13,13},	// 7:(A)
	{219,178,13,13},	// 8:(B)
	{233,178,13,13},	// 9:(C)
	{247,178,13,13},	// 10:(D)
	{205,192,13,13},	// 11:(A)
	{219,192,13,13},	// 12:(B)
	{233,192,13,13},	// 13:(C)
	{247,192,13,13},	// 14:(D)
	{250,99,40,20},		// 15:dark keyboard
};

BMPREG skinreg_view[] = {
	{324,0,316,76},			// FM view1
	{324,75,316,76},		// FM view2
	{324,150,316,76},		// FM slot view
};

BMPREG skinbtn[] = {
	{0,178,40,17},		//  0:play
	{0,196,40,17},		//    play on
	{41,178,40,17},		//  1:stop
	{41,196,40,17},		//    stop on
	{82,178,40,17},		//  2:pause
	{82,196,40,17},		//    pause on
	{123,178,40,17},	//  3:fade
	{123,196,40,17},	//    fade on
	{164,178,40,17},	//  4:busy
	{164,196,40,17},	//    busy on
	{0,214,81,17},		//  5:key
	{0,232,81,17},		//    key on
	{82,214,81,17},		//  6:reg
	{82,232,81,17},		//    reg on
	{164,214,81,17},	//  7:level
	{164,232,81,17},	//    level on
	{0,250,40,17},		//  8:fm1
	{0,268,40,17},		//    fm1 on
	{41,250,40,17},		//  9:fm2
	{41,268,40,17},		//    fm2 on
	{82,250,40,17},		// 10:fm3
	{82,268,40,17},		//    fm3 on
	{123,250,40,17},	// 11:fm4
	{123,268,40,17},	//    fm4 on
	{164,250,40,17},	// 12:fm5
	{164,268,40,17},	//    fm5 on
	{205,250,40,17},	// 13:fm6
	{205,268,40,17},	//    fm6 on
	{0,286,40,17},		// 14:ssg1
	{0,304,40,17},		//    ssg1 on
	{41,286,40,17},		// 15:ssg2
	{41,304,40,17},		//    ssg2 on
	{82,286,40,17},		// 16:ssg3
	{82,304,40,17},		//    ssg3 on
	{123,286,40,17},	// 17:adpcm
	{123,304,40,17},	//    adpcm
	{164,286,40,17},	// 18:reserve
	{164,304,40,17},	//    reserve
	{205,286,40,17},	// 19:reserve
	{205,304,40,17},	//    reserve
};

const int keyW1 = 5;	// 白鍵
const int keyH1 = 20;
const int keyW2 = 3;	// 黒鍵
const int keyH2 = 10;
const int keyXOffset[] = { 0,3,5,8,10,15,18,20,23,25,28,30 };
const int keyHiXOffset[] = { 35,50,40,50,45,35,50,40,50,40,50,45 };
const int keyHiW[] = { 4,3,4,3,4,4,3,4,3,4,3,4,3,4 };
const int keyHiH[] = { 20,10,20,10,20,20,10,20,10,20,10,20,10,20 };


void vis_draw_str( HDC hdc, HDC hskin, HDC hmask, int type, int x, int y, const char *str )
{
	if( type < 0 || 3 <= type )
		return;

	int left = skinreg_fonts[type].left;
	int top = skinreg_fonts[type].top;
	int len = (int)strlen(str);
	for( int i=0; i<len; i++ ){
		int c = str[i]-0x20;
		if( 0<=c && c<96 ){
			int srcx = left + 6 * (c&0x0f);
			int srcy = top  + 8 * (c>>4);
			BitBlt(hdc, x+6*i, y, 6, 7, hmask, srcx, srcy, SRCAND );
			BitBlt(hdc, x+6*i, y, 6, 7, hskin, srcx, srcy, SRCPAINT );
		}
	}
}

void vis_draw_vstr( HDC hdc, HDC hskin, HDC hmask, int type, int x, int y, const char *str )
{
	if( type < 0 || 3 <= type )
		return;

	int left = skinreg_fonts[3+type].left;
	int top = skinreg_fonts[3+type].top;
	int len = (int)strlen(str);
	for( int i=0; i<len; i++ ){
		int c = str[i]-0x20;
		if( 0<=c && c<96 ){
			int srcx = left + (8 * (c>>4));
			int srcy = top  - (6 * (c&0x0f)) - 5;
			BitBlt(hdc, x-5, y-i*6, 7, 6, hmask, srcx, srcy, SRCAND );
			BitBlt(hdc, x-5, y-i*6, 7, 6, hskin, srcx, srcy, SRCPAINT );
		}
	}
}

void vis_draw_numstr1( HDC hdc, HDC hskin, HDC hmask, int x, int y, const char *str )
{
	int left = skinreg_fonts[6].left;
	int top = skinreg_fonts[6].top;
	int cx = 8;//skinreg_fonts[6].width;
	int cy = 11;//skinreg_fonts[6].height;
	int len = (int)strlen(str);
	for( int i=0; i<len; i++ ){
		char c = str[i]==' ' ? 0 : str[i] - '0' + 1;
		int srcx = left + cx * c;
		int srcy = top;

		BitBlt(hdc, x+cx*i, y, cx, cy, hmask, srcx, srcy, SRCAND );
		BitBlt(hdc, x+cx*i, y, cx, cy, hskin, srcx, srcy, SRCPAINT );
	}
}

void vis_draw_keyboard( HDC hdc, HDC hskin, HDC hmask, int x, int y )
{
	int sx = skinreg_tool[0].left;
	int sy = skinreg_tool[0].top;
	
	for( int i=0; i<8; i++ ){
		int w = keyW1*8;
		BitBlt(hdc, x+w*i, y, w, keyH1, hmask, sx, sy, SRCAND );
		BitBlt(hdc, x+w*i, y, w, keyH1, hskin, sx, sy, SRCPAINT );
	}
}
void vis_draw_dark_keyboard( HDC hdc, HDC hskin, HDC hmask, int x, int y )
{
	int sx = skinreg_tool[15].left;
	int sy = skinreg_tool[15].top;
	
	for( int i=0; i<8; i++ ){
		int w = keyW1*8;
		BitBlt(hdc, x+w*i, y, w, keyH1, hmask, sx, sy, SRCAND );
		BitBlt(hdc, x+w*i, y, w, keyH1, hskin, sx, sy, SRCPAINT );
	}
}

void vis_draw_hilight_key( HDC hdc, HDC hskin, HDC hmask, int x, int y, int oct, int note )
{
	int srcx = skinreg_tool[0].left + keyHiXOffset[note];
	int srcy = skinreg_tool[0].top;
	int dstx = keyW1*8*oct + keyXOffset[note];
	int w = keyHiW[note];
	int h = keyHiH[note];
	BitBlt(hdc, x+dstx, y, w, h, hmask, srcx, srcy, SRCAND );
	BitBlt(hdc, x+dstx, y, w, h, hskin, srcx, srcy, SRCPAINT );
}

void vis_draw_tickcircle( HDC hdc, HDC hskin, HDC hmask, int x, int y, int tick )
{
	int idx = (tick==-1) ? 0 : 1+(tick&0x7);
	BMPREG reg = skinreg_tick[idx];
	BitBlt( hdc, x, y, reg.width, reg.height, hmask, reg.left, reg.top, SRCAND );
	BitBlt( hdc, x, y, reg.width, reg.height, hskin, reg.left, reg.top, SRCPAINT );
}

void vis_draw_logo(HDC hdc, HDC hskin, HDC hmask, int x, int y )
{
	BMPREG reg = skinreg_tool[6];
	BitBlt( hdc, x, y, reg.width, reg.height, hmask, reg.left, reg.top, SRCAND );
	BitBlt( hdc, x, y, reg.width, reg.height, hskin, reg.left, reg.top, SRCPAINT );
}

void vis_draw_button(HDC hdc, HDC hskin, HDC hmask, int x, int y, int id, int sw )
{
	if(id<0||19<id) return;
	BMPREG reg = skinbtn[(id<<1)+(sw&0x01)];
	BitBlt( hdc, x, y, reg.width, reg.height, hmask, reg.left, reg.top, SRCAND );
	BitBlt( hdc, x, y, reg.width, reg.height, hskin, reg.left, reg.top, SRCPAINT );
}

bool vis_hittest_button(int x, int y, int id, int mx, int my )
{
	if(id<0||19<id) return false;
	BMPREG reg = skinbtn[(id<<1)];
	mx-=x;
	my-=y;
	return ( 0<=mx && mx<reg.width &&
			 0<=my && my<reg.height ) ? true : false;
}

void vis_draw_frame(HWND hwnd, HDC hdc, HDC hskin, HDC hmask, CHAR *title )
{
	RECT rc, rc2;
	GetClientRect(hwnd, &rc);
	HPEN hpen=0, holdpen=0;

	// frame
	hpen = ::CreatePen(PS_SOLID, 1, col_light);
	if( hpen ){
		holdpen = (HPEN)::SelectObject(hdc, hpen);
		if( holdpen ){
			::MoveToEx(hdc, rc.right, 0, NULL);
			::LineTo(hdc, 0, 0);
			::LineTo(hdc, 0, rc.bottom);
			::SelectObject(hdc, holdpen);
		}
		::DeleteObject(hpen);
	}
	hpen = ::CreatePen(PS_SOLID, 1, col_shadow);
	if( hpen ){
		holdpen = (HPEN)::SelectObject(hdc, hpen);
		if( holdpen ){
			::MoveToEx(hdc, rc.right-1, 0, NULL);
			::LineTo(hdc, rc.right-1, rc.bottom-1);
			::LineTo(hdc, 0, rc.bottom-1);
			::SelectObject(hdc, holdpen);
		}
		::DeleteObject(hpen);
	}
	hpen = ::CreatePen(PS_SOLID, 1, col_mid);
	if( hpen ){
		holdpen = (HPEN)::SelectObject(hdc, hpen);
		if( holdpen ){
			::MoveToEx(hdc, 1, 1, NULL);
			::LineTo(hdc, rc.right-2, 1);
			::LineTo(hdc, rc.right-2, rc.bottom-2);
			::LineTo(hdc, 1, rc.bottom-2);
			::LineTo(hdc, 1, 1 );
			::SelectObject(hdc, holdpen);
		}
		::DeleteObject(hpen);
	}

	// topbar
	BMPREG reg_l = skinreg_frame[0];
	BMPREG reg_r = skinreg_frame[1];
	BMPREG reg_c = skinreg_frame[2];

	BitBlt(hdc,           2, 2, reg_l.width, reg_l.height, hskin, reg_l.left, reg_l.top, SRCCOPY);
	BitBlt(hdc, rc.right-10, 2, reg_r.width, reg_r.height, hskin, reg_r.left, reg_r.top, SRCCOPY);

	int tw = rc.right - 4 - reg_l.width - reg_r.width;
	int l = 10;
	while(0<tw){
		INT w = __min( tw, reg_c.width );
		BitBlt(hdc, l, 2, w, reg_c.height, hskin, reg_c.left, reg_c.top, SRCCOPY);
		l+=w;  tw-=w;
	}

	// title
	l = (int)strlen(title);
	if( 0<l ){
		rc2.top = 3;
		rc2.bottom = rc2.top + 13;
		rc2.left = 3;
		rc2.right = rc2.left + 6*(l+2);
		::FillRect( hdc, &rc2, (HBRUSH)GetStockObject(BLACK_BRUSH) );
		vis_draw_str( hdc, hskin, hmask, 1, 10, 6, title );
	}
}


void vis_fill_rect(HDC hdc, COLORREF col, RECT *rc)
{
	SetBkColor(hdc, col);
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, rc, NULL, 0, NULL);
};

static void draw_knob( HDC hdc, double pos, int cx, int cy )
{
	double dx, dy;
	const double x = 0;
	const double y = -8;
	// 0.0~1.0 => -150~+150
	double rad = (300.0*pos-150.0)*PI/180.0;
	double dcos = cos(rad);
	double dsin = sin(rad);
	dx = dcos*x - dsin*y;
	dy = dsin*x + dcos*y;
	::MoveToEx( hdc, cx, cy, NULL );
	::LineTo( hdc, static_cast<int>(cx+dx), static_cast<int>(cy+dy) );
};

void vis_draw_fm_view( HDC hdc, HDC hskin, HDC hmask, int x, int y, COPNAFmCh *pFM )
{
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
		int idx = pFM->slot[i]->isOn() ? 4 : 0;
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
}

void vis_draw_fmslot_view( HDC hdc, HDC hskin, HDC hmask, int x, int y, COPNAFmSlot *pSlot, int slotidx )
{
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


}
#endif

// --------------------------------------------------------------------------------------
void blt( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
		  IVisBitmap *src, int src_x, int src_y )
{
	if( dst_x<0 || dst_y<0 || src_x<0 || src_y<0 )
		return;
	if( src_x+w > src->getWidth() || src_y+h > src->getHeight() ||
		dst_x+w > dst->getWidth() || dst_y+h > dst->getHeight() )
		return;

	for( int y=0; y<h; y++ ){
		UINT *ps = ((UINT*)src->getRow0( src_y+y )) + src_x;
		UINT *pd = ((UINT*)dst->getRow0( dst_y+y )) + dst_x;
		for( int x=0; x<w; x++ ){
			*pd++ = (*ps++|0xff000000);
		}
	}
}

void alphablt( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
			   IVisBitmap *src, int src_x, int src_y )
{
	if( dst_x<0 || dst_y<0 || src_x<0 || src_y<0 )
		return;
	if( src_x+w > src->getWidth() || src_y+h > src->getHeight() ||
		dst_x+w > dst->getWidth() || dst_y+h > dst->getHeight() )
		return;

	for( int y=0; y<h; y++ ){
		UINT *ps = ((UINT*)src->getRow0( src_y+y )) + src_x;
		UINT *pd = ((UINT*)dst->getRow0( dst_y+y )) + dst_x;
		for( int x=0; x<w; x++ ){
			UINT a = *ps >> 24;
			UINT na = a^0xff; // 255-a
			
			UINT s = *ps;
			UINT d = *pd;
			UINT t;
			// (s*a) + (d*(1-a))
			t = (((((s&0x00ff00ff) * ( a+1)) >> 8) & 0x00ff00ff) |
			     ((((s&0x0000ff00) * ( a+1)) >> 8) & 0x0000ff00)) +
			  + (((((d&0x00ff00ff) * (na+1)) >> 8) & 0x00ff00ff) |
			     ((((d&0x0000ff00) * (na+1)) >> 8) & 0x0000ff00));
			*pd = t;

			ps++;
			pd++;
		}
	}
}

void transblt( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
			   IVisBitmap *src1, int src1_x, int src1_y,
			   IVisBitmap *src2, int src2_x, int src2_y,
			   IVisBitmap *trans, int trans_x, int trans_y, int t )
{
	if( dst_x<0 || dst_y<0 || trans_x<0 || trans_y<0 )
		return;
	if( src1_x<0 || src1_y<0 || src2_x<0 || src2_y<0 )
		return;
	if( dst_x+w > dst->getWidth() || dst_y+h > dst->getHeight() ||
		src1_x+w > src1->getWidth() || src1_y+h > src1->getHeight() ||
		src2_x+w > src2->getWidth() || src2_y+h > src2->getHeight() ||
		trans_x+w > trans->getWidth() || trans_y+h > trans->getHeight() )
		return;

	for( int y=0; y<h; y++ ){
		UINT *pd = (UINT*)dst->getRow0( dst_y+y ) + dst_x;
		UINT *ps1 = (UINT*)src1->getRow0( src1_y+y ) + src1_x;
		UINT *ps2 = (UINT*)src2->getRow0( src2_y+y ) + src2_x;
		UINT *ts = (UINT*)trans->getRow0( trans_y+y ) + trans_x;
		for( int x=0; x<w; x++ ){
			int a = *ts&0xff;
			*pd = ( (t<=a) ? *ps1 : *ps2 ) | 0xff000000;
			pd++; ps1++; ps2++; ts++; 
		}
	}
}

// [tmin, tmax)
void transblt2( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
			   IVisBitmap *src1, int src1_x, int src1_y,
			   IVisBitmap *src2, int src2_x, int src2_y,
			   IVisBitmap *trans, int trans_x, int trans_y, int tmin, int tmax )
{
	if( dst_x<0 || dst_y<0 || trans_x<0 || trans_y<0 )
		return;
	if( src1_x<0 || src1_y<0 || src2_x<0 || src2_y<0 )
		return;
	if( dst_x+w > dst->getWidth() || dst_y+h > dst->getHeight() ||
		src1_x+w > src1->getWidth() || src1_y+h > src1->getHeight() ||
		src2_x+w > src2->getWidth() || src2_y+h > src2->getHeight() ||
		trans_x+w > trans->getWidth() || trans_y+h > trans->getHeight() )
		return;

	for( int y=0; y<h; y++ ){
		UINT *pd = (UINT*)dst->getRow0( dst_y+y ) + dst_x;
		UINT *ps1 = (UINT*)src1->getRow0( src1_y+y ) + src1_x;
		UINT *ps2 = (UINT*)src2->getRow0( src2_y+y ) + src2_x;
		UINT *ts = (UINT*)trans->getRow0( trans_y+y ) + trans_x;
		for( int x=0; x<w; x++ ){
			int a = *ts&0xff;
			*pd = ( (tmin<=a && a<tmax) ? *ps2 : *ps1 ) | 0xff000000;
			pd++; ps1++; ps2++; ts++; 
		}
	}
}



#define SWAP(a,b)  do{ int t=(a); (a)=(b); (b)=t; }while(0)

void visDrawLine( IVisBitmap *bmp, int xs, int ys, int xe, int ye, COLORREF col )
{
	// note: めんどくさがって bpp==4専用コードで書いてるので注意
	if( !bmp )
		return;
	if( xs<0 || ys<0 || bmp->getWidth()<=xs || bmp->getHeight()<=ys )
		return;
	if( xe<0 || ye<0 || bmp->getWidth()<=xe || bmp->getHeight()<=ye )
		return;
	
	int step = bmp->getStep();

	if( xs == xe ){	// 垂直線
		if( ye<ys ) SWAP(ys,ye);
		UINT *p = ((UINT*)bmp->getRow0(ys)) + xs;
		UINT s4 = step>>2;
		for( int y=ys; y<=ye; y++ ){
			*p = col;
			p += s4;
		}
	}else if( ys == ye ){ //水平線
		if( xe<xs ) SWAP(xs,xe);
		UINT *p = ((UINT*)bmp->getRow0(ys)) + xs;
		for( int x=xs; x<=xe; x++ ){
			*p++ = col;
		}
	}else{ // 斜め
		// TODO : デバッグしてないの。
		int dx = abs(xe-xs);
		int dy = abs(ye-ys);
		int dx2=dx*2;
		int dy2=dy*2;
		
		if( dx > dy ){
			if( xe<xs ){
				SWAP(xs,xe);
				SWAP(ys,ye);
			}
			UINT *p = ((UINT*)bmp->getRow0(ys)) + xs;
			int dstep = ys<ye ? step : -step;
			
			for( int e=dy, x=xs; x<=xe; x++, p+=1 ){
				*p = col;
				e+=dy2;
				if( e>=dx2 ){
					e-=dx2;
					p+=dstep;
				}
			}
		}else{
			if( ye<ys ){
				SWAP(xs,xe);
				SWAP(ys,ye);
			}
			UINT *p = ((UINT*)bmp->getRow0(ys)) + xs;
			int dstep = xs<xe ? 1 : -1;
			
			for( int e=dx, y=ys; y<=ye; y++, p+=step ){
				*p = col;
				e+=dx2;
				if( e>=dy2 ){
					e-=dy2;
					p+=dstep;
				}
			}
		}
	}
}

void visFillRect( IVisBitmap *bmp, int xs, int ys, int w, int h, COLORREF col )
{
	if( !bmp )
		return;
	if( xs<0 || ys<0 || bmp->getWidth()<(xs+w) || bmp->getHeight()<(ys+h) )
		return;
	
	for( int y=0; y<h; y++ ){
		UINT *p = ((UINT*)bmp->getRow0(ys+y)) + xs;
		for( int x=0; x<w; x++ ){
			*p++ = col;
		}
	}
}

void visDrawHBar( IVisBitmap *bmp, int xs, int ys, int level, int peak )
{
	UINT *pd = (UINT*)bmp->getRow0(ys) + xs;
	UINT step = bmp->getStep()>>2;

	UINT *pd2 = pd;
	for( int y=0; y<20; y++ ){
		*pd2 = col_mid;
		pd2 += step;
	}
	pd+=2;
	for( int x=1; x<32; x++ ){
		pd2 = pd;
		if( x<=level || x == peak ){
			for( int y=0; y<20; y++ ){
				*pd2 = col_high;
				pd2 += step;
			}
		}else{
			for( int y=0; y<20; y++ ){
				*pd2 = col_shadow;
				pd2 += step;
			}
		}
		pd+=2;
	}
}
