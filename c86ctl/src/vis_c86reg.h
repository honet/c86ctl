/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "opna.h"
#include "opm.h"
#include "opn3l.h"
#include "vis_c86wnd.h"

// --------------------------------------------------------
class CVisC86Reg : public CVisWnd
{
public:
	CVisC86Reg(int idx)
		: CVisWnd()
		, id(idx)
	{
		windowWidth = 284;
		windowHeight = 350;
	};
	~CVisC86Reg(void){};

public:
	virtual bool create( HWND parent = 0 );
	virtual void close(void);
	virtual int getId(void){ return id; };

protected:
	void drawRegView( IVisBitmap *canvas, int ltx, int lty, const UCHAR *regval, const UCHAR *regatime );

protected:
	int id;
};

typedef std::shared_ptr<CVisC86Reg> CVisC86RegPtr;


// --------------------------------------------------------
class CVisC86OPNAReg : public CVisC86Reg
{
public:
	CVisC86OPNAReg(COPNA *pchip, int id) : CVisC86Reg(id), pOPNA(pchip){
		TCHAR str[40];
		_stprintf_s(str, _T("C86OPNAREG%d"), id);
		windowClass = str;
		_stprintf_s(str, _T("[%d] OPNA REGISTER VIEW"), id);
		windowTitle = str;
	};
	~CVisC86OPNAReg(void){};

protected:
	virtual void onPaintClient(void);
	
private:
	COPNA *pOPNA;
};

// --------------------------------------------------------
class CVisC86OPMReg : public CVisC86Reg
{
public:
	CVisC86OPMReg(COPM *pchip, int id) : CVisC86Reg(id), pOPM(pchip){
		TCHAR str[40];
		_stprintf_s(str, _T("C86OPMREG%d"), id);
		windowClass = str;
		_stprintf_s(str, _T("[%d] OPM REGISTER VIEW"), id);
		windowTitle = str;
		
		windowHeight = 250;
	};
	~CVisC86OPMReg(void){};

protected:
	virtual void onPaintClient(void);
	
private:
	COPM *pOPM;
};

// --------------------------------------------------------
class CVisC86OPN3LReg : public CVisC86Reg
{
public:
	CVisC86OPN3LReg(COPN3L *pchip, int id) : CVisC86Reg(id), pOPN3L(pchip){
		TCHAR str[40];
		_stprintf_s(str, _T("C86OPN3LREG%d"), id);
		windowClass = str;
		_stprintf_s(str, _T("[%d] OPN3L REGISTER VIEW"), id);
		windowTitle = str;
	};
	~CVisC86OPN3LReg(){};

protected:
	virtual void onPaintClient(void);
	
private:
	COPN3L *pOPN3L;
};

// --------------------------------------------------------
// factory
CVisC86RegPtr visC86RegViewFactory(Chip *pchip, int id);


