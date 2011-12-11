/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include "module.h"
#include "resource.h"
#include "vis_c86sub.h"
#include "vis_c86wnd.h"
#include <map>
#include <assert.h>
#include <tchar.h>

std::map< HWND, CVisWnd* > frameWndMap;
std::map< HWND, CVisWnd* > wndMap;
CVisWnd* creatingWnd;
HANDLE hCreatingMutex = NULL;

#define MUTEX_NAME TEXT("C86WINDOW_MANAGER_MUTEX")

void initializeWndManager()
{
	if( !hCreatingMutex )
		hCreatingMutex = ::CreateMutex( NULL, FALSE, MUTEX_NAME );
}

void uninitializeWndManager()
{
	if( hCreatingMutex ){
		::CloseHandle( hCreatingMutex );
		hCreatingMutex = NULL;
	}
}

static LRESULT CALLBACK framwWndMsgDispatcher(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	std::map< HWND, CVisWnd* >::iterator it;
	it = frameWndMap.find( hWnd );
	if( it != frameWndMap.end() )
		return it->second->frameWndProc(hWnd,msg,wp,lp);
	else if( creatingWnd ){
		frameWndMap.insert( std::pair<HWND,CVisWnd*>(hWnd, creatingWnd) );
		return creatingWnd->frameWndProc(hWnd,msg,wp,lp);
	}

	assert(0);
	return -1;
};

static LRESULT CALLBACK clientWndMsgDispatcher(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	std::map< HWND, CVisWnd* >::iterator it;
	it = wndMap.find( hWnd );
	if( it != wndMap.end() )
		return it->second->wndProc(hWnd,msg,wp,lp);
	else if( creatingWnd ){
		wndMap.insert( std::pair<HWND,CVisWnd*>(hWnd, creatingWnd) );
		return creatingWnd->wndProc(hWnd,msg,wp,lp);
	}

	assert(0);
	return -1;
};

bool CVisWnd::isClose( int a, int b ) const {
	const int margin = 10;
	return abs(a-b) < margin;
};

LRESULT CALLBACK CVisWnd::frameWndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	RECT rc;
	const int maxlen = 256;
	
	switch (msg) {
	case WM_DESTROY:
		if( hSkinBMP ){
			::SelectObject(hSkinDC, hSkinOldBMP);
			::DeleteObject(hSkinBMP);
			hSkinBMP = NULL;
			hSkinOldBMP = NULL;
		}
		if( hSkinMaskBMP ){
			::SelectObject(hSkinMaskDC, hSkinMaskOldBMP);
			::DeleteObject(hSkinMaskBMP);
			hSkinMaskBMP = NULL;
			hSkinMaskOldBMP = NULL;
		}
		if( hSkinDC ){
			::DeleteDC(hSkinDC);
			hSkinDC = NULL;
		}
		if( hSkinMaskDC ){
			::DeleteDC(hSkinMaskDC);
			hSkinMaskDC = NULL;
		}
		break;
	case WM_CREATE:
		{
			BITMAP bmp;
			hSkinBMP = LoadBitmap( getModuleHandle(), MAKEINTRESOURCE(IDB_MAINSKIN) );
			::GetObject( hSkinBMP, sizeof(BITMAP), &bmp );
			hSkinMaskBMP = ::CreateBitmap( bmp.bmWidth, bmp.bmHeight, 1, 1, NULL );
				
			HDC hdc = ::GetDC(hWnd);
			hSkinDC = ::CreateCompatibleDC(hdc);
			hSkinMaskDC = ::CreateCompatibleDC(hdc);
			hSkinOldBMP = (HBITMAP)::SelectObject(hSkinDC, hSkinBMP);
			hSkinMaskOldBMP = (HBITMAP)::SelectObject(hSkinMaskDC, hSkinMaskBMP);
			::SetBkColor( hSkinDC, 0 );
			::BitBlt( hSkinMaskDC, 0, 0, bmp.bmWidth, bmp.bmHeight, hSkinDC, 0, 0, SRCCOPY );
			ReleaseDC(hWnd, hdc);
		}
		break;
	case WM_RBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		PostMessage(hWnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, lp);
		break;
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
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			char str[maxlen];
			HDC hdc = BeginPaint(hWnd, &ps);
			::GetWindowTextA(hWnd, str, maxlen);
			vis_draw_frame(hWnd, hdc, hSkinDC, hSkinMaskDC, str);
			EndPaint(hWnd, &ps);
		}
		break;
	default:
		return DefWindowProc(hWnd , msg , wp , lp);
	}
	return 0;
}

