/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "vis/vis_c86main.h"

namespace c86ctl{

class C86CtlMainWnd
{
private:
	C86CtlMainWnd()
		: hwnd(0),
		  wm(0),
		  mainVisWnd(0),
		  hVisThread(0),
		  visThreadID(0)
	{};
	virtual ~C86CtlMainWnd(){
		if( pthis ){
			delete pthis;
		}
	};

public:
	static C86CtlMainWnd* getInstance(){
		if( !pthis ){
			pthis = new C86CtlMainWnd();
		}
		return pthis;
	};
	
public:
	int createMainWnd(LPVOID param);
	int destroyMainWnd(LPVOID param);
	
private:
	static unsigned int WINAPI threadVis(LPVOID param);
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	
private:
	int startVis();
	int stopVis();

private:
	HWND hwnd;
	NOTIFYICONDATA notifyIcon;

	c86ctl::vis::CVisManager *wm;
	c86ctl::vis::CVisC86Main *mainVisWnd;
	HANDLE hVisThread;
	UINT visThreadID;

private:
	static C86CtlMainWnd *pthis;
};

};


