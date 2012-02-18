/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <map>
#include <assert.h>
#include <tchar.h>

#include "module.h"
#include "config.h"
#include "resource.h"
#include "vis_c86sub.h"
#include "vis_c86wnd.h"
#include "vis_manager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#define MUTEX_NAME TEXT("C86WINDOW_MANAGER_MUTEX")


std::map< HWND, CVisWnd* > CVisWnd::wndMap;
CVisWnd* CVisWnd::creatingWnd;
HANDLE CVisWnd::hCreatingMutex = NULL;


CVisWnd::CVisWnd()
	: hWnd(0)
	, canvas(0)
	, clientCanvas(0)
	, wcapture(0)
	, windowWidth(0)
	, windowHeight(0)
	, windowClass(_T("C86CTL"))
	, windowTitle(_T("C86CTL"))
{
	hCreatingMutex = ::CreateMutex( NULL, FALSE, MUTEX_NAME );
};

CVisWnd::~CVisWnd()
{
	//d2dTarget.Release();
	close();
	
	if( hCreatingMutex ){
		::CloseHandle( hCreatingMutex );
		hCreatingMutex = NULL;
	}
};

bool CVisWnd::isClose( int a, int b ) const {
	const int margin = 10;
	return abs(a-b) < margin;
};

void CVisWnd::onPaint()
{
	const int maxlen = 256;
	char str[maxlen];
	
	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint(hWnd, &ps);

	::GetWindowTextA(hWnd, str, maxlen);
	gVisSkin.drawFrame( canvas, str);

	onPaintClient();
	std::for_each( widgets.begin(), widgets.end(),
				   [this](std::shared_ptr<CVisWidget> x){ x->onPaint(this->clientCanvas); } );

	::StretchDIBits(
		hdc, 0, 0, canvas->getWidth(), canvas->getHeight(),
		0, 0, canvas->getWidth(), canvas->getHeight(),
		canvas->getRow0(0), canvas->getBMPINFO(),
		DIB_RGB_COLORS, SRCCOPY );
	
	::EndPaint(hWnd, &ps);
}


LRESULT CALLBACK CVisWnd::wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	bool handled = false;
	
	switch (msg) {
	case WM_DESTROY:
		onDestroy();
		return 0;

	case WM_CREATE:
		onCreate();
		return 0;

	// mouse events
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(hWnd, &pt);
		windowToClient(pt);
		if( wcapture ){
			wcapture->onMouseEvent(msg, wp, lp);
		}else{
			for( auto it = widgets.begin(); it != widgets.end(); it++ ){
				RECT rc;
				(*it)->getWindowRect(rc);
				if( (rc.left <= pt.x) && (pt.x < rc.right) &&
					( rc.top <= pt.y ) && (pt.y < rc.bottom) ){
					(*it)->onMouseEvent(msg, wp, lp);
					handled = true;
					break;
				}
			}
		}
		break;

	case WM_PAINT:
		onPaint();
		return 0;
	case WM_ERASEBKGND:
		return 0;
	}
	
	if( !handled ){
		switch( msg ){
		case WM_RBUTTONUP:
			break;
		case WM_LBUTTONDOWN:
			PostMessage(hWnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, lp);
			break;
#if 0
		case WM_MOVING:
			{
				std::map< HWND, CVisWnd* >::iterator it;
				RECT *prc = reinterpret_cast<LPRECT>(lp);
				if( ::GetAsyncKeyState(VK_SHIFT) < 0 )
					break;

				for( it=frameWndMap.begin(); it!=frameWndMap.end(); it++ ){
					if( it->first == hWnd ) continue;
					::GetWindowRect( it->first, &rc );


					if( isClose( prc->left, rc.right ) ){
						if( isClose( prc->top, rc.top ) ){ // 右上-左上
							::OffsetRect( prc, rc.right-prc->left, rc.top-prc->top );
						}else if( isClose( prc->bottom, rc.bottom ) ){ // 右下-左下
							::OffsetRect( prc, rc.right-prc->left, rc.bottom-prc->bottom );
						}else if( rc.top <= prc->top && prc->top <= rc.bottom ){
							::OffsetRect( prc, rc.right-prc->left, 0 );
						}
					}else if( isClose( prc->right, rc.left ) ){
						if( isClose( prc->top, rc.top ) ){ // 左上-右上
							::OffsetRect( prc, rc.left-prc->right, rc.top-prc->top );
						}else if( isClose( prc->bottom, rc.bottom ) ){ // 左下-右下
							::OffsetRect( prc, rc.left-prc->right, rc.bottom-prc->bottom );
						}else if( rc.top <= prc->top && prc->top <= rc.bottom ){
							::OffsetRect( prc, rc.left-prc->right, 0 );
						}
					}
					else if( isClose( prc->left, rc.left ) ){
						if( isClose( prc->top, rc.bottom ) ){
							::OffsetRect( prc, rc.left-prc->left, rc.bottom-prc->top );
						}else if( isClose( prc->bottom, rc.top ) ){
							::OffsetRect( prc, rc.left-prc->left, rc.top-prc->bottom );
						}
					}
					else if( isClose( prc->right, rc.right ) ){
						if( isClose( prc->top, rc.bottom ) ){
							::OffsetRect( prc, rc.right-prc->right, rc.bottom-prc->top );
						}else if( isClose( prc->bottom, rc.top ) ){
							::OffsetRect( prc, rc.right-prc->right, rc.top-prc->bottom );
						}
					}
				}
			}
#endif
			break;

		default:
			return DefWindowProc(hWnd , msg , wp , lp);
		}
	}
	return 0;
}

