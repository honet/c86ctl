/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

void initializeWndManager();
void uninitializeWndManager();


class CVisWnd
{
public:
	CVisWnd(){
		hFrameWnd = NULL;
		hWnd = NULL;
		hSkinBMP = NULL;
		hSkinMaskBMP = NULL;
		hSkinDC = NULL;
		hSkinMaskDC = NULL;
		hSkinOldBMP = NULL;
		hSkinMaskOldBMP = NULL;
	};
	virtual ~CVisWnd(){
		close();
	};

public:
	virtual bool create( 
		LPCTSTR className, LPCTSTR windowName, int left, int top, int width, int height, 
		DWORD exstyle = 0, DWORD style = (WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN), HWND hParent = 0 );
	virtual void close(void);
	virtual HWND getFrameHWND(void){ return hFrameWnd; };
	
	virtual LRESULT CALLBACK wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);
	virtual LRESULT CALLBACK frameWndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp);

protected:
	bool isClose( int a, int b ) const;

protected:
	HWND hFrameWnd;
	HWND hWnd;
	HBITMAP hSkinBMP, hSkinOldBMP;
	HBITMAP hSkinMaskBMP, hSkinMaskOldBMP;
	
	HDC hSkinDC;
	HDC hSkinMaskDC;
	TCHAR className[64];
	TCHAR clientClassName[64];
};

