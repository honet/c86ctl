/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "opna.h"
#include "vis_c86wnd.h"


class CVisC86Fm : public CVisWnd
{
public:
	CVisC86Fm()
		: CVisWnd()
		, exmode(0)
	{
	};
	~CVisC86Fm(){};

public:
	bool create(
		LPCTSTR className, LPCTSTR windowName, int left, int top, HWND parent = 0 );

	void close();
	void update(){
		::InvalidateRect(hWnd, NULL, FALSE);
	};
	void setExMode(bool ex){ exmode = ex; };

	void attach( COPNAFm *pOPNAFm ){ pFM = pOPNAFm; };
	COPNAFm* detach( void ){ return pFM; };
	
protected:
	virtual LRESULT CALLBACK wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);
	void OnPaint();

	bool exmode;
	COPNAFm *pFM;
};

