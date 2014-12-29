/***
	c86ctl
	C86USB コントロール WinUSB版
	
	Copyright (c) 2009-2013, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com

	PIPE MESSAGE FORMAT ----
	00 [7:4]slot,[3:1]chip,[0:0]ex addr data
	FE nn nn nn nsync
	
	note: honet.kk
*/


#include "stdafx.h"
#include "if_c86usb_winusb.h"

#ifdef SUPPORT_WINUSB

#include <setupapi.h>
#include <initguid.h>
#include <algorithm>
#include "chip/chip.h"

using namespace c86ctl;

/*----------------------------------------------------------------------------
	追加ライブラリ
----------------------------------------------------------------------------*/
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "winusb.lib")


/*----------------------------------------------------------------------------
	定義いろいろ
----------------------------------------------------------------------------*/
#define PIPE_BUFFER_SIZE 1024
// VENDOR REQUEST ------------------------------------------------------------
#define C86_VENDOR_GET_BOARD_INFO		0x81
// SETUP:
//   bmRequestType: (0x80|0x40|0x00) DeviceToHost, vendor, device
//   bRequest     : USBFS_VENDOR_GET_BOARD_INFO
//   wValue       : ignore
//   wIndex       : ボード番号(0~4)
//   wLength      : 
// DATA STAGE:
//   BOARD_INFO

#define C86_VENDOR_GET_FW_VER			0x82
// SETUP:
//   bmRequestType: DeviceToHost, vendor, device
//   bRequest     : USBFS_VENDOR_GET_FW_VER
//   wValue       : ignore
//   wIndex       : ignore
//   wLength      : 4
// DATA STAGE:
//   (uint32_t)verison

#define C86_VENDOR_SYSTEM_RESET			0x11
// SETUP:
//   bmRequestType: HostToDevice, vendor, device
//   bRequest     : USBFS_VENDOR_CBUS_RESET
//   wValue       : ignore
//   wIndex       : 0
//   wLength      : 0

#define C86_VENDOR_BOARD_CONTROL		0x13
// SETUP:
//   bmRequestType: HostToDevice, vendor, device
//   bRequest     : USBFS_VENDOR_BOARD_CONTROL
//   wValue       : コントロール番号
//   wIndex       : ボード番号(0~4)
//   wLength      : データ長
// DATTA STAGE:
//   コントロールごとに定義


/*----------------------------------------------------------------------------
	コンストラクタ
----------------------------------------------------------------------------*/
C86WinUSB::C86WinUSB()
	: hDev(0), hWinUsb(0), cps(0), cal(0), calcount(0),
	  inPipeId(0), outPipeId(0), inPipeMaxPktSize(0), outPipeMaxPktSize(0), nmodules(0)
{
	rbuff.alloc( PIPE_BUFFER_SIZE );
	::InitializeCriticalSection(&csection);

	::QueryPerformanceFrequency(&freq);
	freq.QuadPart/=1000; // 1ms

	memset(modules, 0, sizeof(modules));
}

/*----------------------------------------------------------------------------
	デストラクタ
----------------------------------------------------------------------------*/
C86WinUSB::~C86WinUSB(void)
{
	::DeleteCriticalSection(&csection);
	CloseHandle(hDev);
	WinUsb_Free(hWinUsb);
	hDev = NULL;
	hWinUsb = NULL;
	for(int i=0; i<nmodules; i++)
		if(modules[i])
			delete modules[i];

}

