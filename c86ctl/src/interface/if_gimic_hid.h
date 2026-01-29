/***
	c86ctl
	gimic コントロール HID版
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
	Thanks to Nagai "Guu" Osamu 2011/12/08 for his advice.
 */
#pragma once

#include "if.h"

#ifdef SUPPORT_HID

#include <mmsystem.h>
#include <vector>
#include "ringbuff.h"
#include "withlock.h"
#include "chip/chip.h"

namespace c86ctl {

class GimicHID : public BaseSoundDevice, public IFirmwareVersionInfo
{
public:
	class GimicModuleHID : public BaseSoundModule 
	{
	public:
		GimicModuleHID(GimicHID* device, int boardidx, int chipidx, ChipType chipType);
		virtual ~GimicModuleHID();

	public:
		// IByteInput
		virtual void byteOut(UINT addr, UCHAR data);

	public:
		virtual void directOut(UINT addr, UCHAR data);

	public:
		virtual int setSSGVolume(UCHAR vol);
		virtual int getSSGVolume(UCHAR* vol);
		virtual int setPLLClock(UINT clock);
		virtual int getPLLClock(UINT* clock);
		virtual int getChipStatus(UINT addr, UCHAR* status);
		virtual int getModuleInfo(struct Devinfo* info);
		virtual const GimicParam* getGimicParam();

	public:
		// override to BaseSoundModule
		virtual enum ChipType getChipType() { return chiptype; }
		virtual int isValid(void) { return devif->isValid(); }
		virtual std::basic_string<TCHAR> getNodeId();

		virtual BaseSoundDevice* getParentDevice() {
			return static_cast<BaseSoundDevice*>(devif);
		}

	private:
		GimicHID* devif;
		int devidx;
		int boardidx;
		int chipidx;

		//IChip* chip;
		ChipType chiptype;
		GimicParam gimicParam;
	};


public:
	static int UpdateInstances(withlock< std::vector< std::shared_ptr<BaseSoundDevice> > >& gimics);

// C86CTL内部利用 ---------------------------------------------------
private:
	GimicHID(HANDLE h);

public:
	~GimicHID(void);

public:
	// override to BaseSoundDevice
	virtual int reset(void);
	virtual void tick(void);
	virtual void update(void);
	virtual UINT getCPS(void) { return cps; }

	virtual int isValid(void);
	virtual void checkConnection(void);
	virtual std::basic_string<TCHAR> getNodeId();

	virtual BaseSoundModule* getModule(int id) {
		if (id < 0 || id > nmodules)
			return NULL;
		return modules[id];
	}
	virtual int getNumberOfModules() { return nmodules; }

public:
	virtual int getMBInfo(struct Devinfo* info);
	virtual int getModuleInfo(UCHAR idx, struct Devinfo* info);
	virtual int getFWVer(UINT* major, UINT* minor, UINT* rev, UINT* build);
	
private:
	bool CreateModules(void);

// プライベート -----------------------------------------------------
private:
	struct MSG {
		// なんとなく合計2-DWORDになるようにしてみた。
		UCHAR len;
		UCHAR dat[7];	// 最大メッセージ長は今のところ6byte.
	};
	
private:
	int sendMsg( MSG *data );
	int transaction( MSG *txdata, uint8_t *rxdata, uint32_t rxsz );
	
	int devWrite(LPCVOID data);
	int devRead(LPVOID data);

	void out2buf(UCHAR idx, UINT addr, UCHAR data);
	void out(UCHAR idx, UINT addr, UCHAR data);

private:
	HANDLE hHandle;
	std::basic_string<TCHAR> devPath;

	CRITICAL_SECTION csection;
	CRingBuff<MSG> rbuff;
	UINT cps, cal, calcount;

	static const int NMAXCHIP = 8;
	int nmodules;
	GimicModuleHID *modules[NMAXCHIP];
	

protected:
	int refcount;
};

typedef std::shared_ptr<GimicHID> GimicHIDPtr;

};

#endif

