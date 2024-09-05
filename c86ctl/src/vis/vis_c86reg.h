/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "chip/opna.h"
#include "chip/opm.h"
#include "chip/opn3l.h"
#include "chip/opl3.h"
#include "chip/opll.h"
#include "chip/tms3631.h"
#include "chip/generic.h"
#include "vis_c86wnd.h"

namespace c86ctl{
namespace vis {

// --------------------------------------------------------
class CVisC86Reg : public CVisWnd
{
public:
	CVisC86Reg(int idx)
		: CVisWnd()
		, id(idx)
	{
		_windowWidth = 284;
		_windowHeight = 350;
	}
	~CVisC86Reg(void){}

public:
	virtual bool create(HWND parent = 0);
	virtual void close(void);
	virtual int getId(void) { return id; }

protected:
	void drawRegView(IVisBitmap *canvas, int ltx, int lty, const UCHAR *regval, const UCHAR *regatime, int ynum = 16);

protected:
	int id;
	int _windowWidth;
	int _windowHeight;
};

typedef std::shared_ptr<CVisC86Reg> CVisC86RegPtr;


// --------------------------------------------------------
class CVisC86OPNAReg : public CVisC86Reg
{
public:
	CVisC86OPNAReg(COPNA* pchip, int id) : CVisC86Reg(id), pOPNA(pchip) {
		TCHAR str[40];
		_stprintf_s(str, _countof(str), _T("C86OPNAREG%d"), id);
		windowClass = str;
		_stprintf_s(str, _countof(str), _T("[%d] OPNA REGISTER VIEW"), id);
		windowTitle = str;
	}
	~CVisC86OPNAReg(void) {}

protected:
	virtual void onPaintClient(void);

private:
	COPNA* pOPNA;
};

// --------------------------------------------------------
class CVisC86OPMReg : public CVisC86Reg
{
public:
	CVisC86OPMReg(COPM* pchip, int id) : CVisC86Reg(id), pOPM(pchip) {
		TCHAR str[40];
		_stprintf_s(str, _countof(str), _T("C86OPMREG%d"), id);
		windowClass = str;
		_stprintf_s(str, _countof(str), _T("[%d] OPM REGISTER VIEW"), id);
		windowTitle = str;

		_windowHeight = 180;
	}
	~CVisC86OPMReg(void) {}

protected:
	virtual void onPaintClient(void);

private:
	COPM* pOPM;
};

// --------------------------------------------------------
class CVisC86OPN3LReg : public CVisC86Reg
{
public:
	CVisC86OPN3LReg(COPN3L* pchip, int id) : CVisC86Reg(id), pOPN3L(pchip) {
		TCHAR str[40];
		_stprintf_s(str, _countof(str), _T("C86OPN3LREG%d"), id);
		windowClass = str;
		_stprintf_s(str, _countof(str), _T("[%d] OPN3L REGISTER VIEW"), id);
		windowTitle = str;
	}
	~CVisC86OPN3LReg() {}

protected:
	virtual void onPaintClient(void);

private:
	COPN3L* pOPN3L;
};

// --------------------------------------------------------
class CVisC86OPL3Reg : public CVisC86Reg
{
public:
	CVisC86OPL3Reg(COPL3* pchip, int id) : CVisC86Reg(id), pOPL3(pchip) {
		TCHAR str[40];
		_stprintf_s(str, _countof(str), _T("C86OPL3REG%d"), id);
		windowClass = str;
		_stprintf_s(str, _countof(str), _T("[%d] OPL3 REGISTER VIEW"), id);
		windowTitle = str;
	}
	~CVisC86OPL3Reg() {}

protected:
	virtual void onPaintClient(void);

private:
	COPL3* pOPL3;
};

// --------------------------------------------------------
class CVisC86OPLLReg : public CVisC86Reg
{
public:
	CVisC86OPLLReg(COPLL* pchip, int id) : CVisC86Reg(id), pOPLL(pchip) {
		TCHAR str[40];
		_stprintf_s(str, _countof(str), _T("C86OPLLREG%d"), id);
		windowClass = str;
		_stprintf_s(str, _countof(str), _T("[%d] OPLL REGISTER VIEW"), id);
		windowTitle = str;
	}
	~CVisC86OPLLReg() {}

protected:
	virtual void onPaintClient(void);

private:
	COPLL* pOPLL;
};

// --------------------------------------------------------
class CVisC86TMS3631Reg : public CVisC86Reg
{
public:
	CVisC86TMS3631Reg(CTMS3631* pchip, int id) : CVisC86Reg(id), pChip(pchip) {
		TCHAR str[40];
		_stprintf_s(str, _countof(str), _T("C86 TMS3631 REG%d"), id);
		windowClass = str;
		_stprintf_s(str, _countof(str), _T("[%d] REGISTER VIEW"), id);
		windowTitle = str;

		_windowWidth = 284;
		_windowHeight = 64;
	}
	~CVisC86TMS3631Reg() {}

protected:
	virtual void onPaintClient(void);

private:
	CTMS3631* pChip;
};

// --------------------------------------------------------
class CVisC86Generic1Reg : public CVisC86Reg
{
public:
	CVisC86Generic1Reg(CGenericChipBank1* pchip, int id) : CVisC86Reg(id), pChip(pchip) {
		TCHAR str[40];
		_stprintf_s(str, _countof(str), _T("C86 GENERIC1REG%d"), id);
		windowClass = str;
		_stprintf_s(str, _countof(str), _T("[%d] REGISTER VIEW"), id);
		windowTitle = str;

		_windowWidth = 284;
		_windowHeight = 172;
	}
	~CVisC86Generic1Reg() {}

protected:
	virtual void onPaintClient(void);

private:
	CGenericChipBank1* pChip;
};

// --------------------------------------------------------
class CVisC86Generic2Reg : public CVisC86Reg
{
public:
	CVisC86Generic2Reg(CGenericChipBank2* pchip, int id) : CVisC86Reg(id), pChip(pchip) {
		TCHAR str[40];
		_stprintf_s(str, _countof(str), _T("C86 GENERIC2REG%d"), id);
		windowClass = str;
		_stprintf_s(str, _countof(str), _T("[%d] REGISTER VIEW"), id);
		windowTitle = str;
	}
	~CVisC86Generic2Reg() {}

protected:
	virtual void onPaintClient(void);

private:
	CGenericChipBank2* pChip;
};

// --------------------------------------------------------
// factory
CVisC86RegPtr visC86RegViewFactory(Chip *pchip, int id);


}
}
