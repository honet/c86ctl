/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "opna.h"
#include "vis_c86wnd.h"


class CVisC86Reg : public CVisWnd
{
public:
	CVisC86Reg()
		: CVisWnd()
		, pOPNA(NULL)
	{
	};
	~CVisC86Reg(){};

public:
	bool create(
		LPCTSTR className, LPCTSTR windowName, int left, int top, HWND parent = 0 );
	
	void close();
	void update(){
		::InvalidateRect(hWnd, NULL, FALSE);
	};

	void attach( COPNA *pOPNA ){ this->pOPNA = pOPNA; };
	COPNA* detach( void ){ return pOPNA; };

protected:
	virtual LRESULT CALLBACK wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);
	void OnPaint();
	void drawRegView( HDC hdc, HDC hSkinDC, HDC hSkinMaskDC, int ltx, int lty, const UCHAR *regval, const UCHAR *regatime );

	COPNA *pOPNA;
};

