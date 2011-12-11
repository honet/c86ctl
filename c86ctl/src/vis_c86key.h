/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "opna.h"
#include "vis_c86wnd.h"


class CVisC86Key : public CVisWnd
{
public:
	CVisC86Key()
		: CVisWnd()
		, pOPNA(NULL)
	{
	};
	~CVisC86Key(){};

public:
	bool create( LPCTSTR className, LPCTSTR windowName, int left, int top, HWND parent = 0 );

	void close();
	void update(){
		::InvalidateRect(hWnd, NULL, FALSE);
	};

	void attach( COPNA *pOPNA ){ this->pOPNA = pOPNA; };
	COPNA* detach( void ){ return pOPNA; };
	
protected:
	virtual LRESULT CALLBACK wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);
	void OnPaint();
	void drawFMTrackView( HDC hdc, int ltx, int lty, int trNo, int fmNo );
	void drawFM3EXTrackView( HDC hdc, int ltx, int lty, int trNo, int fmNo, int exNo );
	void drawSSGTrackView( HDC hdc, int ltx, int lty, int trNo, int ssgNo );
	void drawADPCMTrackView( HDC hdc, int ltx, int lty, int trNo );
	void drawRhythmTrackView( HDC hdc, int ltx, int lty, int trNo );

protected:
	COPNA *pOPNA;
};

