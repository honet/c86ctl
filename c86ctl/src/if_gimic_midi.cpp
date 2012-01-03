/***
	c86ctl
	gimic コントロール MIDI版(実験コード)
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
	Thanks to Nagai "Guu" Osamu 2011/12/08 for his advice.

	note: honet/k.kotajima
	MIDIでのレジスタダンプ転送はいずれ廃止したいと思っているが
	今のところHID版がまだ安定していないので残している。

	転送仕様： sysexに3byteとごにパックして送る。
	 idx  data   mean
	   0  0xF0: Start of SysEx
	   1  0x7D:   device id
	3n+2  (d0):   {000, exaddr[1:0], addr[7:6]}
	3n+3  (d1):   {addr[5:0], data[7:7]}
	3n+4  (d2):   data[6:0]
	...
	3N+5  0xF7: End of SysEx

	システムメッセージ：
	SW Reset     : f0, 7d, 40, 01, 00, 7e, f7
	HW Reset     : f0, 7d, 40, 01, 00, 7f, f7
	PLL Set      : f0, 7d, 40, 01, 00, 70, v0, v1, v2, v3, f7
	GM System ON : f0, 7e, 7f, 09, 01, f7
	SSG Vol      : f0, 7f, 04, vol[6:0], vol[13:7], f7
 */

#include	<stdafx.h>
#include	"if_gimic_midi.h"
#include	"config.h"

/*----------------------------------------------------------------------------
	コンストラクタ
----------------------------------------------------------------------------*/
GimicMIDI::GimicMIDI( HMIDIOUT h ) : hHandle(h)
{
	rbuff.alloc(128);
}

/*----------------------------------------------------------------------------
	デストラクタ
----------------------------------------------------------------------------*/
GimicMIDI::~GimicMIDI( void)
{
	if(hHandle) {
		midiOutClose(hHandle);
		hHandle = NULL;
	}
}


/*----------------------------------------------------------------------------
	MIDI-IF factory
----------------------------------------------------------------------------*/
std::vector< std::shared_ptr<GimicIF> > GimicMIDI::CreateInstances(void)
{
	std::vector< std::shared_ptr<GimicIF> > instances;

	// MIDIOUTCAPSでgimicかどうか選別できないかと思ったけどダメだった。
//	UINT n = midiOutGetNumDevs();
//	for( UINT i=0; i<n; i++ ){
//		MIDIOUTCAPS midiOutCaps;
//		midiOutGetDevCaps( i, &midiOutCaps, sizeof(midiOutCaps) );
//	}

	int DeviceID = gConfig.getInt(INISC_MAIN, INIKEY_MIDIDEVICE, -1);
	HMIDIOUT hmidi = NULL;
	UINT res = midiOutOpen(&hmidi, DeviceID, 0, 0, CALLBACK_NULL);
	if(res == MMSYSERR_NOERROR){
		instances.push_back( GimicMIDIPtr(new GimicMIDI(hmidi)) );
	}

	return instances;
}

void GimicMIDI::sendSysEx( uint8_t *data, uint32_t sz )
{
	MIDIHDR head;
	ZeroMemory(&head, sizeof(MIDIHDR));
	head.lpData = reinterpret_cast<LPSTR>(data);
	head.dwBufferLength = sz;

	midiOutPrepareHeader(hHandle, &head, sizeof(MIDIHDR));
	midiOutLongMsg(hHandle, &head, sizeof(MIDIHDR));
	// ↓これ無くてもいいですよね？(by Guu)
	//	while((head.dwFlags & MHDR_DONE) == 0) ;
	midiOutUnprepareHeader(hHandle, &head, sizeof(MIDIHDR));
}

/*----------------------------------------------------------------------------
	実装
----------------------------------------------------------------------------*/

int GimicMIDI::reset( void )
{
	// 転送完了待ち
	while(rbuff.get_length()){
		Sleep(10);
	}

	// GM System ON
	UCHAR d[] = { 0xf0, 0x7e, 0x7f, 0x9, 0x1, 0xf7 };
	sendSysEx( &d[0], 6 );
	return C86CTL_ERR_NONE;
}

void GimicMIDI::out(UINT addr, UCHAR data)
{
	// data packing.
	UCHAR d[3] = { (addr>>6)&0x0f, (addr&0x3f)<<1 | (data>>7), (data&0x7f) };
	rbuff.write(d,3);
}

int GimicMIDI::setSSGVolume(UCHAR vol)
{
	// master volume set.
	UCHAR d[] = { 0xf0, 0x7f, 0x04, (vol<<6)&0x7f, (vol>>1)&0x7f, 0xf7 };
	sendSysEx( &d[0], 6 );
	return C86CTL_ERR_NONE;
}

int GimicMIDI::setPLLClock(UINT clock)
{
	UCHAR d[] = { 0xf0, 0x7d, 0x40, 0x01, 0x00, 0x70,
		clock&0x7f, (clock>>7)&0x7f, (clock>>14)&0x7f, 0xf7 };
	sendSysEx( &d[0], 10 );
	return C86CTL_ERR_NONE;
}

void GimicMIDI::tick(void)
{
	UCHAR buff[150];
	
	if( !hHandle )
		return;

	// 転送配列準備
	buff[0] = 0xf0;		// start of sysex
	buff[1] = 0x7d;		// device id
	UINT msz = rbuff.get_length();
	UINT sz = msz / 3;
	if( 0 < sz ){
		rbuff.read( &buff[2], MIN(sz*3,144) );
		buff[sz*3+2] = 0xf7;		// end of sysex
		sendSysEx( &buff[0], sz*3+3 );
	}
	
	return;
}


