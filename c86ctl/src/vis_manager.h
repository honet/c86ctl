/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include <d2d1.h>
#include <d2d1helper.h>
#include <atlbase.h>
#include <vector>
#include "vis_c86skin.h"
#include "fpscounter.h"

class CVisWnd;

class CVisManager
{
public:
	CVisManager(void) : fps(0) {
		gVisSkin.init();
	};
	
	virtual ~CVisManager(void){
		gVisSkin.deinit();
	};

	void draw(void);
	void add( CVisWnd *wnd );
	void del( CVisWnd *wnd );

	double getCurrentFPS(){
		return fps;
	};

protected:
	std::vector<CVisWnd*> clients;
	
	FPSCounter counter;
	double fps;
};


