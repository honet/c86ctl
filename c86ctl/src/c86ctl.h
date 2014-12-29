/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */


#ifndef _C86CTL_H
#define _C86CTL_H

#include <ObjBase.h>

#ifdef __cplusplus
namespace c86ctl{
#endif

/*----------------------------------------------------------------------------*/
/*  定数定義                                                                  */
/*----------------------------------------------------------------------------*/
#define C86CTL_ERR_NONE						0
#define C86CTL_ERR_UNKNOWN					-1
#define C86CTL_ERR_INVALID_PARAM			-2
#define C86CTL_ERR_UNSUPPORTED				-3
#define C86CTL_ERR_NODEVICE					-1000
#define C86CTL_ERR_NOT_IMPLEMENTED			-9999

enum ChipType {
	CHIP_UNKNOWN		= 0x0000,
	
	CHIP_OPNA			= 0x00001,
	CHIP_YM2608			= 0x00001,
	CHIP_YM2608NOADPCM	= 0x10001,
	CHIP_OPM			= 0x00002,
	CHIP_YM2151			= 0x00002,
	CHIP_OPN3L			= 0x00003,
	CHIP_YMF288			= 0x00003,
	CHIP_OPL3			= 0x00004,
	CHIP_YMF262			= 0x00004,
	CHIP_OPLL			= 0x00005,
	CHIP_YM2413			= 0x00005,
	
