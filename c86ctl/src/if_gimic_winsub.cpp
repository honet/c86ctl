/***
	c86ctl
	gimic コントロール WinUSB版(実験コード)
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com


	note: honet.kk
	Interface誌 2010/02号 第５章掲載のサンプルプログラムを
	ほぼ丸ぱくりしています。あくまでもHID通信版との違いを
	テストしてみるためのサンプル実装。

	使用するにはgimicのファーム変更（デスクリプタ変更）と
	winusbドライバのインストールが必要。
*/


#include "stdafx.h"
#include "if_gimic_winusb.h"

#ifdef SUPPORT_WINUSB

#include <setupapi.h>
#include <initguid.h>
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "winusb.lib")

using namespace c86ctl;

// デバイスドライバのinf内で定義したGUID
// (WinUSB.sys使用デバイスに対する識別子）
// {63275336-530B-4069-92B6-5F8AE3465462}
DEFINE_GUID(GUID_DEVINTERFACE_WINUSBTESTTARGET, 
  0x63275336, 0x530b, 0x4069, 0x92, 0xb6, 0x5f, 0x8a, 0xe3, 0x46, 0x54, 0x62);


#define PIPE_BUFFER_SIZE 64

// -----------------------------------------------------------------------------------

// -#1-
// WinUsb_howto.docx 記載のプログラムを元に、
// 異常判定処理を追加。
BOOL GIMICWinUSB::GetDevicePath(
	LPGUID InterfaceGuid,
	LPTSTR DevicePath,
	size_t BufLen)
{
	BOOL bResult = TRUE;
	HDEVINFO hDeviceInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DATA interfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA pDetailData = NULL;
	ULONG requiredLength = 0;
	ULONG gotLength = 0;

	// [1]
	hDeviceInfo = SetupDiGetClassDevs(
		InterfaceGuid, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (INVALID_HANDLE_VALUE == hDeviceInfo)
	{
		bResult = FALSE;
	}

	// [2]
	if (bResult)
	{
		memset(&interfaceData, 0, sizeof(interfaceData));
		interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		bResult = SetupDiEnumDeviceInterfaces(
			hDeviceInfo, NULL, InterfaceGuid, 0, &interfaceData);
	}

	// [3]
	if (bResult)
	{
		bResult = SetupDiGetDeviceInterfaceDetail(
			hDeviceInfo, &interfaceData, NULL, 0, &requiredLength, NULL);
		if (!bResult && (ERROR_INSUFFICIENT_BUFFER == GetLastError()))
		{
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}

	}
	if (bResult)
	{
		assert(0 < requiredLength);
		pDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)
			LocalAlloc(LMEM_FIXED, requiredLength);
		if (NULL == pDetailData)
		{
			bResult = FALSE;
		}
	}

	if (bResult)
	{
		memset(pDetailData, 0, requiredLength);
		pDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		bResult = SetupDiGetDeviceInterfaceDetail(
			hDeviceInfo, &interfaceData,
			pDetailData, requiredLength, &gotLength, NULL);
	}

	// [4]
	if (bResult)
	{
		if (_tcscpy_s(DevicePath, BufLen, pDetailData->DevicePath))
		{
			bResult = FALSE;
		}
	}

	if (INVALID_HANDLE_VALUE != hDeviceInfo)
	{
		SetupDiDestroyDeviceInfoList(hDeviceInfo);
	}
	if (pDetailData)
	{
		LocalFree(pDetailData);
	}

	return bResult;
}


