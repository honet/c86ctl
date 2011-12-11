/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "opna.h"
#include "vis_c86wnd.h"
#include "vis_c86key.h"
#include "vis_c86reg.h"
#include "vis_c86fm.h"

class CVisC86Main : public CVisWnd
{
public:
	CVisC86Main()
		: CVisWnd()
		, tick(0)
	{
	};
	~CVisC86Main(){};

public:
	bool create();
	void close();

protected:
	void OnPaint();
	void OnLButtonUp( UINT flags, int mx, int my );

protected:
	virtual LRESULT CALLBACK wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);

	CVisC86Key keyWnd;
	CVisC86Reg regWnd;
	CVisC86Fm fmWnd[6];
	UINT tick;
};

