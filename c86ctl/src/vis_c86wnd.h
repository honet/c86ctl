/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

#include <map>
#include "vis_manager.h"
#include "vis_c86skin.h"
#include "vis_widget.h"

#define BORDER			2
#define BORDER_TITLE	15


class CVisManager;

class CVisWnd
{
public:
	CVisWnd();
	virtual ~CVisWnd();
	
public:
	virtual bool create( int left, int top, int width, int height, 
						 DWORD exstyle = 0, DWORD style = (WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN), HWND hParent = 0 );
	virtual void close(void);
	
public:
	void setManager(CVisManager *m){ manager = m; };
	HWND getHWND(void){ return hWnd; };

public:
	void windowToClient( POINT &pt ){
		pt.x -= BORDER;
		pt.y -= (BORDER+BORDER_TITLE);
	};
	void windowToClient( RECT &rc ){
		rc.top -= (BORDER+BORDER_TITLE);
		rc.left -= BORDER;
		rc.bottom -= (2*BORDER+BORDER_TITLE);
		rc.right -= (2*BORDER);
	};

	void setCapture(CVisWidget *w){
		::SetCapture(hWnd);
		wcapture = w;
	};
	void releaseCapture(){
		wcapture = NULL;
		::ReleaseCapture();
	};
	
protected:
	// message handler
	virtual void onCreate(){};
	virtual void onDestroy(){};
	virtual void onPaint();
	virtual void onPaintClient(){};
	
protected:
	virtual LRESULT CALLBACK wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);
	bool isClose( int a, int b ) const;

protected:
	HWND hWnd;
	//CComQIPtr<ID2D1HwndRenderTarget> d2dTarget;
	
	tstring windowClass;
	tstring windowTitle;
	int windowWidth;
	int windowHeight;
//	TCHAR className[64];
//	TCHAR clientClassName[64];

	CVisManager *manager;
	CVisBitmap *canvas;
	CVisChildBitmap *clientCanvas;

	std::list< std::shared_ptr<CVisWidget> > widgets;
	

private:
	CVisWidget *wcapture;
	
private: // CVisWndÇ∆HWNDÇÃëŒâûïtÇØóp
	static LRESULT CALLBACK wndProcDispatcher(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);
	static std::map< HWND, CVisWnd* > wndMap;
	static CVisWnd* creatingWnd;
	static HANDLE hCreatingMutex;


protected:
	// å„Ç≈è¡Ç∑
	HBITMAP hSkinBMP, hSkinOldBMP;
	HBITMAP hSkinMaskBMP, hSkinMaskOldBMP;
	HDC hSkinDC;
	HDC hSkinMaskDC;
};

