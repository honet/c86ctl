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
//		::D2D1CreateFactory( D2D1_FACTORY_TYPE_MULTI_THREADED, &d2dFactory );
	};
	
	virtual ~CVisManager(void){
		gVisSkin.deinit();
//		d2dFactory.Release();
	};

	void draw(void);
	void add( CVisWnd *wnd );
	void del( CVisWnd *wnd );

	double getCurrentFPS(){
		return fps;
	};

//	CComQIPtr<ID2D1Factory> getD2DFactory() { return d2dFactory; };
//	std::shared_ptr<CVisC86Skin> getSkin(){ return skin; };

protected:
//	CComQIPtr<ID2D1Factory> d2dFactory;
	std::vector<CVisWnd*> clients;
	
	FPSCounter counter;
	double fps;
};


