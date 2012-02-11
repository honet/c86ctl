/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"

#include "vis_c86wnd.h"
#include "vis_widget.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

// --------------------------------------------------------
void CVisCheckBox::onPaint(IVisBitmap *canvas){
	gVisSkin.drawCheckBox( canvas, sx, sy, sw );
	gVisSkin.drawStr( canvas, sw?1:0, sx+10, sy, str.c_str() );
}
	
void CVisCheckBox::onMouseEvent(UINT msg, WPARAM wp, LPARAM lp){
	switch(msg){
	case WM_LBUTTONDOWN:
		sw ^= 1;
		std::for_each( changeEvent.begin(), changeEvent.end(), [this](std::function<void(CVisWidget*)> h){ h(this); } );
		break;
	}
}

// --------------------------------------------------------
void CVisDipSw::onPaint(IVisBitmap *canvas){
	gVisSkin.drawDipSw( canvas, sx, sy, sw );
};
	
void CVisDipSw::onMouseEvent(UINT msg, WPARAM wp, LPARAM lp){
	switch(msg){
	case WM_LBUTTONDOWN:
		sw ^= 1;
		std::for_each( changeEvent.begin(), changeEvent.end(), [this](std::function<void(CVisWidget*)> h){ h(this); } );
		break;
	}
};

// --------------------------------------------------------
void CVisKnob::onPaint(IVisBitmap *canvas){
	int value = 0;
	if( getter ) value = getter();
	gVisSkin.drawKnob( canvas, sx, sy, (255*(value-minval))/(maxval-minval), value );
}

void CVisKnob::onMouseEvent(UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg){
	case WM_MOUSEMOVE:
		if( bMoving ){
			POINT cpos;
			::GetCursorPos(&cpos);
			int dx = (int)cpos.x - spos.x;
			int dy = (int)cpos.y - spos.y;
			int xvalue = svalue + (maxval - minval)*dx/512;
			if( maxval < xvalue ) xvalue = maxval;
			if( xvalue < minval ) xvalue = minval;
			if( setter ) setter(xvalue);
			std::for_each( changeEvent.begin(), changeEvent.end(), [this](std::function<void(CVisWidget*)> h){ h(this); } );
		}
		break;
	case WM_LBUTTONDOWN:
		::GetCursorPos(&spos);
		if( getter ) svalue = getter();
		bMoving = true;
		parent->setCapture(this);
		break;
	case WM_LBUTTONUP:
		bMoving = false;
		parent->releaseCapture();
		break;
	}
}

