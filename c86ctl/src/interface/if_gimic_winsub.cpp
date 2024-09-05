/***
	c86ctl
	gimic コントロール WinUSB版
	
	Copyright (c) 2009-2013, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com

	note: honet.kk
	WinUSBドライバによるバルクダンプ転送方式。

	HID版では1フレーム(1ms)に1回の転送しか行えなかったが、
	WinUSB版では十数回（理論最大値）まで
	1ms内に転送を行えるため、転送速度が速くなっている。

	ただし、USB転送速度よりもチップ書き込み速度のほうが遅いので
	その制限によってある程度の速度で頭打ちとなる。

	また、電文及び時間分解能等の仕組みはHID版と同じで有るため、
	USB伝送速度以外の違いはHID版とWinUSB版の間には無い。
	
	-- HID版のコード流用するためにこのような仕様にしてしまったが、
	-- 今となってはsetPLL等のコントロール系はEP0(ControlTransfer)を
	-- 使うべきであったと若干後悔している・・・。

	使用するにはgimicのファーム変更（デスクリプタ変更）と
	winusbドライバのインストールが必要。
*/


#include "stdafx.h"
#include "if_gimic_winusb.h"

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
	パラメタ
----------------------------------------------------------------------------*/
#define PIPE_BUFFER_SIZE 64


/*----------------------------------------------------------------------------
	コンストラクタ
----------------------------------------------------------------------------*/
GimicWinUSB::GimicWinUSB()
	: hDev(0), hWinUsb(0), cps(0), cal(0), calcount(0),
	inPipeId(0), outPipeId(0), inPipeMaxPktSize(0), outPipeMaxPktSize(0), nmodules(0)
{
	rbuff.alloc(128);
	::InitializeCriticalSection(&csection);

	::QueryPerformanceFrequency(&freq);
	freq.QuadPart /= 1000; // 1ms

	memset(modules, 0, sizeof(modules));
}

/*----------------------------------------------------------------------------
	デストラクタ
----------------------------------------------------------------------------*/
GimicWinUSB::~GimicWinUSB(void)
{
	::DeleteCriticalSection(&csection);
	CloseHandle(hDev);
	WinUsb_Free(hWinUsb);
	hDev = NULL;
	hWinUsb = NULL;
	for (int i = 0; i < nmodules; i++)
		if (modules[i])
			delete modules[i];
}

