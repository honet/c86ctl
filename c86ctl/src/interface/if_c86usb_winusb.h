/***
	c86ctl
	gimic コントロール WinUSB版
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#pragma once

#include "if.h"

#ifdef SUPPORT_WINUSB

#include <mmsystem.h>
#include <winusb.h>
#include <vector>
#include "ringbuff.h"
#include "withlock.h"
#include "chip/chip.h"

namespace c86ctl {


// デバイスドライバのinf内で定義したGUID
// (WinUSB.sys使用デバイスに対する識別子）
// {b0320d09-0791-4c3f-a741-9ef97c8885d6}
DEFINE_GUID(GUID_DEVINTERFACE_C86BOX_WINUSB_TARGET,
	0xb0320d09, 0x0791, 0x4c3f, 0xa7, 0x41, 0x9e, 0xf9, 0x7c, 0x88, 0x85, 0xd6);

class C86WinUSB : public BaseSoundDevice, public IFirmwareVersionInfo
{
public:
	// ---------------------------------------------------------
	class C86ModuleWinUSB : public BaseSoundModule
	{
	public:
		C86ModuleWinUSB(C86WinUSB* device, int slotidx, int chipidx, ChipType chipType);
		virtual ~C86ModuleWinUSB();

	public:
		// IByteInput
		// -- before filter
		virtual void byteOut(UINT addr, UCHAR data);

	public:
		virtual void directOut(UINT addr, UCHAR data);

	public:
		// override to BaseSoundModule
		virtual enum ChipType getChipType() { return chiptype; }
		virtual int isValid(void) { return devif->isValid(); }
		//		virtual std::basic_string<TCHAR> getNodeId();

		virtual BaseSoundDevice* getParentDevice() {
			return static_cast<BaseSoundDevice*>(devif);
		}

	public:
		int getSlotIndex() { return slotidx; }
		int getChipIndex() { return chipidx; }
		CBUS_BOARD_TYPE getBoardType();

	private:
		C86WinUSB* devif;
		int devidx;
		int slotidx;
		int chipidx;

	private:
		//Chip *chip;
		ChipType chiptype;
	};


	// ---------------------------------------------------------
public:
	static const UINT NMAXCHIPS = 4;
	static const UINT NMAXBOARDS = 2;

public:
	static int UpdateInstances(withlock< std::vector< std::shared_ptr<BaseSoundDevice> > >& devices);

private:
	C86WinUSB();
	bool OpenDevice(std::basic_string<TCHAR> devpath);

public:
	~C86WinUSB(void);

public:
	// override to BaseSoundDevice
	virtual int reset(void);
	virtual void tick(void);
	virtual void update(void);
	virtual UINT getCPS(void) { return cps; }

	virtual int isValid(void);
	virtual void checkConnection(void);
	//	virtual std::basic_string<TCHAR> getNodeId();

	virtual BaseSoundModule* getModule(int id) {
		if (id<0 || id>nmodules)
			return NULL;
		return modules[id];
	}
	virtual int getNumberOfModules() {
		return nmodules;
	}

public:
	virtual int getFWVer(UINT* major, UINT* minor, UINT* rev, UINT* build);

	// private -----------------------------------------------------
private:
	struct BOARD_INFO {
		UINT type;
		UINT nchips;
		UINT chiptype[NMAXCHIPS];
	};

private:
	int sendMsg(UINT* data, UINT size);
	int devWrite(LPCVOID data);
	void out(UCHAR idx, UINT addr, UCHAR data);

	int getBoardInfo(int boardIdx, BOARD_INFO* binfo);

private:
	HANDLE hDev;
	WINUSB_INTERFACE_HANDLE hWinUsb;
	std::basic_string<TCHAR> devPath;

	UCHAR inPipeId;
	USHORT inPipeMaxPktSize;
	UCHAR outPipeId;
	USHORT outPipeMaxPktSize;

private:
	CRITICAL_SECTION csection;
	CRingBuff<UINT> rbuff;
	UINT cps, cal, calcount;

	LARGE_INTEGER freq;

private:
	static const int NMAXCHIP = 8;
	int nmodules;
	C86ModuleWinUSB* modules[NMAXCHIP];

protected:
	int refcount;


};

typedef std::shared_ptr<C86WinUSB> C86WinUSBPtr;

}

#endif

