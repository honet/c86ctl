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

namespace c86ctl{

// デバイスドライバのinf内で定義したGUID
// (WinUSB.sys使用デバイスに対する識別子）
// {63275336-530B-4069-92B6-5F8AE3465462}
DEFINE_GUID(GUID_DEVINTERFACE_GIMIC_WINUSB_TARGET, 
  0x63275336, 0x530b, 0x4069, 0x92, 0xb6, 0x5f, 0x8a, 0xe3, 0x46, 0x54, 0x62);


class GimicWinUSB : public BaseSoundDevice, public IFirmwareVersionInfo
{
public:
	// ---------------------------------------------------------
	class GimicModuleWinUSB : public BaseSoundModule
	{
	public:
		GimicModuleWinUSB(GimicWinUSB *device, int idx, ChipType chipType);
		virtual ~GimicModuleWinUSB();

	public:
		// IByteInput
		// -- before filter
		virtual void byteOut( UINT addr, UCHAR data );
		
	public:
		virtual void directOut( UINT addr, UCHAR data );
		
	public:
		// IGimicModule
		// -- without filter
		virtual int setSSGVolume(UCHAR vol);
		virtual int getSSGVolume(UCHAR *vol);
		virtual int setPLLClock(UINT clock);
		virtual int getPLLClock(UINT *clock);
		virtual int getChipStatus( UINT addr, UCHAR *status );
		virtual int getModuleInfo(struct Devinfo *info);
		virtual const GimicParam* getGimicParam();

	public:
		// override to BaseSoundModule
		virtual enum ChipType getChipType() { return chiptype; };
		virtual int isValid(void){ return devif->isValid(); };
		virtual std::basic_string<TCHAR> getNodeId();

		virtual BaseSoundDevice* getParentDevice(){
			return static_cast<BaseSoundDevice*>(devif);
		};

//	public:
//		virtual IChip* getChip() { return chip; };
//		void directOut(UCHAR idx, UINT addr, UCHAR data);
		
	private:
		GimicWinUSB *devif;
		int devidx;
		
	private:
		//Chip *chip;
		ChipType chiptype;
		GimicParam gimicParam;
	};


public:
	static int UpdateInstances( withlock< std::vector< std::shared_ptr<BaseSoundDevice> > > &gimics);

private:
	GimicWinUSB();
	bool OpenDevice(std::basic_string<TCHAR> devpath);
	
public:
	~GimicWinUSB(void);

public:
	// override to BaseSoundDevice
	virtual int reset(void);
	virtual void tick(void);
	virtual void update(void);
	virtual UINT getCPS(void){ return cps; };
	
	virtual int isValid(void);
	virtual void checkConnection(void);
	virtual std::basic_string<TCHAR> getNodeId();

	virtual BaseSoundModule* getModule(int id){
		if(id<0 || id>nmodules)
			return NULL;
		return modules[id];
	};
	virtual int getNumberOfModules(){
		return nmodules;
	}

public:
	virtual int getMBInfo(struct Devinfo *info);
	virtual int getModuleInfo(UCHAR idx, struct Devinfo *info);
	virtual int getFWVer(UINT *major, UINT *minor, UINT *rev, UINT *build );
	
//	virtual int setDelay(int d);
//	virtual int getDelay(int *d);
	

// プライベート -----------------------------------------------------
private:
	struct MSG {
		// なんとなく合計2-DWORDになるようにしてみた。
		UCHAR len;
		UCHAR dat[7];	// 最大メッセージ長は7byte.
	};
	
private:
	int sendMsg( MSG *data );
	int transaction( MSG *txdata, uint8_t *rxdata, uint32_t rxsz, bool ignore_error=false );
	int devRead( LPVOID data, bool ignore_error=false );
	int devWrite( LPCVOID data );
	
	void out2buf(UCHAR idx, UINT addr, UCHAR data);
	void out(UCHAR idx, UINT addr, UCHAR data);
	
	
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
	CRingBuff<MSG> rbuff;
	UINT cps, cal, calcount;

	LARGE_INTEGER freq;
	
private:
	static const int NMAXCHIP=8;
	int nmodules;
	GimicModuleWinUSB *modules[NMAXCHIP];

protected:
	int refcount;


};

typedef std::shared_ptr<GimicWinUSB> GimicWinUSBPtr;



};

#endif

