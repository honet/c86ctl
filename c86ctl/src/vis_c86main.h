/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "opna.h"
#include "vis_c86wnd.h"
#include "vis_c86key.h"
#include "vis_c86reg.h"
#include "vis_c86fm.h"
#include "vis_widget.h"

#include "if.h"
#include "if_gimic_hid.h"
#include "if_gimic_midi.h"


class CVisC86Main : public CVisWnd
{
public:
	CVisC86Main()
		: CVisWnd(),
		  tick(0)
	{
		windowClass = TEXT("C86CTL");
		windowTitle = TEXT("C86 CONTROL");
	};
	~CVisC86Main(){};

public:
	void attach( std::vector< std::shared_ptr<GimicIF> > &g );
	void detach( void );
	void updateInfo(void);

	bool create(void);
	void close(void);
	void saveConfig(void);

protected:
	// message handler
	virtual void onPaintClient();
	
protected:
	std::vector< std::shared_ptr<GimicIF> > gimic;
	struct hwinfo{
		Devinfo mbinfo;
		Devinfo chipinfo;
		ChipType chiptype;
		UINT major, minor, rev, build;
		UINT clock;

		CVisCheckBoxPtr checkKey;
		CVisCheckBoxPtr checkReg;
		CVisCheckBoxPtr checkFM[9];
		CVisCheckBoxPtr checkSSG;
		CVisCheckBoxPtr checkRHYTHM;
		CVisCheckBoxPtr checkADPCM;

		CVisC86RegPtr regView;
		CVisC86KeyPtr keyView;
		CVisC86FmPtr fmView[9];
	};
	std::vector<hwinfo> info;

	UINT tick;
};

