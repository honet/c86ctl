/*=============================================================================
	I/Fアクセスインタフェース定義 for GIMIC
=============================================================================*/
#pragma once
#include <memory>

//#define SUPPORT_WINUSB
#define SUPPORT_MIDI
#define SUPPORT_HID

class GimicIF {
public:
	virtual void Init(void){};
	virtual void Reset(void){};
	virtual void Out(uint16_t addr, uint8_t data){};
	virtual void Tick(void){};

	//void SetVolume(void);
	//void SetPLLClock(void);
};

typedef std::shared_ptr<GimicIF> GimicIFPtr;