	CHIP_SN76489		= 0x00006,
	CHIP_SN76496		= 0x10006,
	CHIP_AY38910		= 0x00007,
	CHIP_YM2149			= 0x10007,
	CHIP_YM2203			= 0x00008,
	CHIP_YM2612			= 0x00009,
	CHIP_YM3526			= 0x0000a,
	CHIP_YM3812			= 0x0000b,
	CHIP_YMF271			= 0x0000c,
	CHIP_YMF278B		= 0x0000d,
	CHIP_YMZ280B		= 0x0000e,
	CHIP_YMF297			= 0x0000f,
	CHIP_YM2610B		= 0x00010,
	CHIP_Y8950			= 0x00020,
	CHIP_Y8950ADPCM 	= 0x10020,
    CHIP_YM3438         = 0x00021
};

typedef enum {
	CBUS_BOARD_UNKNOWN					= 0x0,
	CBUS_BOARD_14						= 0x00001,	// (未対応) NEC PC-9801-14
	CBUS_BOARD_26						= 0x00002,	// NEC PC-9801-26, 26K
	CBUS_BOARD_SOUND_ORCHESTRA			= 0x00012,	// (未テスト) SNE Sound Orchestra : YM2203, YM3812
	CBUS_BOARD_SOUND_ORCHESTRA_L		= 0x00022,	// (未テスト) SNE Sound Orchestra L : YM2203, YM3812
	CBUS_BOARD_SOUND_ORCHESTRA_V		= 0x00032,	// (未テスト) SNE Sound Orchestra V : YM2203, Y8950(w/ADPCM-RAM)
	CBUS_BOARD_SOUND_ORCHESTRA_VS		= 0x00042,	// (未テスト) SNE Sound Orchestra VS : YM2203, Y8950(w/ADPCM-RAM)
	CBUS_BOARD_SOUND_ORCHESTRA_LS		= 0x00052,	// (未テスト) SNE Sound Orchestra LS : YM2203, Y8950
	CBUS_BOARD_SOUND_ORCHESTRA_MATE		= 0x00062,	// (未テスト) SNE Sound Orchestra MATE : YM2203, Y8950
	CBUS_BOARD_MULTIMEDIA_ORCHESTRA		= 0x00072,	// (未テスト) SNE Multimedia Orchestra : YM2203, YM262M
	CBUS_BOARD_LITTLE_ORCHESTRA			= 0x00082,	// (未テスト) SNE Littele Orchestra : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_L		= 0x00092,	// (未テスト) SNE Littele Orchestra L : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_RS		= 0x000a2,	// (未テスト) SNE Littele Orchestra RS : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_LS		= 0x000b2,	// (未テスト) SNE Littele Orchestra LS : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_SS		= 0x000c2,	// (未テスト) SNE Littele Orchestra SS : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_MATE	= 0x000d2,	// (未テスト) SNE Littele Orchestra MATE : YM2203
	CBUS_BOARD_LITTLE_ORCHESTRA_FELLOW	= 0x000e2,	// (未テスト) SNE Littele Orchestra FELLOW : YM2203
	CBUS_BOARD_JOY2						= 0x000f2,	// (未テスト) SNE JOY-2 : YM2203
	CBUS_BOARD_SOUND_GRANPRI			= 0x00102,	// (未テスト) SNE SOUND GRANPRI : YM2203
	CBUS_BOARD_TN_F3FM					= 0x00112,	// (未テスト) 東京ニーズ TN-F3FM : YM2203C
	
	CBUS_BOARD_73						= 0x00003,	// NEC PC-9801-73
	CBUS_BOARD_86						= 0x00023,	// NEC PC-9801-86
	
	CBUS_BOARD_ASB01					= 0x00043,	// (未テスト) SIS アミューズメントサウンドボードASB-01 : YM2608
	CBUS_BOARD_SPEAKBOARD				= 0x00053,	// (未テスト) アイドルジャパン SpeakBoard : YM2608(w/ADPCM-RAM)
	CBUS_BOARD_SOUNDPLAYER98			= 0x00063,	// (未テスト) コンピュータテクニカ SPB-98 : YM2608, YMF278
	
	CBUS_BOARD_SECONDBUS86				= 0x00073,	// (未対応) second-bus86 : YM2608, YMF278B-S
	CBUS_BOARD_SOUNDEDGE				= 0x00083,	// (未対応) sound-edge : YAMAHA Sound Edge SW20-98 : YM2608B, YMF278B
	CBUS_BOARD_WINDUO					= 0x00093,	// (未対応) win-duo : YM2608
	CBUS_BOARD_OTOMI					= 0x000a3,	// (未テスト) MAD FACTORY 音美 : YM2608(w/ADPCM-RAM), YM3438

	CBUS_BOARD_WAVEMASTER				= 0x000b3,	// Q-Vision WaveMaster(86互換) : YM2608
	CBUS_BOARD_WAVESMIT					= 0x000c3,	// Q-Vision WaveSMIT(86互換) : YMF288-S
	CBUS_BOARD_WAVESTAR					= 0x000d3,	// Q-Vision WaveStar(86互換) : YMF288-S
	CBUS_BOARD_WSN_A4F					= 0x000e3,	// (未対応) Buffalo WSN-A4F : YMF288-S

	CBUS_BOARD_SB16						= 0x00004,	// (未対応) sound-blaster 16 (CT2720) : YMF262-F
	CBUS_BOARD_SB16_2203				= 0x00014,	// (未対応) sound-blaster 16 with YM2203 (CT2720) : YMF262-F, YM2203
	CBUS_BOARD_SB16VALUE				= 0x00024,	// (未対応) sound-blaster 16Value (CT3720) : YMF262-F
	CBUS_BOARD_POWERWINDOW_T64S			= 0x00034,	// (未対応) canopus PowerWindow T64S : YMF262-M
	CBUS_BOARD_PCSB2					= 0x00044,	// (未対応) EPSON PCSB2 : YMF262-M
	CBUS_BOARD_WGS98S					= 0x00054,	// (未対応) コンピュータテクニカ : YMF262-M
	CBUS_BOARD_SRB_G					= 0x00064,	// (未対応) buffalo SRB-G : YMF264-F, YMZ263B-F
	CBUS_BOARD_MIDI_ORCHESTRA_MIDI3		= 0x00074,	// (未対応) SNE MIDI ORCHESTRA MIDI-3 : YM264F
		
	CBUS_BOARD_SB_AWE32					= 0x00005,	// (未対応) SoundBlaster AWE32 (CT3610) : YMF289B-S
	CBUS_BOARD_118						= 0x00006	// NEC PC-9801-118 : YMF297-F
} CBUS_BOARD_TYPE;

/*----------------------------------------------------------------------------*/
/*  構造体定義                                                                */
/*----------------------------------------------------------------------------*/
struct Devinfo{
	char Devname[16];
	char Rev;
	char Serial[15];
};

/*----------------------------------------------------------------------------*/
/*  Interface定義                                                             */
/*----------------------------------------------------------------------------*/
// IRealChipBase {5C457918-E66D-4AC1-8CB5-B91C4704DF79}
static const GUID IID_IRealChipBase = 
{ 0x5c457918, 0xe66d, 0x4ac1, { 0x8c, 0xb5, 0xb9, 0x1c, 0x47, 0x4, 0xdf, 0x79 } };

interface IRealChipBase : public IUnknown
{
	virtual int __stdcall initialize(void) = 0;
	virtual int __stdcall deinitialize(void) = 0;
	virtual int __stdcall getNumberOfChip(void) = 0;
	virtual HRESULT __stdcall getChipInterface( int id, REFIID riid, void** ppi ) = 0;
};

// ---------------------------------------
// deprecated. use IRealChip2 instead.
// IRealChip {F959C007-6B4D-46F3-BB60-9B0897C7E642}
static const GUID IID_IRealChip = 
{ 0xf959c007, 0x6b4d, 0x46f3, { 0xbb, 0x60, 0x9b, 0x8, 0x97, 0xc7, 0xe6, 0x42 } };
interface IRealChip : public IUnknown
{
public:
	virtual int __stdcall reset(void) = 0;
	virtual void __stdcall out( UINT addr, UCHAR data ) = 0;
	virtual UCHAR __stdcall in( UINT addr ) = 0;
};


// ---------------------------------------
// IRealChip2 {BEFA830A-0DF3-46E4-A79E-FABB78E80357}
static const GUID IID_IRealChip2 = 
{ 0xbefa830a, 0xdf3, 0x46e4, { 0xa7, 0x9e, 0xfa, 0xbb, 0x78, 0xe8, 0x3, 0x57 } };

interface IRealChip2 : public IRealChip
{
	virtual int __stdcall getChipStatus( UINT addr, UCHAR *status ) = 0;
	virtual void __stdcall directOut(UINT addr, UCHAR data) = 0;
};


// ---------------------------------------
// deprecated. use IGimic2 instead.
// IGimic {175C7DA0-8AA5-4173-96DA-BB43B8EB8F17}
static const GUID IID_IGimic = 
{ 0x175c7da0, 0x8aa5, 0x4173, { 0x96, 0xda, 0xbb, 0x43, 0xb8, 0xeb, 0x8f, 0x17 } };
interface IGimic : public IUnknown
{
	virtual int __stdcall getFWVer( UINT *major, UINT *minor, UINT *revision, UINT *build ) = 0;
	virtual int __stdcall getMBInfo( struct Devinfo *info ) = 0;
	virtual int __stdcall getModuleInfo( struct Devinfo *info ) = 0;
	virtual int __stdcall setSSGVolume(UCHAR vol) = 0;
	virtual int __stdcall getSSGVolume(UCHAR *vol) = 0;
	virtual int __stdcall setPLLClock(UINT clock) = 0;
	virtual int __stdcall getPLLClock(UINT *clock) = 0;
};

// ---------------------------------------
// IGimic2 {47141A01-15F5-4BF5-9554-CA7AACD54BB8}
static const GUID IID_IGimic2 = 
{ 0x47141a01, 0x15f5, 0x4bf5, { 0x95, 0x54, 0xca, 0x7a, 0xac, 0xd5, 0x4b, 0xb8 } };
interface IGimic2 : public IGimic
{
	virtual int __stdcall getModuleType( enum ChipType *type ) = 0;
};


// ---------------------------------------
// IC86Usb {312481E2-A93C-4A2F-87CA-CE3AC1096ED5}
static const GUID IID_IC86BOX = 
{ 0x312481e2, 0xa93c, 0x4a2f, { 0x87, 0xca, 0xce, 0x3a, 0xc1, 0x9, 0x6e, 0xd5 } };
interface IC86Box : public IUnknown
{
	virtual int __stdcall getFWVer(UINT *major, UINT *minor, UINT *revision, UINT *build) = 0;
	virtual int __stdcall getBoardType(CBUS_BOARD_TYPE *type) = 0;
	virtual int __stdcall getSlotIndex() = 0;
	virtual int __stdcall writeBoardControl(UINT index, UINT val) = 0;
};

/*----------------------------------------------------------------------------*/
/*  公開関数定義                                                              */
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

HRESULT WINAPI CreateInstance( REFIID riid, void** ppi );


int WINAPI c86ctl_initialize(void);					// DEPRECATED
int WINAPI c86ctl_deinitialize(void);				// DEPRECATED
int WINAPI c86ctl_reset(void);						// DEPRECATED
void WINAPI c86ctl_out( UINT addr, UCHAR data );	// DEPRECATED
UCHAR WINAPI c86ctl_in( UINT addr );				// DEPRECATED



#ifdef __cplusplus
};
};
#endif

#endif
