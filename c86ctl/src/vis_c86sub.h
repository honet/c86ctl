/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once
#include "opna.h"
#include "vis_bitmap.h"


#define BUTTON_PLAY		0
#define BUTTON_STOP		1
#define BUTTON_PAUSE	2
#define BUTTON_FADE		3
#define BUTTON_BUSY		4
#define BUTTON_KEY		5
#define BUTTON_REG		6
#define BUTTON_LEVEL	7
#define BUTTON_FM1		8
#define BUTTON_FM2		9
#define BUTTON_FM3		10
#define BUTTON_FM4		11
#define BUTTON_FM5		12
#define BUTTON_FM6		13
#define BUTTON_SSG1		14
#define BUTTON_SSG2		15
#define BUTTON_SSG3		16
#define BUTTON_ADPCM	17


void vis_draw_str( HDC hdc, HDC hskin, HDC hmask, int type, int x, int y, const char *str );
void vis_draw_vstr( HDC hdc, HDC hskin, HDC hmask, int type, int x, int y, const char *str );
void vis_draw_numstr1( HDC hdc, HDC hskin, HDC hmask, int x, int y, const char *str );
void vis_draw_keyboard( HDC hdc, HDC hskin, HDC hmask, int x, int y );
void vis_draw_dark_keyboard( HDC hdc, HDC hskin, HDC hmask, int x, int y );
void vis_draw_hilight_key( HDC hdc, HDC hskin, HDC hmask, int x, int y, int oct, int note );
void vis_draw_tickcircle( HDC hdc, HDC hskin, HDC hmask, int x, int y, int tick );
void vis_draw_logo(HDC hdc, HDC hskin, HDC hmask, int x, int y );
void vis_draw_button(HDC hdc, HDC hskin, HDC hmask, int x, int y, int id, int sw );
bool vis_hittest_button(int x, int y, int id, int mx, int my );
void vis_draw_frame(HWND hwnd, HDC hdc, HDC hskin, HDC hmask, CHAR *title );
void vis_fill_rect(HDC hdc, COLORREF col, RECT *rc);

void vis_draw_fm_view( HDC hdc, HDC hskin, HDC hmask, int x, int y, COPNAFm *pFM );
void vis_draw_fmslot_view( HDC hdc, HDC hskin, HDC hmask, int x, int y, COPNAFmSlot *pFM, int slotidx );

// bitmap operations ------------------------------------------------
void blt( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
		  IVisBitmap *src, int src_x, int src_y );
void alphablt( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
			   IVisBitmap *src, int src_x, int src_y );
void transblt( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
			   IVisBitmap *src1, int src1_x, int src1_y,
			   IVisBitmap *src2, int src2_x, int src2_y,
			   IVisBitmap *trans, int trans_x, int trans_y, int t );

void visDrawLine( IVisBitmap *bmp, int xs, int ys, int xe, int ye, COLORREF col );
void visFillRect( IVisBitmap *bmp, int xs, int ys, int w, int h, COLORREF col );

