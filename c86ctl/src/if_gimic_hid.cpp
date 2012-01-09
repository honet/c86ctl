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
#include <algorithm>
#include "chip.h"
#include "opm.h"
#include "opna.h"
#include "opn3l.h"

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
GimicHID::GimicHID( HANDLE h )
	: hHandle(h), chip(0), chiptype(CHIP_UNKNOWN), seqno(0)
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
	if( chip )
		delete chip;
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
				0,//FILE_FLAG_NO_BUFFERING,
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

	std::for_each( instances.begin(), instances.end(), [](std::shared_ptr<GimicIF> x){ x->init(); } );
	return instances;
}

int GimicHID::sendMsg( MSG *data )
{
	UCHAR buff[66];

	if( !hHandle )
		return C86CTL_ERR_NODEVICE;

	buff[0] = 0; // HID interface id.

	UINT sz = data->len;
	if( 0<sz ){
		memcpy( &buff[1], &data->dat[0], sz );
		if( sz<64 )
			memset( &buff[1+sz], 0xff, 64-sz );

		DWORD len;
		int ret = WriteFile(hHandle, buff, 65, &len, NULL);

		if(ret == 0 || 65 != len){
			CloseHandle(hHandle);
			hHandle = NULL;
			return C86CTL_ERR_UNKNOWN;
		}
	}

	return C86CTL_ERR_NONE;
}

int GimicHID::transaction( MSG *txdata, uint8_t *rxdata, uint32_t rxsz )
{
	int ret;
	if( C86CTL_ERR_NONE != ( ret = sendMsg( txdata ) ) )
		return ret;

	UCHAR buff[66];
	DWORD len = 0;

	if( !ReadFile( hHandle, buff, 65, &len, NULL) ){
		return C86CTL_ERR_UNKNOWN;
	}
	memcpy( rxdata, &buff[1], rxsz ); // 1byte目はUSBのInterfaceNo.なので飛ばす

	return C86CTL_ERR_NONE;
}

/*----------------------------------------------------------------------------
	実装
----------------------------------------------------------------------------*/
int GimicHID::init(void)
{
	Devinfo info;
	getModuleInfo(&info);
	if( !memcmp( info.Devname, "GMC-OPN3L", 9 ) ){
		chiptype = CHIP_OPN3L;
		chip = new COPN3L();
	}else if( !memcmp( info.Devname, "GMC-OPM", 7 ) ){
		chiptype = CHIP_OPM;
		chip = new COPM();
	}else if( !memcmp( info.Devname, "GMC-OPNA", 8 ) ){
		chiptype = CHIP_OPNA;
		chip = new COPNA();
	}
	return C86CTL_ERR_NONE;
}

int GimicHID::reset(void)
{
	// リセットコマンド送信
	MSG d = { 2, { 0xfd, 0x82, 0 } };
	return sendMsg( &d );
}

int GimicHID::setSSGVolume(UCHAR vol)
{
	MSG d = { 3, { 0xfd, 0x84, vol } };
	return sendMsg( &d );
}

int GimicHID::getSSGVolume(UCHAR *vol)
{
	if( !vol )
		return C86CTL_ERR_INVALID_PARAM;

	MSG d = { 2, { 0xfd, 0x86 } };
	return transaction( &d, (uint8_t*)vol, 1 );
}

int GimicHID::setPLLClock(UINT clock)
{
	MSG d = { 6, { 0xfd, 0x83, clock&0xff, (clock>>8)&0xff, (clock>>16)&0xff, (clock>>24)&0xff, 0 } };
	return sendMsg( &d );
}

int GimicHID::getPLLClock(UINT *clock)
{
	if( !clock )
		return C86CTL_ERR_INVALID_PARAM;

	MSG d = { 2, { 0xfd, 0x85 } };
	return transaction( &d, (uint8_t*)clock, 4 );
}

int GimicHID::getMBInfo( struct Devinfo *info )
{
	if( !info )
		return C86CTL_ERR_INVALID_PARAM;

	MSG d = { 3, { 0xfd, 0x91, 0xff } };
	return transaction( &d, (uint8_t*)info, 32 );
}

int GimicHID::getModuleInfo( struct Devinfo *info )
{
	if( !info )
		return C86CTL_ERR_INVALID_PARAM;

	MSG d = { 3, { 0xfd, 0x91, 0 } };
	return transaction( &d, (uint8_t*)info, 32 );
}

int GimicHID::getFWVer( UINT *major, UINT *minor, UINT *rev, UINT *build )
{
	uint8_t rx[16];
	MSG d = { 2, { 0xfd, 0x92 } };
	int ret;

	if( C86CTL_ERR_NONE == (ret = transaction( &d, rx, 16 )) ){
		if( major ) *major = *((uint32_t*)&rx[0]);
		if( minor ) *minor = *((uint32_t*)&rx[4]);
		if( rev )   *rev   = *((uint32_t*)&rx[8]);
		if( build ) *build = *((uint32_t*)&rx[12]);
	}
	return ret;
}

void GimicHID::out(UINT addr, UCHAR data)
{
	bool flag = true;
	if( chip )
		flag = chip->setReg(addr, data );
	if( flag ){
		switch( chiptype ){
		case CHIP_OPNA:
		case CHIP_OPN3L:
			if( 0x100<=addr && addr<=0x110 )
				addr -= 0x40;
			break;
		case CHIP_OPM:
			if( 0xfc<=addr && addr<=0xff )
				addr -= 0xe0;
			break;
		}
		if( addr < 0xfc ){
			MSG d = { 2, { addr&0xff, data } };
			rbuff.write1(d);
		}else if( 0x100 <= addr && addr <= 0x1fb ){
			MSG d = { 3, { 0xfe, addr&0xff, data } };
			rbuff.write1(d);
		}
	}
}

UCHAR GimicHID::in(UINT addr)
{
	if( chip )
		return chip->getReg(addr);

	return 0;
}

void GimicHID::tick(void)
{
	if( !hHandle )
		return;
	if( rbuff.isempty() )
		return;
	
	UCHAR buff[128];
	UINT sz=0, i=1;
	MSG d;
	
	buff[0] = 0; // HID interface id.

	for(;;){
		UINT l = rbuff.query_read_ptr()->len;
		if( 64<(sz+l) )
			break;
		if( !rbuff.read1(&d) )
			break;
		sz += d.len;
		for( UINT j=0; j<d.len; j++ )
			buff[i++] = d.dat[j];
		if( rbuff.isempty() )
			break;
	}

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
	return;
}

#endif
