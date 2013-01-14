/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include "chip/opna.h"
#include "vis_c86wnd.h"
#include "vis_c86key.h"
#include "vis_c86reg.h"
#include "vis_c86fm.h"
#include "vis_widget.h"

#include "interface/if.h"
#include "interface/if_gimic_hid.h"
#include "interface/if_gimic_midi.h"

namespace c86ctl{
namespace vis {


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
	virtual bool create(void);
	bool update(void);

protected:
	// message handler
	virtual void onPaintClient();
	
protected:
	//std::vector< std::shared_ptr<GimicIF> > gimic;
	struct hwinfo{
		Devinfo mbinfo;
		Devinfo chipinfo;
		ChipType chiptype;
		UINT major, minor, rev, build;

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

};
};

