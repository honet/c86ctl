/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "opna.h"
#include "vis_c86wnd.h"


// --------------------------------------------------------
class CVisC86Key : public CVisWnd
{
public:
	CVisC86Key(int idx)
		: CVisWnd()
		, id(idx)
	{
		windowWidth = 334;
		windowHeight = 530;
	};
	~CVisC86Key(){};

public:
	virtual bool create( HWND parent = 0 ){ return false; };
	virtual void close(){};
	virtual int getId(void){ return id; };

protected:
	int id;
};

typedef std::shared_ptr<CVisC86Key> CVisC86KeyPtr;


// --------------------------------------------------------
class CVisC86OPNAKey : public CVisC86Key
{
public:
	CVisC86OPNAKey(COPNA *pchip, int id)
		: CVisC86Key(id)
		, pOPNA(pchip)
	{
		TCHAR str[40];
		_stprintf_s(str, _T("C86OPNAKEY%d"), id);
		windowClass = str;
		_stprintf_s(str, _T("[%d] OPNA KEYBOARD VIEW"), id);
		windowTitle = str;
	};
	~CVisC86OPNAKey(){};

protected:
	virtual bool create( HWND parent = 0 );
	virtual void close();
	virtual void onPaintClient(void);
	
protected:
	void drawFMTrackView( IVisBitmap *canvas, int ltx, int lty, int trNo, int fmNo );
	void drawFM3EXTrackView( IVisBitmap *canvas, int ltx, int lty, int trNo, int fmNo, int exNo );
	void drawSSGTrackView( IVisBitmap *canvas, int ltx, int lty, int trNo, int ssgNo );
	void drawADPCMTrackView( IVisBitmap *canvas, int ltx, int lty, int trNo );
	void drawRhythmTrackView( IVisBitmap *canvas, int ltx, int lty, int trNo );

protected:
	CVisMuteSwPtr muteSw[14];
	CVisSoloSwPtr soloSw[14];
	
	COPNA *pOPNA;
};

// --------------------------------------------------------
// factory
CVisC86KeyPtr visC86KeyViewFactory(Chip *pchip, int id);