LRESULT CALLBACK CVisWnd::wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	return DefWindowProc( hWnd, msg, wp, lp );
}


bool CVisWnd::create( LPCTSTR className, LPCTSTR windowName, int left, int top, int width, int height, DWORD exstyle, DWORD style, HWND hParent )
{
	WNDCLASS winc;
	const int maxlen = 256;
	TCHAR clientWindowName[maxlen];

	if( left == INT_MIN || top == INT_MIN ){
		int wx = GetSystemMetrics(SM_CXSCREEN);
		int wy = GetSystemMetrics(SM_CYSCREEN);
		left = (wx - width) / 2;
		top  = (wy - height) / 2;
	}

	HINSTANCE hinst = getModuleHandle();
	_tcscpy( this->className, className );
	_tcscpy( clientClassName, className );
	_tcsncat( clientClassName, TEXT("CLIENT"), maxlen );
	_tcscpy( clientWindowName, windowName );
	_tcsncat( clientWindowName, TEXT("CLIENT"), maxlen );

	winc.style			= CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc	= framwWndMsgDispatcher;
	winc.cbClsExtra		= winc.cbWndExtra = 0;
	winc.hInstance		= hinst;
	winc.hIcon			= LoadIcon(NULL , IDI_APPLICATION);
	winc.hCursor		= LoadCursor(NULL , IDC_ARROW);
	winc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winc.lpszMenuName	= NULL;
	winc.lpszClassName	= className;

	if (!RegisterClass(&winc))
		return false;

	::WaitForSingleObject( hCreatingMutex, INFINITE );
	creatingWnd = this;
	hFrameWnd = CreateWindowEx(
		exstyle,
		className, windowName,
		style,
		left, top, width, height,
		hParent, NULL, hinst, NULL
	);
	creatingWnd = NULL;
	::ReleaseMutex( hCreatingMutex );
	if(!hFrameWnd) return false;


	winc.style			= CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc	= clientWndMsgDispatcher;
	winc.cbClsExtra		= winc.cbWndExtra = 0;
	winc.hInstance		= hinst;
	winc.hIcon			= LoadIcon(NULL , IDI_APPLICATION);
	winc.hCursor		= LoadCursor(NULL , IDC_ARROW);
	winc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winc.lpszMenuName	= NULL;
	winc.lpszClassName	= clientClassName;

	if (!RegisterClass(&winc))
		return false;

	left=2;
	top=17;
	width-=4;
	height-=19;
	
	::WaitForSingleObject( hCreatingMutex, INFINITE );
	creatingWnd = this;
	hWnd = CreateWindow(
		clientClassName, clientWindowName, 
		WS_CHILD | WS_VISIBLE,
		left, top, width, height,
		hFrameWnd, NULL, hinst, NULL
	);
	creatingWnd = NULL;
	::ReleaseMutex( hCreatingMutex );
	if( !hWnd ) return false;
	//ShowWindow(hWnd, SW_SHOW);
	
	return true;
}

void CVisWnd::close()
{
	if(hWnd){
		DestroyWindow( hWnd );
		wndMap.erase( hWnd );
		hWnd = NULL;
	}
	if(hFrameWnd){
		DestroyWindow( hFrameWnd );
		frameWndMap.erase( hFrameWnd );
		hFrameWnd = NULL;
	}
	::UnregisterClass( clientClassName, getModuleHandle() );
	::UnregisterClass( className, getModuleHandle() );
}

