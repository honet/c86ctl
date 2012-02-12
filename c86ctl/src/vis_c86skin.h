/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "vis_bitmap.h"

//#define RGBtoBGRA(x) 
#define ARGB(a,r,g,b)  ( (UINT)(b) | (UINT)(g) << 8 | (UINT)(r) << 16 | (UINT)(a) << 24 )

#define col_light	ARGB(0,199,200,255)
#define col_shadow	ARGB(0,43,44,75)
#define col_mid		ARGB(0,68,68,119)
#define col_high	ARGB(0,128,128,255)

class CVisC86Skin
{
public:
	CVisC86Skin();
	~CVisC86Skin();

public:
	void init(void);
	void deinit(void);
	
	void drawStr( IVisBitmap *canvas, int type, int x, int y, const char *str );
	void drawVStr( IVisBitmap *canvas, int type, int x, int y, const char *str );
	void drawNanoNumStr( IVisBitmap *canvas, int x, int y, const char *str );
	void drawNumStr1( IVisBitmap *canvas, int x, int y, const char *str );
	void drawFrame( IVisBitmap *canvas, CHAR *title );
	
	void drawKeyboard( IVisBitmap *canvas, int x, int y );
	void drawDarkKeyboard( IVisBitmap *canvas, int x, int y );
	void drawHilightKey( IVisBitmap *canvas, int x, int y, int oct, int note );
	void drawTickCircle( IVisBitmap *canvas, int x, int y, int tick );
	void drawLogo(IVisBitmap *canvas, int x, int y );
	void drawDipSw(IVisBitmap *canvas, int x, int y, int sw );
	void drawCheckBox(IVisBitmap *canvas, int x, int y, int sw );
	void drawKnob(IVisBitmap *canvas, int x, int y, int minval, int maxval, int numval );
	void drawMuteSw(IVisBitmap *canvas, int x, int y, int sw );
	void drawSoloSw(IVisBitmap *canvas, int x, int y, int sw );
	
	void drawFMAlgorithm(IVisBitmap *canvas, int x, int y, int no );
	void drawFMSkin(IVisBitmap *canvas, int x, int y, int exmode );
	void drawFMSlotSkin(IVisBitmap *canvas, int x, int y );
	void drawFMSlotLight(IVisBitmap *canvas, int x, int y, int no, int sw );
	
/*
	bool hittestButton(int x, int y, int id, int mx, int my );
	void drawFrame(HWND hwnd, CHAR *title );
	void fillRect(HDC hdc, COLORREF col, RECT *rc);
*/
//	void drawFMView( IVisBitmap &canvas, int x, int y, COPNAFm *pFM );
//	void drawFMSlotView( IVisBitmap &canvas, int x, int y, COPNAFmSlot *pFM, int slotidx );
/*
protected:
	HBITMAP hSkinBMP, hSkinOldBMP;
	HBITMAP hSkinMaskBMP, hSkinMaskOldBMP;
*/
	
protected:
	CVisBitmap *skinbmp;
};

extern CVisC86Skin gVisSkin;

