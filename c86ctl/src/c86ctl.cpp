/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <string>

#include "c86ctl.h"
#include "module.h"
#include "config.h"
#include "vis_c86main.h"
#include "ringbuff.h"

#include <windows.h>
#include <mmsystem.h>
#include <process.h>
#pragma comment(lib, "winmm.lib")

#include <setupapi.h>
extern "C" {
#include "hidsdi.h"
}
#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// 実験コードなのでどれか一種類しか使えません。
//#define SUPPORT_MIDI
#define SUPPORT_HID


static HANDLE gMainThread = 0;
static HANDLE gSenderThread = 0;
static bool terminateFlag = false;

#ifdef SUPPORT_MIDI
HMIDIOUT hmidi=0;
#endif

#ifdef SUPPORT_HID
HANDLE gHidFile=0;
#endif

//CRingBuff gSendBuff;

HANDLE hMutex=NULL;
std::basic_string<UCHAR> buff;
//static UCHAR buff[10000];
//static UINT idx = 0;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	TCHAR modulePath[_MAX_PATH];
	TCHAR drv[_MAX_PATH], dir[_MAX_PATH], fname[_MAX_PATH], ext[_MAX_PATH];

	switch (ul_reason_for_call){
	case DLL_PROCESS_ATTACH:
		initializeWndManager();
		::GetModuleFileName( hModule, modulePath, _MAX_PATH );
		_tsplitpath( modulePath, drv, dir, fname, ext );
		_tcsncat( inipath, drv, _MAX_PATH );
		_tcsncat( inipath, dir, _MAX_PATH );
		_tcsncat( inipath, TEXT("c86ctl.ini"), _MAX_PATH );
		gModule = hModule;
		break;
	case DLL_PROCESS_DETACH:
		uninitializeWndManager();
		gModule = NULL;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

unsigned int WINAPI threadMain(LPVOID param)
{
	MSG msg;
	CVisC86Main mainWnd;

	mainWnd.create();

	while ( !terminateFlag && GetMessage(&msg , NULL , 0 , 0 )) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	mainWnd.close();

	return (DWORD)msg.wParam;
}


unsigned int WINAPI threadSender(LPVOID param)
{
	LARGE_INTEGER last_send;
	LARGE_INTEGER timer_freq;
	UCHAR sndbuf[65];

	QueryPerformanceCounter(&last_send);
	QueryPerformanceFrequency(&timer_freq);

	while( !terminateFlag ){
		LARGE_INTEGER current;
		QueryPerformanceCounter(&current);
		if( ((current.QuadPart-last_send.QuadPart)*1000/timer_freq.QuadPart) > 1 )
		{
			WaitForSingleObject(hMutex, INFINITE);

			int len = buff.length();
			int idx=0;
			while( idx<len ){
				//memset( sndbuf, 0, 65 );
				sndbuf[0] = 0; // report ID.
				int s = len - idx;
				if( 64<s ) s = 64;
				memcpy( &sndbuf[1], &buff[idx], s );

				if( s<64 ){
					memset( &sndbuf[1+s], 0xff, 64-s );
				}
				idx += s;

				DWORD wlen=0;
				WriteFile( gHidFile, sndbuf, 65, &wlen, 0 );
			}
			
			buff.clear();
			ReleaseMutex(hMutex);
			last_send = current;
			
#ifdef SUPPORT_MIDI
			MIDIHDR midiheader;
//			midiheader.lpData = (LPSTR)&d[0];
			midiheader.dwBufferLength = 6;
			midiheader.dwFlags = 0;
			
			MMRESULT ret;
			ret = midiOutPrepareHeader( hmidi, &midiheader, sizeof(MIDIHDR) );
			ret = midiOutLongMsg( hmidi, &midiheader, sizeof(MIDIHDR) );
			while ((midiheader.dwFlags & MHDR_DONE) == 0);
			ret = midiOutUnprepareHeader( hmidi, &midiheader, sizeof(MIDIHDR) );
#endif
		}
	}
	
	return 0;
}


#ifdef SUPPORT_HID
void OpenHID(void)
{
	GUID hidGuid;
	HDEVINFO devinf;
	SP_DEVICE_INTERFACE_DATA spid;
	SP_DEVICE_INTERFACE_DETAIL_DATA* fc_data=NULL;
	
	HidD_GetHidGuid(&hidGuid);
	devinf = SetupDiGetClassDevs(&hidGuid, NULL, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	for( int i=0; ;i++ ){
		ZeroMemory(&spid, sizeof(spid));
		spid.cbSize = sizeof(spid);
		if( !SetupDiEnumDeviceInterfaces(devinf, NULL, &hidGuid, i, &spid) )
			break;

		unsigned long sz;
		SetupDiGetDeviceInterfaceDetail( devinf, &spid, NULL, 0, &sz, 0 );
		
		PSP_INTERFACE_DEVICE_DETAIL_DATA dev_det = (PSP_INTERFACE_DEVICE_DETAIL_DATA)( malloc(sz) );
		dev_det->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
		SetupDiGetDeviceInterfaceDetail( devinf, &spid, dev_det, sz, &sz, 0 );


		HANDLE handle = CreateFile( dev_det->DevicePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL );
		free(dev_det); dev_det = NULL;
		
		if( handle == INVALID_HANDLE_VALUE ) continue;

		HIDD_ATTRIBUTES attr;
		HidD_GetAttributes( handle, &attr ) ;

		if( attr.VendorID == 0x16c0 && attr.ProductID == 0x05e4 ){
			gHidFile = handle;
			break;
		}

		CloseHandle(handle);
	}
}
void CloseHID(void)
{
	if( gHidFile ){

		CloseHandle(gHidFile);
		gHidFile = NULL;
	}
}

#endif




// ----------------------------------------------------------------------

C86CTL_API int WINAPI c86ctl_initialize(void)
{
	unsigned int mainThreadID, senderThreadID;

#ifdef SUPPORT_HID
	//idx = 1;
	buff.clear();
	OpenHID();
#endif
	
#ifdef SUPPORT_MIDI
	buff.clear();
	//idx = 2;
	MMRESULT res = midiOutOpen(&hmidi, MIDI_MAPPER, 0, 0, CALLBACK_NULL);
#endif

	hMutex = CreateMutex( NULL, FALSE, NULL );
	
	gMainThread = (HANDLE)_beginthreadex( NULL, 0, &threadMain, NULL, 0, &mainThreadID );
	gSenderThread = (HANDLE)_beginthreadex( NULL, 0, &threadSender, NULL, 0, &senderThreadID );
	
	return 0;
}

C86CTL_API int WINAPI c86ctl_deinitialize(void)
{
	c86ctl_reset();

	terminateFlag = true;

	::WaitForSingleObject( gMainThread, INFINITE );
	::WaitForSingleObject( gSenderThread, INFINITE );
	
	if( hMutex ){
		ReleaseMutex( hMutex );
		hMutex = NULL;
	}

		
#ifdef SUPPORT_HID
	buff.clear();
	//idx = 1;
	CloseHID();
#endif
	
#ifdef SUPPORT_MIDI
	buff.clear();
	//idx = 2;
	midiOutClose(hmidi);
#endif

	return 0;
}

C86CTL_API int WINAPI c86ctl_reset(void)
{
	gOPNA[0].reset();
	gOPNA[1].reset();

#ifdef SUPPORT_HID
	WaitForSingleObject(hMutex, INFINITE);
	buff.push_back(0x82);	// software reset
	buff.push_back(0x0);
	buff.push_back(0x0);
	buff.push_back(0x0);
	ReleaseMutex(hMutex);
#endif
	for(;;){
		WaitForSingleObject(hMutex, INFINITE);
		if( buff.length() == 0 ) break;
		ReleaseMutex(hMutex);
		Sleep(100);
	}
	
#ifdef SUPPORT_MIDI
	midiOutReset(hmidi);
#endif
	
	return 0;
}

C86CTL_API void WINAPI c86ctl_out( UINT addr, UCHAR data )
{
	gOPNA[0].setReg(addr,data);

	//UCHAR *d = &buff[idx];

	addr &= 0x1ff;

#ifdef SUPPORT_HID
	if(gHidFile){
		WaitForSingleObject(hMutex, INFINITE);
		buff.push_back(0);// cmd
		buff.push_back( (UCHAR)(addr>>8) );
		buff.push_back( (UCHAR)(addr&0xff) );
		buff.push_back( data );
		ReleaseMutex(hMutex);
#if 0
		d[0] = 0;// cmd
		d[1] = (UCHAR)(addr>>8);
		d[2] = (UCHAR)(addr&0xff);
		d[3] = data;
		idx +=4;
		
		LARGE_INTEGER current;
		QueryPerformanceCounter(&current);
		if( (idx >= 65) ||
			((current.QuadPart-last_send.QuadPart)*1000/timer_freq.QuadPart) >= 1 )
		{
			if(idx<65){
				memset(&buff[idx], 0xff, 65-idx );
			}

			DWORD len=65, wlen=0;
			buff[0] = 0; // report ID
			WriteFile( gHidFile, buff, len, &wlen, 0 );
			
			idx = 1;
			last_send = current;
		}
#endif
	}
#endif
	
#ifdef SUPPORT_MIDI
	if(hmidi){
		d[0] = (UCHAR)(addr>>6);
		d[1] = (UCHAR)(((addr&0x3f)<<1) | (data>>7));
		d[2] = (UCHAR)(data&0x7f);
		idx +=3;
//		if( addr >= 0x200 ){
//			OutputDebugString("ERROR\r\n");
//		}

		LARGE_INTEGER current;
		QueryPerformanceCounter(&current);
		if( (idx >= /*23*/128) || // 32/4=8 ((8-2)+1)*3=21+2=23
			((current.QuadPart-last_send.QuadPart)*1000/timer_freq.QuadPart) > 2 )
//		if( idx >= 10000 || ((current.QuadPart-last_send.QuadPart)*1000/timer_freq.QuadPart) > 10 )
		{
			buff[0] = 0xf0;
			buff[1] = 0x7d;
			buff[idx++] = 0xf7;
			MIDIHDR midiheader;
			midiheader.lpData = (LPSTR)&buff[0];
			midiheader.dwBufferLength = idx;
			midiheader.dwFlags = 0;

			assert( idx % 3 == 0 );
			
			//CHAR str[128];
			//sprintf(str, "OUT:%d ",idx);
			//OutputDebugString(str);
			//for( int j=0; j<idx; j++ ){
			//	sprintf(str, "%02x ", buff[j] );
			//	OutputDebugString(str);
			//}
			//OutputDebugString("\r\n");

			MMRESULT ret;
			ret = midiOutPrepareHeader( hmidi, &midiheader, sizeof(MIDIHDR) );
			ret = midiOutLongMsg( hmidi, &midiheader, sizeof(MIDIHDR) );
			while ((midiheader.dwFlags & MHDR_DONE) == 0);
			ret = midiOutUnprepareHeader( hmidi, &midiheader, sizeof(MIDIHDR) );



			idx = 2;
			last_send = current;
		}
	}
#endif

}

C86CTL_API UCHAR WINAPI c86ctl_in( UINT addr )
{
	return gOPNA[0].getReg(addr);
}