/*----------------------------------------------------------------------------
	Device Open
----------------------------------------------------------------------------*/
bool GimicWinUSB::OpenDevice(std::basic_string<TCHAR> devpath)
{
	HANDLE hNewDev = CreateFile(
		devpath.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0 /*FILE_SHARE_READ|FILE_SHARE_WRITE*/,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED, //FILE_FLAG_NO_BUFFERING,
		NULL);

	if (hNewDev == INVALID_HANDLE_VALUE)
		return false;

	HANDLE hNewWinUsb = NULL;
	if (!WinUsb_Initialize(hNewDev, &hNewWinUsb)) {
		//DWORD err = GetLastError();
		CloseHandle(hNewDev);
		return false;
	}

	// エンドポイント情報取得
	USB_INTERFACE_DESCRIPTOR desc;
	if (!WinUsb_QueryInterfaceSettings(hNewWinUsb, 0, &desc)) {
		WinUsb_Free(hNewWinUsb);
		CloseHandle(hNewDev);
		return false;
	}

	for (int i = 0; i < desc.bNumEndpoints; i++) {
		WINUSB_PIPE_INFORMATION pipeInfo;
		if (WinUsb_QueryPipe(hNewWinUsb, 0, (UCHAR)i, &pipeInfo)) {
			if (pipeInfo.PipeType == UsbdPipeTypeBulk &&
				USB_ENDPOINT_DIRECTION_OUT(pipeInfo.PipeId)) {
				outPipeId = pipeInfo.PipeId;
				outPipeMaxPktSize = pipeInfo.MaximumPacketSize;
			} else if (pipeInfo.PipeType == UsbdPipeTypeBulk &&
				USB_ENDPOINT_DIRECTION_IN(pipeInfo.PipeId)) {
				inPipeId = pipeInfo.PipeId;
				inPipeMaxPktSize = pipeInfo.MaximumPacketSize;
			}
		}
	}

	// タイムアウト設定
	ULONG timeout = 500; //ms
	::WinUsb_SetPipePolicy(hNewWinUsb, outPipeId, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);
	::WinUsb_SetPipePolicy(hNewWinUsb, inPipeId, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);

	// ここでハンドル更新
	hDev = hNewDev;
	hWinUsb = hNewWinUsb;
	devPath = devpath;

	WinUsb_FlushPipe(hWinUsb, outPipeId);
	WinUsb_FlushPipe(hWinUsb, inPipeId);

	// Module情報取得 -----------
	int n = 0;
	for (int i = 0; i < NMAXCHIP; i++) {
		Devinfo info;
		if (C86CTL_ERR_NONE != getModuleInfo(i, &info)) {
			break;
		}
		// この仕様はなかなかマゾい。
		if (!memcmp(info.Devname, "GMC-OPN3L", 9)) {
			if (modules[n] == 0) {
				modules[n++] = new GimicModuleWinUSB(this, i, 0, CHIP_OPN3L);
			} else if (modules[n]->getChipType() != CHIP_OPN3L) {
				goto MODULE_CHANGED;
			}
		} else if (!memcmp(info.Devname, "GMC-OPM", 7)) {
			if (modules[n] == 0) {
				modules[n] = new GimicModuleWinUSB(this, i, 0, CHIP_OPM);
			} else if (modules[n]->getChipType() != CHIP_OPM) {
				goto MODULE_CHANGED;
			}
		} else if (!memcmp(info.Devname, "GMC-OPNA", 8)) {
			if (modules[n] == 0) {
				modules[n] = new GimicModuleWinUSB(this, i, 0, CHIP_OPNA);
			} else if (modules[i]->getChipType() != CHIP_OPNA) {
				goto MODULE_CHANGED;
			}
		} else if (!memcmp(info.Devname, "GMC-OPL3", 8)) {
			if (modules[n] == 0) {
				modules[n] = new GimicModuleWinUSB(this, i, 0, CHIP_OPL3);
			} else if (modules[n]->getChipType() != CHIP_OPL3) {
				goto MODULE_CHANGED;
			}
		} else if (!memcmp(info.Devname, "GMC-OPLL", 8)) {
			if (modules[n] == 0) {
				modules[n] = new GimicModuleWinUSB(this, i, 0, CHIP_OPLL);
			} else if (modules[n]->getChipType() != CHIP_OPLL) {
				goto MODULE_CHANGED;
			}
		} else if (!memcmp(info.Devname, "GMC-OPLMN", 9)) {
			if (modules[n] == 0) {
				modules[n++] = new GimicModuleWinUSB(this, i, 0, CHIP_YMF297_OPN3L);
				modules[n++] = new GimicModuleWinUSB(this, i, 0, CHIP_YMF297_OPL3);
				modules[n++] = new GimicModuleWinUSB(this, i, 1, CHIP_OPM);
			} else if (modules[n]->getChipType() != CHIP_YMF297_OPL3) {
				goto MODULE_CHANGED;
			}
			//	}else if( !memcmp( info.Devname, "GMC-SPC", 8 ) ){
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
int GimicWinUSB::UpdateInstances(withlock< std::vector< std::shared_ptr<BaseSoundDevice> > >& gimics)
{
	gimics.lock();
	std::for_each(gimics.begin(), gimics.end(), [](std::shared_ptr<BaseSoundDevice> x) { x->checkConnection(); });

	BOOL bResult = TRUE;

	HDEVINFO devinf = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DATA spid;
	PSP_DEVICE_INTERFACE_DETAIL_DATA fc_data = NULL;


	devinf = SetupDiGetClassDevs(
		(LPGUID)&GUID_DEVINTERFACE_GIMIC_WINUSB_TARGET,
		NULL,
		0,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (devinf) {
		for (int i = 0; ; i++) {
			ZeroMemory(&spid, sizeof(spid));
			spid.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
			if (!SetupDiEnumDeviceInterfaces(devinf, NULL,
				(LPGUID)&GUID_DEVINTERFACE_GIMIC_WINUSB_TARGET, i, &spid)) {
				break;
			}

			unsigned long sz;
			std::basic_string<TCHAR> devpath;

			// 必要なバッファサイズ取得
			bResult = SetupDiGetDeviceInterfaceDetail(devinf, &spid, NULL, 0, &sz, NULL);
			PSP_INTERFACE_DEVICE_DETAIL_DATA dev_det = (PSP_INTERFACE_DEVICE_DETAIL_DATA)(malloc(sz));
			dev_det->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

			// デバイスノード取得
			if (!SetupDiGetDeviceInterfaceDetail(devinf, &spid, dev_det, sz, &sz, NULL)) {
				free(dev_det);
				break;
			}

			devpath = dev_det->DevicePath;
			free(dev_det);
			dev_det = NULL;

			// 既にインスタンスがあるかどうか検索
			auto it = std::find_if(gimics.begin(), gimics.end(),
				[devpath](std::shared_ptr<BaseSoundDevice> x) -> bool {
				GimicWinUSB* gdev = dynamic_cast<GimicWinUSB*>(x.get());
				if (!gdev) return false;
				if (gdev->devPath != devpath) return false;
				return true;
			}
			);

			if (it == gimics.end()) {
				GimicWinUSB* gimicDev = new GimicWinUSB();
				if (gimicDev) {
					if (gimicDev->OpenDevice(devpath)) {
						gimics.push_back(BaseSoundDevicePtr(gimicDev));
					} else {
						delete gimicDev;
					}
				}
			} else if (!(*it)->isValid()) {
				GimicWinUSB* gimicDev = dynamic_cast<GimicWinUSB*>(it->get());
				if (!gimicDev->OpenDevice(devpath)) {
					// OpenDeviceが失敗した場合は音源モジュールが前回接続時と
					// 異なっているため、別インスタンスを生成する
					GimicWinUSB* gimicDev = new GimicWinUSB();
					if (gimicDev) {
						if (gimicDev->OpenDevice(devpath)) {
							gimics.push_back(BaseSoundDevicePtr(gimicDev));
						} else {
							delete gimicDev;
						}
					}
				}
			}
		}

		SetupDiDestroyDeviceInfoList(devinf);
	}

	gimics.unlock();
	return 0;
}


/*----------------------------------------------------------------------------
	internal.
----------------------------------------------------------------------------*/
int GimicWinUSB::sendMsg(MSG* data)
{
	UCHAR buff[66];
	int ret = C86CTL_ERR_UNKNOWN;

	UINT sz = data->len;
	if (0 < sz) {
		memcpy(&buff[0], &data->dat[0], sz);
		if (sz < 64)
			memset(&buff[sz], 0xff, 64 - sz);

		::EnterCriticalSection(&csection);
		ret = devWrite(buff);
		::LeaveCriticalSection(&csection);
	}

	return ret;
}

int GimicWinUSB::transaction(MSG* txdata, uint8_t* rxdata, uint32_t rxsz, bool ignore_error)
{
	UCHAR buff[66];
	DWORD len = 0;
	int ret = C86CTL_ERR_UNKNOWN;

	::EnterCriticalSection(&csection);
	{
		UINT sz = txdata->len;
		if (0 < sz) {
			memcpy(&buff[0], &txdata->dat[0], sz);
			if (sz < 64)
				memset(&buff[sz], 0xff, 64 - sz);

			ret = devWrite(buff);
		}

		if (C86CTL_ERR_NONE == ret) {
			len = 0;
			ret = devRead(buff, ignore_error);
			if (C86CTL_ERR_NONE == ret)
				memcpy(rxdata, &buff[0], rxsz);
		}
	}
	::LeaveCriticalSection(&csection);

	return ret;
}

int GimicWinUSB::devRead(LPVOID data, bool ignore_error)
{
	if (!hDev || !hWinUsb)
		return C86CTL_ERR_NODEVICE;

	DWORD rlen;
	BOOL ret = WinUsb_ReadPipe(hWinUsb, inPipeId, (UCHAR*)data, 64, &rlen, 0);

	if (ret == FALSE) {
		if (ignore_error == false) {
			WinUsb_Free(hWinUsb);
			hWinUsb = NULL;
			CloseHandle(hDev);
			hDev = NULL;
		}
		return C86CTL_ERR_UNKNOWN;
	}
	return C86CTL_ERR_NONE;
}

int GimicWinUSB::devWrite(LPCVOID data)
{
	if (!hDev || !hWinUsb)
		return C86CTL_ERR_NODEVICE;

	DWORD wlen;
	BOOL ret = WinUsb_WritePipe(hWinUsb, outPipeId, (UCHAR*)data, 64, &wlen, 0);

	if (ret == FALSE || 64 != wlen) {
		WinUsb_Free(hWinUsb);
		hWinUsb = NULL;
		CloseHandle(hDev);
		hDev = NULL;
		return C86CTL_ERR_UNKNOWN;
	}
	return C86CTL_ERR_NONE;
}

void GimicWinUSB::out2buf(UCHAR idx, UINT addr, UCHAR data)
{
	if (idx >= nmodules)
		return;

	if (addr < 0x200) {
		switch (modules[idx]->getChipType()) {
		case CHIP_OPNA:
		case CHIP_OPN3L:
			if (0x100 <= addr && addr <= 0x110)
				addr -= 0x40;
			break;
		case CHIP_OPM:
			if (0xfc <= addr && addr <= 0xff)
				addr -= 0xe0;
			break;
		}
		if (addr < 0xfc) {
			MSG d = { 2, { addr & 0xff, data } };
			rbuff.push(d);
		} else if (0x100 <= addr && addr <= 0x1fb) {
			MSG d = { 3, { 0xfe, addr & 0xff, data } };
			rbuff.push(d);
		}
	} else {
		MSG d = { 4, { 0xfc, (addr >> 8) & 0xff, addr & 0xff, data } };
		rbuff.push(d);
	}
}

void GimicWinUSB::out(UCHAR idx, UINT addr, UCHAR data)
{
	if (idx >= nmodules)
		return;

	out2buf(idx, addr, data);
}


/*----------------------------------------------------------------------------
	実装
---------------------------------------------------------------------------*/
int GimicWinUSB::reset(void)
{
	int ret;

	// リセットコマンド送信
	MSG d = { 2, { 0xfd, 0x82, 0 } };
	ret = sendMsg(&d);

	if (C86CTL_ERR_NONE == ret) {
		// 各ステータス値リセット
		//   マスクの適用をreset内でする（送信処理が発生する）ので
		//   リセット後に処理しないとダメ。
		for (int i = 0; i < nmodules; i++) {
			//if( modules[i] )
			//	modules[i]->chipmodel_reset();
		}
	}

	return ret;
}

int GimicWinUSB::isValid(void)
{
	return (hDev == 0) ? FALSE : TRUE;
}


int GimicWinUSB::getMBInfo(struct Devinfo* info)
{
	int ret;

	if (!info)
		return C86CTL_ERR_INVALID_PARAM;

	MSG d = { 3, { 0xfd, 0x91, 0xff } };
	if (C86CTL_ERR_NONE == (ret = transaction(&d, (uint8_t*)info, 32))) {
		char* p = &info->Devname[15];
		while (*p == 0 || *p == -1) *p-- = 0;
		p = &info->Serial[14];
		while (*p == 0 || *p == -1) *p-- = 0;
	}
	return ret;
}

int GimicWinUSB::getModuleInfo(UCHAR idx, struct Devinfo* info)
{
	int ret;

	if (!info)
		return C86CTL_ERR_INVALID_PARAM;

	MSG d = { 3, { 0xfd, 0x91, idx } };
	if (C86CTL_ERR_NONE == (ret = transaction(&d, (uint8_t*)info, 32, true))) {
		char* p = &info->Devname[15];
		while (*p == 0 || *p == -1) *p-- = 0;
		p = &info->Serial[14];
		while (*p == 0 || *p == -1) *p-- = 0;
	}

	return ret;
}

int GimicWinUSB::getFWVer(UINT* major, UINT* minor, UINT* rev, UINT* build)
{
	uint8_t rx[16];
	MSG d = { 2, { 0xfd, 0x92 } };
	int ret;

	if (C86CTL_ERR_NONE == (ret = transaction(&d, rx, 16))) {
		if (major) *major = *((uint32_t*)&rx[0]);
		if (minor) *minor = *((uint32_t*)&rx[4]);
		if (rev)   *rev = *((uint32_t*)&rx[8]);
		if (build) *build = *((uint32_t*)&rx[12]);
	}
	return ret;
}


void GimicWinUSB::tick(void)
{
	int ret;

	LARGE_INTEGER et, ct;
	::QueryPerformanceCounter(&et);
	et.QuadPart += freq.QuadPart;
	while (!rbuff.isempty()) {
		UCHAR buff[128];
		UINT sz = 0, i = 0;
		MSG d;

		for (;;) {
			UINT l = rbuff.front()->len;
			if (64 < (sz + l))
				break;
			if (!rbuff.pop(&d))
				break;
			sz += d.len;
			for (UINT j = 0; j < d.len; j++)
				buff[i++] = d.dat[j];
			if (rbuff.isempty())
				break;
		}

		if (sz < 64)
			memset(&buff[sz], 0xff, 64 - sz);

		// WriteFileがスレッドセーフかどうかよく分からないので
		// 念のため保護しているが、いらない気がする。
		// (directOut()と重なる可能性がある)
		::EnterCriticalSection(&csection);
		ret = devWrite(buff);
		::LeaveCriticalSection(&csection);

		if (ret == C86CTL_ERR_NONE)
			cal += 64;

		// 1tickの処理が1msを超えたら一回抜ける
		::QueryPerformanceCounter(&ct);
		if (et.QuadPart < ct.QuadPart) {
			break;
		}
	}

	return;
}

void GimicWinUSB::update(void)
{
//	for( int i=0; i<nmodules; i++ ){
//		if( modules[i] )
//			modules[i]->chipmodel_update();
//	}

	if (1 <= calcount++) {
		cps = cal;
		cal = 0;
		calcount = 0;
	}
}

void GimicWinUSB::checkConnection(void)
{
	UCHAR buff[65];
	buff[0] = 0;
	memset( buff, 0, 65 );

	::EnterCriticalSection(&csection);
	devWrite(buff);
	::LeaveCriticalSection(&csection);
}

std::basic_string<TCHAR> GimicWinUSB::getNodeId(){
	return devPath;
}




// -------------------------------------------------------------------------------
GimicWinUSB::GimicModuleWinUSB::GimicModuleWinUSB(GimicWinUSB* device, int boardidx, int chipidx, ChipType chipType)
	: devif(device), devidx(boardidx), chipidx(chipidx), chiptype(chipType)
{
	// 値をキャッシュさせるためのダミー呼び出し
	UCHAR vol;
	getSSGVolume(&vol);
	UINT clock;
	getPLLClock(&clock);
}

GimicWinUSB::GimicModuleWinUSB::~GimicModuleWinUSB()
{
}

void GimicWinUSB::GimicModuleWinUSB::byteOut(UINT addr, UCHAR data)
{
	devif->out(devidx, addr, data);
}

int GimicWinUSB::GimicModuleWinUSB::setSSGVolume(UCHAR vol)
{
	if (chiptype != CHIP_OPNA)
		return C86CTL_ERR_UNSUPPORTED;

	gimicParam.ssgVol = vol;
	if (devidx == 0) {
		MSG d = { 3, { 0xfd, 0x84, vol } };
		return devif->sendMsg(&d);
	} else {
		MSG d = { 4, { 0xfd, 0x88, static_cast<UCHAR>(devidx), vol } };
		return devif->sendMsg(&d);
	}
}

int GimicWinUSB::GimicModuleWinUSB::getSSGVolume(UCHAR* vol)
{
	if (chiptype != CHIP_OPNA)
		return C86CTL_ERR_UNSUPPORTED;
	if (!vol)
		return C86CTL_ERR_INVALID_PARAM;

	int ret;
	if (devidx == 0) {
		MSG d = { 2, { 0xfd, 0x86 } };
		ret = devif->transaction(&d, (uint8_t*)vol, 1);
	} else {
		MSG d = { 3, { 0xfd, 0x8A, static_cast<UCHAR>(devidx) } };
		ret = devif->transaction(&d, (uint8_t*)vol, 1);
	}

	if (C86CTL_ERR_NONE == ret)
		gimicParam.ssgVol = *vol;

	return ret;
}

int GimicWinUSB::GimicModuleWinUSB::setPLLClock(UINT clock)
{
	if (chiptype != CHIP_OPNA && chiptype != CHIP_OPM && chiptype != CHIP_OPL3)
		return C86CTL_ERR_UNSUPPORTED;

	int ret;
	gimicParam.clock = clock;
	if (devidx == 0) {
		MSG d = { 6, { 0xfd, 0x83, clock & 0xff, (clock >> 8) & 0xff, (clock >> 16) & 0xff, (clock >> 24) & 0xff, 0 } };
		ret = devif->sendMsg(&d);
	} else {
		MSG d = { 7, { 0xfd, 0x87, static_cast<UCHAR>(devidx), clock & 0xff, (clock >> 8) & 0xff, (clock >> 16) & 0xff, (clock >> 24) & 0xff } };
		ret = devif->sendMsg(&d);
	}
	return ret;
}

int GimicWinUSB::GimicModuleWinUSB::getPLLClock(UINT* clock)
{
	if (chiptype != CHIP_OPNA && chiptype != CHIP_OPM && chiptype != CHIP_OPL3)
		return C86CTL_ERR_UNSUPPORTED;

	if (!clock)
		return C86CTL_ERR_INVALID_PARAM;

	int ret;
	if (devidx == 0) {
		MSG d = { 2, { 0xfd, 0x85 } };
		ret = devif->transaction(&d, (uint8_t*)clock, 4);
	} else {
		MSG d = { 3, { 0xfd, static_cast<UCHAR>(devidx), 0x89 } };
		ret = devif->transaction(&d, (uint8_t*)clock, 4);
	}

	if (ret == C86CTL_ERR_NONE) {
		if (gimicParam.clock != *clock) {
			gimicParam.clock = *clock;
		}
	}
	return ret;
}



/*
int GimicWinUSB::adpcmZeroClear(void)
{
	uint8_t rx[1];
	MSG d = { 2, { 0xfd, 0xa0 } };
	int ret;

	ret = transaction( &d, rx, 1 );
	return ret;
}

int GimicWinUSB::adpcmWrite( UINT startAddr, UINT size, UCHAR *data )
{
	return C86CTL_ERR_NOT_IMPLEMENTED;
}

int GimicWinUSB::adpcmRead( UINT startAddr, UINT size, UCHAR *data )
{
	return C86CTL_ERR_NOT_IMPLEMENTED;
}
*/

int GimicWinUSB::GimicModuleWinUSB::getChipStatus(UINT addr, UCHAR* status)
{
	if (!status)
		return C86CTL_ERR_INVALID_PARAM;

	uint8_t rx[4];
	MSG d = { 4, { 0xfd, 0x94, static_cast<UCHAR>(devidx), addr & 0x01 } };
	int ret;

	if (C86CTL_ERR_NONE == (ret = devif->transaction(&d, rx, 4))) {
		*status = *((uint32_t*)&rx[0]);
	}
	return ret;
}

void GimicWinUSB::GimicModuleWinUSB::directOut(UINT addr, UCHAR data)
{
	if (devidx == 0) {
		switch (chiptype) {
		case CHIP_OPNA:
		case CHIP_OPN3L:
			if (0x100 <= addr && addr <= 0x110)
				addr -= 0x40;
			break;
		case CHIP_OPM:
			if (0xfc <= addr && addr <= 0xff)
				addr -= 0xe0;
			break;
		}
		if (addr < 0xfc) {
			MSG d = { 2, { addr & 0xff, data } };
			devif->sendMsg(&d);
		} else if (0x100 <= addr && addr <= 0x1fb) {
			MSG d = { 3, { 0xfe, addr & 0xff, data } };
			devif->sendMsg(&d);
		}
	} else {
		if (addr < 0x100) {
			MSG d = { 4, { 0xfc, static_cast<UCHAR>(2 * devidx), addr & 0xff, data } };
			devif->sendMsg(&d);
		} else if (addr < 0x200) {
			MSG d = { 4, { 0xfc, static_cast<UCHAR>(2 * devidx + 1), addr & 0xff, data } };
			devif->sendMsg(&d);
		}
	}
}

int GimicWinUSB::GimicModuleWinUSB::getModuleInfo( struct Devinfo *info )
{
	return devif->getModuleInfo(devidx, info);
}

const GimicParam* GimicWinUSB::GimicModuleWinUSB::getGimicParam()
{
	return &gimicParam;
}

std::basic_string<TCHAR> GimicWinUSB::GimicModuleWinUSB::getNodeId()
{
	TCHAR buf[128];
	_sntprintf(buf, 128, _T("\\%d"), devidx);
//	_sntprintf_s(buf, 128, _TRUNCATE, _T("\\%d"), devidx);	// Not tested
	return devif->getNodeId() + buf;
}

#endif

					