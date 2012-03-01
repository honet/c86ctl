/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <stdlib.h>
#include "opm.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


bool COPMFm::setReg( UCHAR bank, UCHAR addr, UCHAR data )
{
	// M1==S1, M2==C3, C1==S2, C2==S4
	const int slotidx[4] = { 0, 2, 1, 3 };
	bool handled = true;

	if( 0x40<=addr ){
		
		// 0=M1, 1=M2, 2=C1, 3=C2
		int sidx = slotidx[ (addr>>3)&0x03 ];
		int cidx = addr&0x07;

		COPXFmSlot *slot = ch[cidx]->slot[sidx];
		switch( addr>>5 ){
		case 2: // DT1/MUL
			slot->setDetune( (data>>4)&0x07 );
			slot->setMultiple( data&0x0f );
			break;
		case 3: // TL
			slot->setTotalLevel( data&0x7f );
			break;
		case 4: // KS/AR
			slot->setKeyscale( data>>6 );
			slot->setAttackRate( data&0x1f );
			break;
		case 5: // AMS/D1R(DR)
			if(data&0x80) slot->AMOn();
			else slot->AMOff();
			slot->setDecayRate( data&0x1f );
			break;
		case 6: // DT2/D2R(SR)
			slot->setDetune2( data>>6 );
			slot->setSustainRate( data&0x1f );
			break;
		case 7: // D1L(SL)/RR
			slot->setSustainLevel( data>>4 );
			slot->setReleaseRate( data&0x0f );
			break;
		}
		
	}else if( 0x20<=addr ){
		int cidx = addr&0x07;
		
		COPXFmCh *fmCh = ch[cidx];
		switch( addr>>3 ){
		case 4: // RL/FL/CON
			fmCh->setLR( ((data&0x40)?true:false), ((data&0x80)?true:false) );
			fmCh->setFeedback( (data>>3)&0x07 );
			fmCh->setAlgorithm( data&0x07 );
			break;
		case 5: // KC
			break;
		case 6: // KF
			break;
		case 7: // PMS/AMS
			fmCh->setAMS( data&0x03 );
			fmCh->setPMS( (data>>4)&0x07 );
			break;
		}
	}else{
		switch(addr){
		case 0x01: // TEST
			break;
		case 0x08: // KON
			{
				COPXFmCh *fmCh = ch[data&0x7];
				for( int i=0; i<4; i++ ){
					if( data&(0x80<<i) )
						fmCh->slot[i]->on();
					else
						fmCh->slot[i]->off();
				}
			}
			break;
		case 0x0f: // NFREQ
			break;
		case 0x10: // CLKA1
			break;
		case 0x11: // CLKA2
			break;
		case 0x12: // CLKB
			break;
		case 0x14: // CSM/F-RESET/IRQEN/LOAD
			break;
		case 0x18: // LFRQ
			break;
		case 0x19: // PMD/AMD
			break;
		case 0x1b: // CT/W
			break;
		default:
			handled = false;
		}
	}
	
	return handled;
}

void COPM::filter( int addr, UCHAR *data )
{
	return;
}

bool COPM::setReg( int addr, UCHAR data )
{
	return true;
}

UCHAR COPM::getReg( int addr )
{
	return 0;
}

