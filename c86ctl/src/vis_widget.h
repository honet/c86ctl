/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include <functional>
#include "vis_c86sub.h"
#include "vis_c86skin.h"
#include "vis_c86wnd.h"


// ---------------------------------------------------------------------------
class CVisWidget
{
public:
	CVisWidget(CVisWnd *parentWnd)
		: parent(parentWnd), sx(0), sy(0), ex(0), ey(0){};
	~CVisWidget(){};
	
public:
	// message handler
	virtual void onPaint(IVisBitmap *canvas){};
	virtual void onMouseEvent(UINT msg, WPARAM wp, LPARAM lp){};
	virtual void getWindowRect(RECT &rc){
		rc.left = sx;
		rc.top = sy;
		rc.right = ex;
		rc.bottom = ey;
	};

protected:
	int sx, sy, ex, ey;
	CVisWnd *parent;
};


// ---------------------------------------------------------------------------
// checkbox
class CVisCheckBox : public CVisWidget
{
public:
	CVisCheckBox( CVisWnd *parentWnd, int x, int y, char *title )
		: CVisWidget(parentWnd), sw(0), str(title){
		sx = x;
		sy = y;
		ex = x + 10 + 6*str.length();
		ey = y + 8;
	};
	~CVisCheckBox(){};

public:
	virtual void onPaint(IVisBitmap *canvas);
	virtual void onMouseEvent(UINT msg, WPARAM wp, LPARAM lp);

public:
	int getValue(){ return sw; };

public:
	std::list< std::function< void(CVisWidget*) > > changeEvent;
	
protected:
	int sw;
	std::string str;
};

typedef std::shared_ptr<CVisCheckBox> CVisCheckBoxPtr;


// ---------------------------------------------------------------------------
// DIPSW
class CVisDipSw : public CVisWidget
{
public:
	CVisDipSw( CVisWnd *parentWnd, int x, int y )
		: CVisWidget(parentWnd), sw(0){
			sx = x;
			sy = y;
			ex = x+11;
			ey = y+20;
		};
	~CVisDipSw(){};

public:
	virtual void onPaint(IVisBitmap *canvas);
	virtual void onMouseEvent(UINT msg, WPARAM wp, LPARAM lp);
	
public:
	std::list< std::function< void(CVisWidget*) > > changeEvent;
	
protected:
	int sw;
};

typedef std::shared_ptr<CVisDipSw> CVisDipSwPtr;

// ---------------------------------------------------------------------------
// knob
class CVisKnob : public CVisWidget
{
public:
	CVisKnob( CVisWnd *parentWnd, int x, int y )
		: CVisWidget(parentWnd), minval(0), maxval(127),
		  bMoving(false){
		sx = x;
		sy = y;
		ex = sx+22;
		ey = sy+22;
	};
	~CVisKnob(){};

public:
	virtual void onPaint(IVisBitmap *canvas);
	virtual void onMouseEvent(UINT msg, WPARAM wp, LPARAM lp);

	void setRange(int minv, int maxv){
		minval = minv;
		maxval = maxv;
	};
//	void setValue(int val){
//		if( minval <= val && val <= maxval ){
//			value = val;
//		}
//	}
	
public:
	std::list< std::function< void(CVisWidget*) > > changeEvent;
	std::function< int() > getter;
	std::function< void(int) > setter;
	
protected:
//	int value;
	int maxval;
	int minval;
	bool bMoving;
	POINT spos;
	int svalue;
};

typedef std::shared_ptr<CVisKnob> CVisKnobPtr;

// ---------------------------------------------------------------------------
// keyboard
//class CVisKeyboard : public CVisWidget{
//};





// ---------------------------------------------------------------------------
// support macro.
#define HANDLER_DELEGATE( method )  [this](CVisWidget* s){ this-> ## method (s); }
#define ADD_HANDLER( e, method )  e ## .push_back( [this](CVisWidget* s){ this-> ## method (s); } );