// -#2-
// WinUsb_howto.docx 記載のプログラムを元に、
// bSync の取り扱いと、異常判定処理を追加。
HANDLE GIMICWinUSB::OpenDevice(BOOL bSync)
{
	HANDLE hDev = 0;
	TCHAR devicePath[_MAX_PATH + 1];
	BOOL bResult = TRUE;
	DWORD attrFlags = FILE_ATTRIBUTE_NORMAL;

	bResult = GetDevicePath(
		(LPGUID) &GUID_DEVINTERFACE_WINUSBTESTTARGET,
		devicePath, sizeof(devicePath) / sizeof(devicePath[0]));

	if (bResult)
	{
		if (!bSync)
		{
			// 【筆者註】
			// WinUsb_howto.docx では bSync が無視されていましたが、
			// 使い方を想像してこの分岐処理を追加しました。
			attrFlags |= FILE_FLAG_OVERLAPPED;
		}
		hDev = CreateFile(
			devicePath,
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_WRITE | FILE_SHARE_READ,
			NULL, OPEN_EXISTING, attrFlags, 0);
	}

	return hDev;
}




GIMICWinUSB::GIMICWinUSB(HINSTANCE,BUFFER*)
{
	hUsb = INVALID_HANDLE_VALUE;
	hWinUsb = 0;
	IntrPipeId = 0xffu;
	IntrPipeMaxPktSize = 0u;
}

GIMICWinUSB::~GIMICWinUSB(void)
{
	Close();
}


BOOL GIMICWinUSB::Open(void)
{
	MSG msg;
	int i = 0;
	BOOL bResult = FALSE;
	USB_INTERFACE_DESCRIPTOR desc;
	WINUSB_PIPE_INFORMATION pipeInfo;

	hUsb = OpenDevice(FALSE);
	if (INVALID_HANDLE_VALUE == hUsb)
	{
		MessageBox(0,TEXT("デバイスを開けませんでした"),TEXT("winusbtest"),MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// -#3-
	bResult = WinUsb_Initialize(hUsb, &hWinUsb);
	if (!bResult)
	{
		MessageBox(0,TEXT("WinUSB 初期化に失敗しました"),TEXT("winusbtest"),MB_OK | MB_ICONSTOP);
		CloseHandle(hUsb);
		return FALSE;
	}

	// -#4-
	bResult = WinUsb_QueryInterfaceSettings(hWinUsb, 0, &desc);
	if (!bResult)
	{
		MessageBox(0,TEXT("インターフェースディスクリプタ取得に失敗しました"),TEXT("winusbtest"),MB_OK | MB_ICONSTOP);
		WinUsb_Free(hWinUsb);
		CloseHandle(hUsb);
		return FALSE;
	}

	// -#5-
	for (i = 0; i < desc.bNumEndpoints; ++i)
	{
		bResult = WinUsb_QueryPipe(hWinUsb, 0, (UCHAR)i, &pipeInfo);

		if( !bResult )
		{
			break;
		}
		else if(pipeInfo.PipeType == UsbdPipeTypeInterrupt)
		{
			/* Interrupt-IN */
			IntrPipeId = pipeInfo.PipeId;
			IntrPipeMaxPktSize = pipeInfo.MaximumPacketSize;
		}
		else
		{
			bResult = FALSE;
			break;
		}
	}
	if( !bResult ||
		0xffu == IntrPipeId )
	{
		MessageBox(0,TEXT("インターフェースディスクリプタ解析に失敗しました"),TEXT("winusbtest"),MB_OK | MB_ICONSTOP);
		WinUsb_Free(hWinUsb);
		CloseHandle(hUsb);
		return FALSE;
	}
	assert(IntrPipeMaxPktSize <= PIPE_BUFFER_SIZE);

	return TRUE;
}


void GIMICWinUSB::Close(void)
{
	// -#9-
	if( hWinUsb ){
		WinUsb_Free(hWinUsb);
		hWinUsb = NULL;
	}
	if( hUsb ){
		CloseHandle(hUsb);
		hUsb = NULL;
	}
}

void GIMICWinUSB::Reset(void)
{
}

BOOL GIMICWinUSB::Write(UCHAR*,int)
{
	#if 0
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
#ifdef SUPPORT_HID
				WriteFile( gHidFile, sndbuf, 65, &wlen, 0 );
#endif

#ifdef SUPPORT_WINUSB
				WinUsb_WritePipe( hWinUsb, IntrPipeId, &sndbuf[1], IntrPipeMaxPktSize, &wlen, 0);
#endif
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
		#endif

	return TRUE;
}


#endif

