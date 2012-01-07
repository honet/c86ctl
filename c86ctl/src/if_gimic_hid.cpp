/***
	c86ctl
	gimic コントロール HID版(実験コード)
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
	Thanks to Nagai "Guu" Osamu 2011/12/08 for his advice.
	
 */
#include "stdafx.h"
#include "if_gimic_hid.h"

#ifdef SUPPORT_HID

#define GIMIC_USBVID 0x16c0
#define GIMIC_USBPID 0x05e4

#include <setupapi.h>
extern "C" {
#include "hidsdi.h"
}

/*----------------------------------------------------------------------------
	追加ライブラリ
----------------------------------------------------------------------------*/
#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "winmm.lib")

/*----------------------------------------------------------------------------
	コンストラクタ
----------------------------------------------------------------------------*/
GimicHID::GimicHID( HANDLE h ): hHandle(h)
{
	rbuff.alloc( 128 );
}

/*----------------------------------------------------------------------------
	デストラクタ
----------------------------------------------------------------------------*/
GimicHID::~GimicHID(void)
{
	CloseHandle(hHandle);
	hHandle = NULL;
}

/*----------------------------------------------------------------------------
	HIDIF factory
----------------------------------------------------------------------------*/
std::vector< std::shared_ptr<GimicIF> > GimicHID::CreateInstances(void)
{
	std::vector< std::shared_ptr<GimicIF> > instances;
	
	GUID hidGuid;
	HDEVINFO devinf;
	SP_DEVICE_INTERFACE_DATA spid;
	SP_DEVICE_INTERFACE_DETAIL_DATA* fc_data = NULL;

	HidD_GetHidGuid(&hidGuid);
	devinf = SetupDiGetClassDevs(
		&hidGuid,
		NULL,
		0,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if( devinf ){
		for(int i = 0; ;i++) {
			ZeroMemory(&spid, sizeof(spid));
			spid.cbSize = sizeof(spid);
			if(!SetupDiEnumDeviceInterfaces(devinf, NULL, &hidGuid, i, &spid))
				break;

			unsigned long sz;
			SetupDiGetDeviceInterfaceDetail(devinf, &spid, NULL, 0, &sz, 0);

			PSP_INTERFACE_DEVICE_DETAIL_DATA dev_det =
				(PSP_INTERFACE_DEVICE_DETAIL_DATA)(malloc(sz));
			dev_det->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			SetupDiGetDeviceInterfaceDetail(devinf, &spid, dev_det, sz, &sz, 0);

			HANDLE hHID = CreateFile(
				dev_det->DevicePath,
				GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_NO_BUFFERING,
				NULL);
			free(dev_det);
			dev_det = NULL;

			if(hHID == INVALID_HANDLE_VALUE)
				continue;

			HIDD_ATTRIBUTES attr;
			HidD_GetAttributes(hHID, &attr);

			if(attr.VendorID == GIMIC_USBVID && attr.ProductID == GIMIC_USBPID){
				instances.push_back( GimicIFPtr(new GimicHID(hHID)) );
			}else{
				CloseHandle(hHID);
			}
		}
	}

	return instances;
}


/*----------------------------------------------------------------------------
	実装
----------------------------------------------------------------------------*/

int GimicHID::reset(void)
{
	// リセットコマンド送信
	UCHAR d[4] = { 0x82, 0, 0, 0 };
	rbuff.write(d,4);
	// 転送完了待ち
	while(rbuff.get_length())
		Sleep(10);

	return C86CTL_ERR_NONE;
}

int GimicHID::setSSGVolume(UCHAR vol)
{
	UCHAR d[4] = { 0x84, vol, 0, 0 };
	rbuff.write(d,4);

	// 転送完了待ち
	while(rbuff.get_length())
		Sleep(10);

	return C86CTL_ERR_NONE;
}

int GimicHID::getSSGVolume(UCHAR *vol)
{
	if( !vol )
		return C86CTL_ERR_INVALID_PARAM;

	UCHAR d[4] = { 0x86, 0, 0, 0 };
	rbuff.write(d,4);

	UCHAR buff[66];
	DWORD len;

	// 転送完了待ち
	while(rbuff.get_length())
		Sleep(10);

	int ret = ReadFile( hHandle, buff, 65, &len, NULL);
	*vol = buff[1];

	return C86CTL_ERR_NONE;
}

int GimicHID::setPLLClock(UINT clock)
{
	UCHAR d[4] = { 0x83, clock&0xff, (clock>>8)&0xff, (clock>>16)&0xff };
	rbuff.write(d,4);
	
	// 転送完了待ち
	while(rbuff.get_length())
		Sleep(10);

	return C86CTL_ERR_NONE;
}

int GimicHID::getPLLClock(UINT *clock)
{
	if( !clock )
		return C86CTL_ERR_INVALID_PARAM;

	UCHAR d[4] = { 0x85, 0, 0, 0 };
	rbuff.write(d,4);

	UCHAR buff[66];
	DWORD len;

	// 転送完了待ち
	while(rbuff.get_length())
		Sleep(10);

	int ret = ReadFile( hHandle, buff, 65, &len, NULL);
	*clock = *((UINT*)&buff[1]);

	return C86CTL_ERR_NONE;
}


int GimicHID::getMBInfo( struct Devinfo *info )
{
	if( !info )
		return C86CTL_ERR_INVALID_PARAM;

	UCHAR d[4] = { 0x91, 0xff, 0, 0 };
	rbuff.write(d,4);

	UCHAR buff[66];
	DWORD len;

	// 転送完了待ち
	while(rbuff.get_length())
		Sleep(10);

	int ret = ReadFile( hHandle, buff, 65, &len, NULL);
	memcpy( info, &buff[1], 32 ); // 1byte目はUSBのInterfaceNo.なので飛ばす

	return C86CTL_ERR_NONE;
}

int GimicHID::getModuleInfo( struct Devinfo *info )
{
	if( !info )
		return C86CTL_ERR_INVALID_PARAM;

	UCHAR d[4] = { 0x91, 0, 0, 0 };
	rbuff.write(d,4);

	UCHAR buff[66];
	DWORD len;

	// 転送完了待ち
	while(rbuff.get_length())
		Sleep(10);

	int ret = ReadFile( hHandle, buff, 65, &len, NULL);
	memcpy( info, &buff[1], 32 ); // 1byte目はUSBのInterfaceNo.なので飛ばす

	return C86CTL_ERR_NONE;
}


void GimicHID::out(UINT addr, UCHAR data)
{
	UCHAR d[4] = { 0, addr>>8, addr&0xff, data };
	rbuff.write(d,4);
}

void GimicHID::tick(void)
{
	UCHAR buff[128];

	if( !hHandle )
		return;

	buff[0] = 0; // HID interface id.
	UINT sz = rbuff.get_length();
	if( 0<sz ){
		sz = MIN(sz,64) & ~0x3;
		rbuff.read( &buff[1], sz );
		if( sz<64 )
			memset( &buff[1+sz], 0xff, 64-sz );

		DWORD len;
		int ret = WriteFile(hHandle, buff, 65, &len, NULL);
		//{
		//	char str[128];
		//	sprintf(str, "%02x %02x %02x %02x\n", buff[1], buff[2], buff[3], buff[4] );
		//	OutputDebugStringA(str);
		//}

		if(ret == 0 || 65 != len){
			CloseHandle(hHandle);
			hHandle = NULL;
			// なんかthrowする？
			return;
		}
	}

	return;
}

#endif
