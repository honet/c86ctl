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
class CVisC86Fm : public CVisWnd
{
public:
	CVisC86Fm(int idx)
		: CVisWnd()
		, id(idx)
	{
		windowWidth = 330+4;
		windowHeight = 75*5+4+15;
	};
	~CVisC86Fm(){};

public:
	virtual bool create(HWND parent = 0 ){ return false; };
	virtual void close(){};
	virtual int getId(void){ return id; };

protected:
	int id;
};

typedef std::shared_ptr<CVisC86Fm> CVisC86FmPtr;

// --------------------------------------------------------
class CVisC86OPNAFm : public CVisC86Fm
{
public:
	CVisC86OPNAFm(COPNA *chip, int id, int channel)
		: CVisC86Fm(id),
		  pOPNA(chip), ch(channel)
	{
		TCHAR str[40];
		_stprintf_s(str, _T("C86OPNAFM%d%d"), id,ch);
		windowClass = str;
		_stprintf_s(str, _T("[%d] OPNA FM CH%d PARAMETER VIEW"), id, ch+1);
		windowTitle = str;
	};
	~CVisC86OPNAFm(){};

public:
	virtual bool create(HWND parent = 0 );
	virtual void close();
	
protected:
	virtual void onPaintClient(void);

protected:
	void drawFMView( IVisBitmap *canvas, int x, int y, COPNAFmCh *pFmCh );
	void drawFMSlotView( IVisBitmap *canvas, int x, int y, COPNAFmSlot *pSlot, int slotidx );

private:
	COPNA *pOPNA;
	int ch;

protected:
	CVisKnobPtr knobAMS;
	CVisKnobPtr knobPMS;
	CVisKnobPtr knobFB;
	CVisKnobPtr knobPAN;
	
	CVisKnobPtr knobAR[4];
	CVisKnobPtr knobDR1[4];
	CVisKnobPtr knobDR2[4];
	CVisKnobPtr knobSR[4];
	CVisKnobPtr knobRR[4];
	CVisKnobPtr knobSL[4];
	CVisKnobPtr knobTL[4];
	CVisKnobPtr knobMUL[4];
	CVisKnobPtr knobDET[4];
	CVisDipSwPtr dipswAM[4];
	


//public:
//	virtual bool create( HWND parent = 0 );
//	virtual void close(void);
//	void setExMode(bool ex){ exmode = ex; };
	
//protected:
//	virtual LRESULT CALLBACK wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);
//	void OnPaint();

//	bool exmode;
};

// --------------------------------------------------------
// factory
CVisC86FmPtr visC86FmViewFactory(Chip *pchip, int id, int ch);