/*----------------------------------------------------------------------------
	Device Open
----------------------------------------------------------------------------*/
bool C86WinUSB::OpenDevice(std::basic_string<TCHAR> devpath)
{
	HANDLE hNewDev = CreateFile(
		devpath.c_str(),
		GENERIC_READ|GENERIC_WRITE,
		0 /*FILE_SHARE_READ|FILE_SHARE_WRITE*/,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED, //FILE_FLAG_NO_BUFFERING,
		NULL);

	if(hNewDev == INVALID_HANDLE_VALUE)
		return false;

	HANDLE hNewWinUsb = NULL;
	if (!WinUsb_Initialize(hNewDev, &hNewWinUsb)){
		//DWORD err = GetLastError();
		CloseHandle(hNewDev);
		return false;
	}
	
	// エンドポイント情報取得
	USB_INTERFACE_DESCRIPTOR desc;
	if (!WinUsb_QueryInterfaceSettings(hNewWinUsb, 0, &desc)){
		WinUsb_Free(hNewWinUsb);
		CloseHandle(hNewDev);
		return false;
	}

	for ( int i=0; i<desc.bNumEndpoints; i++ ){
		WINUSB_PIPE_INFORMATION pipeInfo;
		if (WinUsb_QueryPipe(hNewWinUsb, 0, (UCHAR)i, &pipeInfo)){
			if( pipeInfo.PipeType == UsbdPipeTypeBulk &&
				USB_ENDPOINT_DIRECTION_OUT(pipeInfo.PipeId) ){
				outPipeId = pipeInfo.PipeId;
				outPipeMaxPktSize = pipeInfo.MaximumPacketSize;
			}else if ( pipeInfo.PipeType == UsbdPipeTypeBulk &&
				USB_ENDPOINT_DIRECTION_IN(pipeInfo.PipeId) ){
				inPipeId = pipeInfo.PipeId;
				inPipeMaxPktSize = pipeInfo.MaximumPacketSize;
			}
		}
	}

	// タイムアウト設定
	ULONG timeout = 5000; //ms
	::WinUsb_SetPipePolicy( hNewWinUsb, outPipeId, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout );
	::WinUsb_SetPipePolicy( hNewWinUsb, inPipeId, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout );

	// ここでハンドル更新
	hDev = hNewDev;
	hWinUsb = hNewWinUsb;
	devPath = devpath;

	WinUsb_FlushPipe(hWinUsb, outPipeId);
	WinUsb_FlushPipe(hWinUsb, inPipeId);

	// Module情報取得 -----------
	// TODO: とりあえず。
	int n = 0;
	for (int i=0; i<2; i++){
		BOARD_INFO binfo;
		int ret = getBoardInfo(i, &binfo);
		for (int k=0; k<binfo.nchips; k++){
			ChipType newtype = static_cast<ChipType>(binfo.chiptype[k]);
			if (modules[n]==0){
				C86WinUSB::C86ModuleWinUSB *module =  new C86ModuleWinUSB(this, i, k, newtype);
				if (module)
					modules[n++] = module;
			}else if( modules[n++]->getChipType() != newtype ){
				goto MODULE_CHANGED;
			}
		}
	}
	nmodules = n;

	return true;


MODULE_CHANGED:
	CloseHandle(hDev);
	WinUsb_Free(hWinUsb);
	hDev = NULL;
	hWinUsb = NULL;
	return false;
}

