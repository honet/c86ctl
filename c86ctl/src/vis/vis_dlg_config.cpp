/***
	c86ctl
	
	Copyright (c) 2009-2013, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <CommCtrl.h>
#include <WindowsX.h>

#include "c86ctlmain.h"
#include "vis_dlg_config.h"
#include "resource.h"

using namespace c86ctl::vis;

HWND CVisDlgConfig::hDlg = 0;
bool CVisDlgConfig::isInit = false;

INT_PTR CALLBACK CVisDlgConfig::dlgProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	switch (msg) {
	case WM_ACTIVATE:
		switch(LOWORD(wp)){
		case WA_CLICKACTIVE:
		case WA_ACTIVE:
			C86CtlMain::setActiveDlg(hDlg);
			return TRUE;
			break;
		case WA_INACTIVE:
			C86CtlMain::setActiveDlg(NULL);
			return TRUE;
			break;
		}
		break;

	case WM_COMMAND:
		onCommand((HWND)lp, wp&0xffff, (wp>>16)&0xffff);
		break;
		
	case WM_INITDIALOG:
		onInitDialog(hWnd);
		break;

	case WM_CLOSE:
		destroy();
		return TRUE;
		break;
	}

	return FALSE;
}

void CVisDlgConfig::onInitDialog(HWND hWnd)
{
	HWND hspin, hedit;
	auto gimic = GetC86CtlMain()->getGimics();
	int n = gimic.size();
	
	const int nMaxModules = 4;
	const UINT editid[nMaxModules] = { IDC_EDIT_DELAY0, IDC_EDIT_DELAY1, IDC_EDIT_DELAY2, IDC_EDIT_DELAY3 };
	const UINT spinid[nMaxModules] = { IDC_SPIN_DELAY0, IDC_SPIN_DELAY1, IDC_SPIN_DELAY2, IDC_SPIN_DELAY3 };

	for( int i=0; i<nMaxModules; i++ ){
		hspin = GetDlgItem(hWnd, spinid[i]);
		hedit = GetDlgItem(hWnd, editid[i]);
		SendMessage(hspin, UDM_SETRANGE32, 0, 5000);
		
		if(i<n && gimic[i].get()->isValid()){
			TCHAR str[128];
			int delay=0;
			gimic[i].get()->getDelay(&delay);
			_stprintf(str, _T("%d"), delay);
OutputDebugString(str);
			Edit_SetText(hedit, str);
		}else{
			EnableWindow(hspin, FALSE);
			EnableWindow(hedit, FALSE);
		}
	}
	isInit = true;
}

void CVisDlgConfig::onDelayEditNotify(HWND hwnd, DWORD id, DWORD notifyCode)
{
	auto gimic = GetC86CtlMain()->getGimics();
	int n = gimic.size();
	TCHAR buff[256];
	int delay=0;

	switch(notifyCode){
	case EN_CHANGE:
		if( isInit ){
			Edit_GetText(hwnd, buff, sizeof(buff));
		
			if(id<gimic.size() && gimic[id].get()->isValid()){
				delay = _ttoi(buff);
				gimic[id].get()->setDelay(delay);
			}
		}
		break;
	}
}

void CVisDlgConfig::onCommand(HWND hwnd, DWORD id, DWORD notifyCode)
{
	
	switch(id){
	case IDOK:
		SendMessage(hDlg, WM_CLOSE, 0, 0);
		break;

	case IDC_EDIT_DELAY0:
		onDelayEditNotify(hwnd, 0, notifyCode);
		break;
	case IDC_EDIT_DELAY1:
		onDelayEditNotify(hwnd, 1, notifyCode);
		break;
	case IDC_EDIT_DELAY2:
		onDelayEditNotify(hwnd, 2, notifyCode);
		break;
	case IDC_EDIT_DELAY3:
		onDelayEditNotify(hwnd, 3, notifyCode);
		break;
	}
}

void CVisDlgConfig::create(HWND hParent)
{
	if( !hDlg ){
		hDlg = CreateDialog(C86CtlMain::getInstanceHandle(),
							MAKEINTRESOURCE(IDD_C86CTL_CONFIG), hParent, &dlgProc);

		HICON hIcon;
		hIcon = (HICON)LoadImage(C86CtlMain::getInstanceHandle(),
			MAKEINTRESOURCE(IDI_ICON_C86CTL), IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON), 
			GetSystemMetrics(SM_CYSMICON), 0);
		if(hIcon)
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	}
	ShowWindow( hDlg, SW_SHOW );


}


void CVisDlgConfig::destroy()
{
	DestroyWindow(hDlg);
	hDlg = NULL;
	isInit = false;
}

