/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "opna.h"
#include "opn3l.h"
#include "opm.h"
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
	virtual int getId(void){ return id; };


protected:
	bool createFmView(COPNFmCh *pFmCh);
	void drawFMView( IVisBitmap *canvas, int x, int y, COPNFmCh *pFmCh );
	void drawFMSlotView( IVisBitmap *canvas, int x, int y, COPNFmSlot *pSlot, int slotidx );
	
protected:
	int id;
	
protected:
	CVisKnobPtr knobAMS;
	CVisKnobPtr knobPMS;
	CVisKnobPtr knobFB;
	CVisKnobPtr knobPAN;
	
	CVisKnobPtr knobAR[4];
	CVisKnobPtr knobDR[4];
	CVisKnobPtr knobSR[4];
	CVisKnobPtr knobRR[4];
	CVisKnobPtr knobSL[4];
	CVisKnobPtr knobTL[4];
	CVisKnobPtr knobMUL[4];
	CVisKnobPtr knobDET[4];
	CVisKnobPtr knobDET2[4];
	CVisDipSwPtr dipswAM[4];
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
	
protected:
	virtual void onPaintClient(void);

private:
	COPNA *pOPNA;
	int ch;


};

// --------------------------------------------------------
class CVisC86OPN3LFm : public CVisC86Fm
{
public:
	CVisC86OPN3LFm(COPN3L *chip, int id, int channel)
		: CVisC86Fm(id),
		  pOPN3L(chip), ch(channel)
	{
		TCHAR str[40];
		_stprintf_s(str, _T("C86OPN3LFM%d%d"), id,ch);
		windowClass = str;
		_stprintf_s(str, _T("[%d] OPN3L FM CH%d PARAMETER VIEW"), id, ch+1);
		windowTitle = str;
	};
	~CVisC86OPN3LFm(){};

public:
	virtual bool create(HWND parent = 0 );
	
protected:
	virtual void onPaintClient(void);
	
private:
	COPN3L *pOPN3L;
	int ch;
};

// --------------------------------------------------------
class CVisC86OPMFm : public CVisC86Fm
{
public:
	CVisC86OPMFm(COPM *chip, int id, int channel)
		: CVisC86Fm(id),
		  pOPM(chip), ch(channel)
	{
		TCHAR str[40];
		_stprintf_s(str, _T("C86OPMFM%d%d"), id,ch);
		windowClass = str;
		_stprintf_s(str, _T("[%d] OPNM FM CH%d PARAMETER VIEW"), id, ch+1);
		windowTitle = str;
	};
	~CVisC86OPMFm(){};

public:
	virtual bool create(HWND parent = 0 );
	
protected:
	virtual void onPaintClient(void);
	
private:
	COPM *pOPM;
	int ch;
};

// --------------------------------------------------------
// factory
CVisC86FmPtr visC86FmViewFactory(Chip *pchip, int id, int ch);

