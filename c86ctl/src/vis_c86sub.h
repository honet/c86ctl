/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
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


// bitmap operations ------------------------------------------------
void blt( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
		  IVisBitmap *src, int src_x, int src_y );
void alphablt( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
			   IVisBitmap *src, int src_x, int src_y );
void transblt( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
			   IVisBitmap *src1, int src1_x, int src1_y,
			   IVisBitmap *src2, int src2_x, int src2_y,
			   IVisBitmap *trans, int trans_x, int trans_y, int t );
void transblt2( IVisBitmap *dst, int dst_x, int dst_y, int w, int h,
			   IVisBitmap *src1, int src1_x, int src1_y,
			   IVisBitmap *src2, int src2_x, int src2_y,
			   IVisBitmap *trans, int trans_x, int trans_y, int tmin, int tmax );

void visDrawLine( IVisBitmap *bmp, int xs, int ys, int xe, int ye, COLORREF col );
void visFillRect( IVisBitmap *bmp, int xs, int ys, int w, int h, COLORREF col );


