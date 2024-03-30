/***
	c86ctl
	I/Fアクセスインタフェース定義 for GIMIC
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
	Thanks to Nagai "Guu" Osamu 2011/12/08 for his advice.
 */
#pragma once
#include <memory>
#include <string>
#include "c86ctl.h"

//#define SUPPORT_MIDI <=全然メンテしていません
#define SUPPORT_WINUSB
//#define SUPPORT_HID

namespace c86ctl{

class GimicParam;

// ------------------------------------
interface IComponentControl
{
public:
	virtual void reset(void) = 0;
	virtual void tick(void) = 0;
};

interface IByteInput
{
public:
	virtual void byteOut(UINT addr, UCHAR data) = 0;
};

/*
interface IGimicModule
{
public:
	virtual int setSSGVolume(UCHAR vol) = 0;
	virtual int getSSGVolume(UCHAR *vol) = 0;
	virtual int setPLLClock(UINT clock) = 0;
	virtual int getPLLClock(UINT *clock) = 0;
	virtual int getChipStatus(UINT addr, UCHAR *status) = 0;
	virtual int getModuleInfo(struct Devinfo *info) = 0;
	virtual const GimicParam* getGimicParam() = 0;
};

interface IGimicDevice
{
public:
	virtual int getFWVer( UINT *major, UINT *minor, UINT *rev, UINT *build ) = 0;
	virtual int getMBInfo(struct Devinfo *info) = 0;
};
*/
interface IFirmwareVersionInfo
{
public:
	virtual int getFWVer(UINT* major, UINT* minor, UINT* rev, UINT* build) = 0;
};


// ---------------------------------
class BaseSoundDevice;

// 子モジュール
class BaseSoundModule : public IByteInput
{
public:
	BaseSoundModule() {}
public:
	// IByteInput
	virtual void byteOut(UINT addr, UCHAR data) {}
	virtual void directOut(UINT addr, UCHAR data) {}

public:
	virtual enum ChipType getChipType() { return CHIP_UNKNOWN; }
	virtual int isValid(void) { return C86CTL_ERR_NOT_IMPLEMENTED; }
	//virtual std::basic_string<TCHAR> getNodeId(){ return std::basic_string<TCHAR>(); }

	virtual BaseSoundDevice* getParentDevice() { return 0; }

};

// 親デバイス
class BaseSoundDevice
{
public:
	BaseSoundDevice() {}

public:
	virtual int reset() { return C86CTL_ERR_NOT_IMPLEMENTED; }
	virtual void tick(void) {}
	virtual void update(void) {}
	virtual UINT getCPS(void) { return 0; }

	virtual int isValid(void) { return C86CTL_ERR_NOT_IMPLEMENTED; }
	virtual void checkConnection(void) {}
	//virtual std::basic_string<TCHAR> getNodeId(){ return std::basic_string<TCHAR>(); }

	virtual BaseSoundModule* getModule(int id) { return NULL; }
	virtual int getNumberOfModules() { return 0; }
};

typedef std::shared_ptr<BaseSoundDevice> BaseSoundDevicePtr;

class GimicParam
{
public:
	GimicParam() : ssgVol(0), clock(0) {
	}

	UCHAR ssgVol;
	UINT clock;
};



};
