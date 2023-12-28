/***
	c86ctl
	論理デバイス定義
	
	Copyright (c) 2009-2013, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com

	note: honet.kk
*/

#include "stdafx.h"

#include <initguid.h>
#include <algorithm>
#include "logical_device.h"
#include "chip/chip.h"
#include "chip/opm.h"
#include "chip/opna.h"
#include "chip/opn3l.h"
#include "chip/opl3.h"
#include "chip/opll.h"

#include "if_c86usb_winusb.h"
#include "if_gimic_winusb.h"

namespace c86ctl{

LogicalDevice::LogicalDevice()
	: refcount(0)
{
	AddRef();
}

LogicalDevice::~LogicalDevice()
{
}


int LogicalDevice::reset(void)
{
	int ret = C86CTL_ERR_NODEVICE;
	for (size_t i=0; i<streams.size(); i++){
		// １デバイスに複数のモジュールが接続されている場合に
		// リセット処理が重複してしまうが、目をつぶる。
		ret = streams[i]->module->getParentDevice()->reset();
		if (ret!=C86CTL_ERR_NONE) break;
	}
	
	return ret;
}

void LogicalDevice::out( UINT addr, UCHAR data)
{
	for (size_t i=0; i<streams.size(); i++){
		streams[i]->delay->byteOut(addr, data);
	}
}

UCHAR LogicalDevice::in( UINT addr )
{
	if (streams.size()<=0)
		return 0;
	
	return streams[0]->chip->getReg(addr);
}

int LogicalDevice::getChipStatus( UINT addr, UCHAR *status )
{
	if (streams.size()<=0)
		return C86CTL_ERR_NODEVICE;

	// 複数モジュールに接続されている場合は順にスキャンして
	// サポートしていたモジュールの値を代表値とする。
	for (size_t i=0; i<streams.size(); i++){
		GimicWinUSB::GimicModuleWinUSB *gimic = dynamic_cast<GimicWinUSB::GimicModuleWinUSB*>(streams[i]->module);
		if(gimic) return gimic->getChipStatus(addr, status);
	}
	return C86CTL_ERR_UNSUPPORTED;
}

void LogicalDevice::directOut(UINT addr, UCHAR data)
{
	for (size_t i=0; i<streams.size(); i++){
		streams[i]->module->directOut(addr, data);
	}
}

int LogicalDevice::getChipType( enum ChipType *type )
{
	if (streams.size()<=0)
		return C86CTL_ERR_NODEVICE;
	if (type==NULL)
		return C86CTL_ERR_INVALID_PARAM;

	*type = streams[0]->module->getChipType();
	return C86CTL_ERR_NONE;
}

int LogicalDevice::getModuleType(enum ChipType *type)
{
	return getChipType(type);
}
	
int LogicalDevice::setSSGVolume(UCHAR vol)
{
	int ret = C86CTL_ERR_NONE;
	for (size_t i=0; i<streams.size(); i++){
		GimicWinUSB::GimicModuleWinUSB *gimic = dynamic_cast<GimicWinUSB::GimicModuleWinUSB*>(streams[i]->module);
		if(gimic){
			ret = gimic->setSSGVolume(vol);
			if (ret!=C86CTL_ERR_NONE) break;
		}
	}
	return ret;
}

int LogicalDevice::getSSGVolume(UCHAR *vol)
{
	if (streams.size()<=0)
		return C86CTL_ERR_NODEVICE;

	// 複数モジュールに接続されている場合は順にスキャンして
	// サポートしていたモジュールの値を代表値とする。
	for (size_t i=0; i<streams.size(); i++){
		GimicWinUSB::GimicModuleWinUSB *gimic = dynamic_cast<GimicWinUSB::GimicModuleWinUSB*>(streams[i]->module);
		if(gimic){
			return gimic->getSSGVolume(vol);
		}
	}

	return C86CTL_ERR_UNSUPPORTED;
}

int LogicalDevice::setPLLClock(UINT clock)
{
	int ret = C86CTL_ERR_NONE;
	for (size_t i=0; i<streams.size(); i++){
		GimicWinUSB::GimicModuleWinUSB *gimic = dynamic_cast<GimicWinUSB::GimicModuleWinUSB*>(streams[i]->module);
		Chip *chip = streams[i]->chip;
		
		if(gimic){
			ret = gimic->setPLLClock(clock);
			if (ret!=C86CTL_ERR_NONE) break;
			if (chip)
				chip->setMasterClock(clock);
		}
	}
	return ret;
}

int LogicalDevice::getPLLClock(UINT *clock)
{
	if (streams.size()<=0)
		return C86CTL_ERR_NODEVICE;
	
	// 複数モジュールに接続されている場合は順にスキャンして
	// サポートしていたモジュールの値を代表値とする。
	for (size_t i=0; i<streams.size(); i++){
		GimicWinUSB::GimicModuleWinUSB *gimic = dynamic_cast<GimicWinUSB::GimicModuleWinUSB*>(streams[i]->module);
		Chip *chip = streams[i]->chip;
		
		if(gimic){
			return gimic->getPLLClock(clock);
			
			if( chip && *clock != chip->getMasterClock() )
				chip->setMasterClock(*clock);
		}
	}
	return C86CTL_ERR_UNSUPPORTED;
}

int LogicalDevice::getFWVer( UINT *major, UINT *minor, UINT *rev, UINT *build )
{
	if (streams.size()<=0)
		return C86CTL_ERR_NODEVICE;
	
	// 複数モジュールに接続されている場合は順にスキャンして
	// サポートしていたモジュールの値を代表値とする。
	for (size_t i=0; i<streams.size(); i++){
		IFirmwareVersionInfo *verinfo = dynamic_cast<IFirmwareVersionInfo*>(streams[i]->module->getParentDevice());
		if(verinfo){
			return verinfo->getFWVer(major, minor, rev, build);
		}
	}
	return C86CTL_ERR_UNSUPPORTED;
}

int LogicalDevice::getMBInfo(struct Devinfo *info)
{
	if (streams.size()<=0)
		return C86CTL_ERR_NODEVICE;
	if (!info)
		return C86CTL_ERR_INVALID_PARAM;
	
	// 複数モジュールに接続されている場合は順にスキャンして
	// サポートしていたモジュールの値を代表値とする。
	for (size_t i=0; i<streams.size(); i++){
		GimicWinUSB *gimic = dynamic_cast<GimicWinUSB*>(streams[i]->module->getParentDevice());
		if(gimic){
			return gimic->getMBInfo(info);
		}
	}
	return C86CTL_ERR_UNSUPPORTED;
}

int LogicalDevice::getModuleInfo(struct Devinfo *info)
{
	if (streams.size()<=0)
		return C86CTL_ERR_NODEVICE;
	if (!info)
		return C86CTL_ERR_INVALID_PARAM;
	
	// 複数モジュールに接続されている場合は順にスキャンして
	// サポートしていたモジュールの値を代表値とする。
	for (size_t i=0; i<streams.size(); i++){
		GimicWinUSB::GimicModuleWinUSB *gimic = dynamic_cast<GimicWinUSB::GimicModuleWinUSB*>(streams[i]->module);
		if(gimic){
			return gimic->getModuleInfo(info);
		}
	}
	return C86CTL_ERR_UNSUPPORTED;
}


// IC86Box
int LogicalDevice::getBoardType(CBUS_BOARD_TYPE *boardType)
{
	return C86CTL_ERR_UNSUPPORTED;
}

int LogicalDevice::getSlotIndex()
{
	return C86CTL_ERR_UNSUPPORTED;
}

int LogicalDevice::writeBoardControl(UINT index, UINT val)
{
	return C86CTL_ERR_UNSUPPORTED;
}

int LogicalDevice::isValid(void)
{
	for (size_t i=0; i<streams.size(); i++){
		if (!streams[i]->module->isValid())
			return false;
	}
	return true;
}

void LogicalDevice::connect(Stream *s)
{
	streams.push_back(s);
}

void LogicalDevice::disconnect(Stream *s)
{
	auto it = std::find(streams.begin(), streams.end(), s);
	if (it != streams.end())
		streams.erase(it);
}

}