/*----------------------------------------------------------------------------
	factory
----------------------------------------------------------------------------*/
int C86WinUSB::UpdateInstances( withlock< std::vector< std::shared_ptr<BaseSoundDevice> > > &devices)
{
	devices.lock();
	std::for_each( devices.begin(), devices.end(), []( std::shared_ptr<BaseSoundDevice> x ){ x->checkConnection(); } );

	BOOL bResult = TRUE;
	
	HDEVINFO devinf = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DATA spid;
	PSP_DEVICE_INTERFACE_DETAIL_DATA fc_data = NULL;


	devinf = SetupDiGetClassDevs(
		(LPGUID)&GUID_DEVINTERFACE_C86BOX_WINUSB_TARGET,
		NULL,
		0,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if( devinf ){
		for ( int i=0; ;i++ ){
			ZeroMemory(&spid, sizeof(spid));
			spid.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
			if ( !SetupDiEnumDeviceInterfaces(devinf, NULL,
				  (LPGUID) &GUID_DEVINTERFACE_C86BOX_WINUSB_TARGET, i, &spid) ){
				break;
			}

			unsigned long sz;
			std::basic_string<TCHAR> devpath;

			// 必要なバッファサイズ取得
			bResult = SetupDiGetDeviceInterfaceDetail(devinf, &spid, NULL, 0, &sz, NULL);
			PSP_INTERFACE_DEVICE_DETAIL_DATA dev_det = (PSP_INTERFACE_DEVICE_DETAIL_DATA)(malloc(sz));
			dev_det->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

			// デバイスノード取得
			if (!SetupDiGetDeviceInterfaceDetail(devinf, &spid, dev_det, sz, &sz, NULL)){
				free(dev_det);
				break;
			}

			devpath = dev_det->DevicePath;
			free(dev_det);
			dev_det = NULL;

			// 既にインスタンスがあるかどうか検索
			auto it = std::find_if( devices.begin(), devices.end(),
				[devpath](std::shared_ptr<BaseSoundDevice> x) -> bool {
					C86WinUSB *gdev = dynamic_cast<C86WinUSB*>(x.get());
					if(!gdev) return false;
					if(gdev->devPath != devpath ) return false;
					return true;
				}
			);
			
			if( it == devices.end() ){
				C86WinUSB *dev = new C86WinUSB();
				if( dev ){
					if( dev->OpenDevice(devpath) ){
						devices.push_back( BaseSoundDevicePtr(dev) );
					}else{
						delete dev;
					}
				}
			}
			else if (!(*it)->isValid()){
				C86WinUSB *dev = dynamic_cast<C86WinUSB*>(it->get());
				if( !dev->OpenDevice(devpath) ){
					// OpenDeviceが失敗した場合は音源モジュールが前回接続時と
					// 異なっているため、別インスタンスを生成する
					C86WinUSB *newdev = new C86WinUSB();
					if( newdev ){
						if( newdev->OpenDevice(devpath) ){
							devices.push_back( BaseSoundDevicePtr(newdev) );
						}else{
							delete newdev;
						}
					}
				}
			}
		}
		
		SetupDiDestroyDeviceInfoList(devinf);
	}

	devices.unlock();
	return 0;
}


/*----------------------------------------------------------------------------
	internal.
----------------------------------------------------------------------------*/
int C86WinUSB::sendMsg( UINT *data, UINT sz )
{
	UCHAR buff[66];
	int ret = C86CTL_ERR_UNKNOWN;

	sz<<=2;
	if( 0<sz ){
		memcpy( &buff[0], data, sz );
		if( sz<64 )
			memset( &buff[sz], 0xff, 64-sz );

		::EnterCriticalSection(&csection);
		ret = devWrite(buff);
		::LeaveCriticalSection(&csection);
	}

	return ret;
}

int C86WinUSB::devWrite( LPCVOID data )
{
	if( !hDev || !hWinUsb )
		return C86CTL_ERR_NODEVICE;
	
	DWORD wlen;
	DWORD err = ::GetLastError();
	BOOL ret = WinUsb_WritePipe( hWinUsb, outPipeId, (UCHAR*)data, 64, &wlen, 0);
	err = ::GetLastError();

	if(ret == FALSE || 64 != wlen){
		WinUsb_Free(hWinUsb);
		hWinUsb = NULL;
		CloseHandle(hDev);
		hDev = NULL;
		return C86CTL_ERR_UNKNOWN;
	}
	return C86CTL_ERR_NONE;
}

void C86WinUSB::out(UCHAR idx, UINT addr, UCHAR data)
{
	uint32_t d = (idx<<17) | (addr&0x1ff)<<8 | data;
	rbuff.push(d);
}


/*----------------------------------------------------------------------------
	実装
---------------------------------------------------------------------------*/
int C86WinUSB::reset(void)
{
	BOOL retval;
	WINUSB_SETUP_PACKET setup;
	setup.RequestType = 0x40;
	setup.Request = C86_VENDOR_SYSTEM_RESET;
	setup.Index = 0;
	setup.Value = 0;
	setup.Length = 0;

	::EnterCriticalSection(&csection);
	retval = ::WinUsb_ControlTransfer( hWinUsb, setup, NULL, 0, NULL, NULL );
	::LeaveCriticalSection(&csection);

	return (retval==TRUE) ? C86CTL_ERR_NONE : C86CTL_ERR_UNKNOWN;
}

int C86WinUSB::isValid(void)
{
	return (hDev == 0) ? FALSE : TRUE;
}

void C86WinUSB::tick(void)
{
	int ret;
	
	LARGE_INTEGER et, ct;
	::QueryPerformanceCounter(&et);
	et.QuadPart += freq.QuadPart;

	while( !rbuff.isempty() ){
		UINT buff[32];
		UINT sz=0, i=0;

		for (sz=0; sz<16; ){
			if( !rbuff.pop(&buff[sz++]) )
				break;
			if( rbuff.isempty() )
				break;
		}

		if( sz<16 ){
			// add sync
			buff[sz++] = 0xfe000000;
			if(sz!=16)
				memset( &buff[sz], 0xff, (16-sz)*4 );
		}

		// WriteFileがスレッドセーフかどうかよく分からないので
		// 念のため保護しているが、いらない気がする。
		// (directOut()と重なる可能性がある)
		::EnterCriticalSection(&csection);
		ret = devWrite(buff);
		::LeaveCriticalSection(&csection);

		if( ret == C86CTL_ERR_NONE )
			cal+=64;

		// 1tickの処理が1msを超えたら一回抜ける
		//::QueryPerformanceCounter(&ct);
		//if( et.QuadPart<ct.QuadPart ){
		//	break;
		//}
	}

	return;
}

void C86WinUSB::update(void)
{
	if( 1 <= calcount++ ){
		cps = cal;
		cal = 0;
		calcount = 0;
	}
}

void C86WinUSB::checkConnection(void)
{
	UCHAR buff[65];
	buff[0] = 0;
	memset( buff, 0, 65 );

	::EnterCriticalSection(&csection);
	devWrite(buff);
	::LeaveCriticalSection(&csection);
};

int C86WinUSB::getBoardInfo(int boardIdx, BOARD_INFO *binfo)
{
	if (boardIdx < 0 || boardIdx > 3)
		return C86CTL_ERR_INVALID_PARAM;
	if (!binfo)
		return C86CTL_ERR_INVALID_PARAM;

	USHORT msg_sz = 24;
	UCHAR *msg = new UCHAR[msg_sz];
	if (!msg) return C86CTL_ERR_UNKNOWN;
	
	WINUSB_SETUP_PACKET setup;
	setup.RequestType = 0xc0;
	setup.Request = C86_VENDOR_GET_BOARD_INFO;
	setup.Index = boardIdx;
	setup.Value = 0;
	setup.Length = msg_sz;
	
	ULONG ntransferred;
	
	::EnterCriticalSection(&csection);
	BOOL retval = ::WinUsb_ControlTransfer( hWinUsb, setup, msg, msg_sz, &ntransferred, NULL );
	::LeaveCriticalSection(&csection);

	if (retval == TRUE && ntransferred == msg_sz){
		binfo->type = *reinterpret_cast<uint32_t*>(msg+0);
		binfo->nchips = *reinterpret_cast<uint32_t*>(msg+4);
		for (int i=0; i<NMAXCHIPS; i++){
			binfo->chiptype[i] = *reinterpret_cast<uint32_t*>(msg+8+(i*4));
		}
	}else{
		retval = FALSE;
	}
	
	delete[] msg;
	
	return (retval==TRUE) ? C86CTL_ERR_NONE : C86CTL_ERR_UNKNOWN;
}

int C86WinUSB::getFWVer(UINT *major, UINT *minor, UINT *rev, UINT *build )
{
	USHORT msg_sz = 4;
	UCHAR msg[4];
	
	WINUSB_SETUP_PACKET setup;
	setup.RequestType = 0xc0;
	setup.Request = C86_VENDOR_GET_FW_VER;
	setup.Index = 0;
	setup.Value = 0;
	setup.Length = msg_sz;
	
	ULONG ntransferred;
	
	::EnterCriticalSection(&csection);
	BOOL retval = ::WinUsb_ControlTransfer( hWinUsb, setup, msg, msg_sz, &ntransferred, NULL );
	::LeaveCriticalSection(&csection);

	if (retval == TRUE && ntransferred == msg_sz){
		*major = msg[3];
		*minor = msg[2];
		*rev   = msg[1];
		*build = msg[0];
		return C86CTL_ERR_NONE;
	}else{
		return C86CTL_ERR_UNKNOWN;
	}
}


// -------------------------------------------------------------------------------
C86WinUSB::C86ModuleWinUSB::C86ModuleWinUSB(C86WinUSB *device, int slotidx, int chipidx, ChipType chipType)
	: devif(device), chiptype(chipType), slotidx(slotidx), chipidx(chipidx),
	  devidx( ((slotidx&0x3)<<3) | (chipidx&0x7) )
{
}

C86WinUSB::C86ModuleWinUSB::~C86ModuleWinUSB()
{
}

void C86WinUSB::C86ModuleWinUSB::byteOut( UINT addr, UCHAR data )
{
	devif->out(devidx, addr, data);
}


void C86WinUSB::C86ModuleWinUSB::directOut(UINT addr, UCHAR data)
{
	uint32_t d = (devidx<<17) | (addr&0xffff)<<8 | data;
	devif->sendMsg(&d, 1);
}

CBUS_BOARD_TYPE C86WinUSB::C86ModuleWinUSB::getBoardType()
{
	BOARD_INFO binfo;
	if (C86CTL_ERR_NONE == devif->getBoardInfo(slotidx, &binfo)){
		return static_cast<CBUS_BOARD_TYPE>(binfo.type);
	}else
		return CBUS_BOARD_UNKNOWN;
}

#endif