LRESULT CALLBACK CVisWnd::wndProcDispatcher(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	auto it = wndMap.find( hWnd );
	if( it != wndMap.end() )
		return it->second->wndProc(hWnd,msg,wp,lp);
	else if( creatingWnd ){
		wndMap.insert( std::pair<HWND,CVisWnd*>(hWnd, creatingWnd) );
		return creatingWnd->wndProc(hWnd,msg,wp,lp);
	}

	assert(0);
	return -1;
}



bool CVisWnd::create( DWORD exstyle, DWORD style, HWND hParent )
{
	WNDCLASS winc;

	int left = gConfig.getInt( windowClass.c_str(), INIKEY_WNDLEFT, INT_MIN );
	int top  = gConfig.getInt( windowClass.c_str(), INIKEY_WNDTOP,  INT_MIN );

	if( left == INT_MIN || top == INT_MIN ){
		int wx = GetSystemMetrics(SM_CXSCREEN);
		int wy = GetSystemMetrics(SM_CYSCREEN);
		left = (wx - windowWidth) / 2;
		top  = (wy - windowHeight) / 2;
	}

	canvas = new CVisBitmap( windowWidth, windowHeight );
	clientCanvas = new CVisChildBitmap( canvas, 2, 17, windowWidth-4, windowHeight-19 );

	HINSTANCE hinst = getModuleHandle();

	winc.style			= CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc	= wndProcDispatcher;
	winc.cbClsExtra		= winc.cbWndExtra = 0;
	winc.hInstance		= hinst;
	winc.hIcon			= LoadIcon(NULL , IDI_APPLICATION);
	winc.hCursor		= LoadCursor(NULL , IDC_ARROW);
	winc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winc.lpszMenuName	= NULL;
	winc.lpszClassName	= windowClass.c_str();

	if (!RegisterClass(&winc))
		return false;

	::WaitForSingleObject( hCreatingMutex, INFINITE );
	creatingWnd = this;
	hWnd = CreateWindowEx(
		exstyle,
		windowClass.c_str(), windowTitle.c_str(),
		(style&~WS_VISIBLE),
		left, top, windowWidth, windowHeight,
		hParent, NULL, hinst, NULL
	);

	creatingWnd = NULL;
	::ReleaseMutex( hCreatingMutex );
	if(!hWnd) return false;

	return true;
}

void CVisWnd::onCreate()
{
}

void CVisWnd::onDestroy()
{
	saveConfig();
}
void CVisWnd::onPaintClient()
{
}

void CVisWnd::close()
{
	if(hWnd){
		DestroyWindow( hWnd );
		wndMap.erase( hWnd );
		hWnd = NULL;
	}
	if( clientCanvas ){
		delete clientCanvas;
		clientCanvas = NULL;
	}
	if( canvas ){
		delete canvas;
		canvas = NULL;
	}
	::UnregisterClass( windowClass.c_str(), getModuleHandle() );
}

void CVisWnd::saveConfig(void)
{
	RECT rc;
	HWND hwnd = getHWND();
	if( hwnd ){
		if( ::GetWindowRect( hwnd, &rc ) ){
			gConfig.writeInt( windowClass.c_str(), INIKEY_WNDLEFT, rc.left );
			gConfig.writeInt( windowClass.c_str(), INIKEY_WNDTOP, rc.top );
		}
		//gConfig.writeInt( windowClass.c_str(), INIKEY_WNDVISIBLE, 1 );
	}
}

