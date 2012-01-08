/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <stdlib.h>
#include "opna.h"


bool COPNA::setReg( int addr, UCHAR data ){
	if( 0x200 < addr ) return false;
	int idx = 0;
	if( 0x100 < addr ){
		addr -= 0x100;
		idx = 1;
	}
	reg[idx][addr] = data;
	regATime[idx][addr] = 255;

	if( idx == 0 ){
		if( ssgRegHandling( addr, data ) )
			return true;
		if( rhythmRegHandling( addr, data ) )
			return true;
		if( fmCommonRegHandling( addr, data ) )
			return true;
	}else{
		if( adpcmRegHandling( addr, data ) )
			return true;
	}
	if( fmRegHandling( idx, addr, data ) )
		return true;

	return false;
};

UCHAR COPNA::getReg( int addr ){
	if( 0x200 < addr ) return 0;
	int idx = 0;
	if( 0x100 < addr ){
		addr -= 0x100;
		idx = 1;
	}
	return reg[idx][addr];
};





bool COPNA::fmCommonRegHandling( UCHAR adrs, UCHAR data )
{
	bool handled = true;
	INT ch;
	
	switch(adrs){
	// prescaler
	case 0x2d:
		prescale_fm = 6;
		prescale_ssg = 4;
		break;
	case 0x2e:
		prescale_fm = 3;
		prescale_ssg = 2;
		break;
	case 0x2f:
		prescale_fm = 2;
		prescale_ssg = 1;
		break;

	case 0x27:// Timer Control / Mode
		fm[2].setExMode( (data>>6)&0x3 );
		break;
		
	case 0x28:
		switch(data&0x7){
		case 0x0: ch=0; break;
		case 0x1: ch=1; break;
		case 0x2: ch=2; break;
		case 0x4: ch=3; break;
		case 0x5: ch=4; break;
		case 0x6: ch=5; break;
		default: ch=-1; handled=false; break;
		}
		if(0<=ch){
			fm[ch].keyOn( (data>>4) & 0xf );
		}
		break;
		
	case 0x22:	// LFO
	case 0x24:	// Timer-A Corse
	case 0x25:	// Timer-A Fine
	//case 0x26:	// Timer-B
	case 0x29:	// IRQ/SCH
		break;
		
	default:
		handled = false;
		break;
	}

	return handled;
}

bool COPNA::fmRegHandling( UCHAR bank, UCHAR adrs, UCHAR data )
{
	UINT hi, lo;
	INT ch, slot, tmp, choffset = bank*3;
	bool handled = true;
	static INT stbl[4] = {0,2,1,3};

	switch(adrs){
	// fno, block
	case 0xa0: // ch1
	case 0xa4:
	case 0xa1: // ch2
	case 0xa5:
	case 0xa2: // ch3
	case 0xa6:
		reg[bank][adrs] = data;
		ch = (adrs - 0xa0) & 0x03;
		hi = reg[bank][0xa4+ch];
		lo = reg[bank][0xa0+ch];
		fm[choffset+ch].setFByReg( lo, hi );
		break;
	case 0xa8: // ch3 ex: slot3
	case 0xac:
		reg[bank][adrs] = data;
		hi = reg[bank][0xac];
		lo = reg[bank][0xa8];
		fm[choffset+2].setFByRegEx( 2, lo, hi );
		break;
	case 0xa9: // ch3 ex: slot1
	case 0xad:
		hi = reg[bank][0xad];
		lo = reg[bank][0xa9];
		fm[choffset+2].setFByRegEx( 0, lo, hi );
		break;
	case 0xaa: // ch3 ex: slot2
	case 0xae:
		hi = reg[bank][0xae];
		lo = reg[bank][0xaa];
		fm[choffset+2].setFByRegEx( 1, lo, hi );
		break;

	// algorithm, feedback
	case 0xb0:
	case 0xb1:
	case 0xb2:
		ch = adrs - 0xb0;
		fm[choffset+ch].setAlgorithm( data & 0x07 );
		fm[choffset+ch].setFeedback( (data >> 3) & 0x07 );
		break;

	// LR, AMS, PMS
	case 0xb4:
	case 0xb5:
	case 0xb6:
		ch = adrs - 0xb4;
		fm[choffset+ch].setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		fm[choffset+ch].setAMS( (data>>4)&0x03 );
		fm[choffset+ch].setPMS( data&0x07 );
		break;

	// slot - detune/multi
	case 0x30:	case 0x31:	case 0x32:
	case 0x34:	case 0x35:	case 0x36:
	case 0x38:	case 0x39:	case 0x3a:
	case 0x3c:	case 0x3d:	case 0x3e:
		tmp = adrs - 0x30;
		ch = tmp&0x3;
		slot = stbl[tmp>>2];
		fm[choffset+ch].slot[slot].setMultiple( data&0xf );
		fm[choffset+ch].slot[slot].setDetune( (data>>4)&0x7 );
		break;

	// slot - total level
	case 0x40:	case 0x41:	case 0x42:
	case 0x44:	case 0x45:	case 0x46:
	case 0x48:	case 0x49:	case 0x4a:
	case 0x4c:	case 0x4d:	case 0x4e:
		tmp = adrs - 0x40;
		ch = tmp&0x3;
		slot = stbl[tmp>>2];
		fm[choffset+ch].slot[slot].setTotalLevel( data&0x7f );
		break;
		
	// slot - keyscale / attack rate
	case 0x50:	case 0x51:	case 0x52:
	case 0x54:	case 0x55:	case 0x56:
	case 0x58:	case 0x59:	case 0x5a:
	case 0x5c:	case 0x5d:	case 0x5e:
		tmp = adrs - 0x50;
		ch = tmp&0x3;
		slot = stbl[tmp>>2];
		fm[choffset+ch].slot[slot].setKeyscale( (data>>6)&0x3 );
		fm[choffset+ch].slot[slot].setAttackRate( data&0x1f );
		break;
		
	// slot - decay rate / amon
	case 0x60:	case 0x61:	case 0x62:
	case 0x64:	case 0x65:	case 0x66:
	case 0x68:	case 0x69:	case 0x6a:
	case 0x6c:	case 0x6d:	case 0x6e:
		tmp = adrs - 0x60;
		ch = tmp&0x3;
		slot = stbl[tmp>>2];
		fm[choffset+ch].slot[slot].setDecayRate( data&0x1f );
		if( data&0x80 )	fm[ch].slot[slot].AMOn();
		else			fm[ch].slot[slot].AMOff();
		break;
		
	// slot - sustain rate
	case 0x70:	case 0x71:	case 0x72:
	case 0x74:	case 0x75:	case 0x76:
	case 0x78:	case 0x79:	case 0x7a:
	case 0x7c:	case 0x7d:	case 0x7e:
		tmp = adrs - 0x70;
		ch = tmp&0x3;
		slot = stbl[tmp>>2];
		fm[choffset+ch].slot[slot].setSustainRate( data&0x1f );
		break;
		
	// slot - sustain level / rerease rate
	case 0x80:	case 0x81:	case 0x82:
	case 0x84:	case 0x85:	case 0x86:
	case 0x88:	case 0x89:	case 0x8a:
	case 0x8c:	case 0x8d:	case 0x8e:
		tmp = adrs - 0x80;
		ch = tmp&0x3;
		slot = stbl[tmp>>2];
		fm[choffset+ch].slot[slot].setSustainLevel( (data>>4)&0xf );
		fm[choffset+ch].slot[slot].setReleaseRate( data&0xf );
		break;
		
	// slot - ssgeg-type
	case 0x90:	case 0x91:	case 0x92:
	case 0x94:	case 0x95:	case 0x96:
	case 0x98:	case 0x99:	case 0x9a:
	case 0x9c:	case 0x9d:	case 0x9e:
		tmp = adrs - 0x90;
		ch = tmp&0x3;
		slot = stbl[tmp>>2];
		fm[choffset+ch].slot[slot].setSSGEGType( data&0xf );
		break;
		
	default:
		handled = false;
		break;
	}

	return handled;
};

bool COPNA::ssgRegHandling( UCHAR adrs, UCHAR data )
{
	INT ch;
	bool handled = true;
	
	switch(adrs){
	case 0x00: // fine tune
	case 0x02:
	case 0x04:
		ch = adrs>>1;
		ssg.ch[ch].setFineTune(data);
		break;
	case 0x01: // coarse tune
	case 0x03:
	case 0x05:
		ch = (adrs-1)>>1;
		ssg.ch[ch].setCoarseTune(data&0x0f);
		break;
		
	case 0x06: // noise period
		ssg.setNoisePeriod( data&0x1f );
		break;
	case 0x07: // keyon
		if( data&0x01 ){ ssg.ch[0].toneOn(); }else{ ssg.ch[0].toneOff(); };
		if( data&0x02 ){ ssg.ch[1].toneOn(); }else{ ssg.ch[1].toneOff(); };
		if( data&0x04 ){ ssg.ch[2].toneOn(); }else{ ssg.ch[2].toneOff(); };
		if( data&0x08 ){ ssg.ch[0].noiseOn(); }else{ ssg.ch[0].noiseOff(); };
		if( data&0x10 ){ ssg.ch[1].noiseOn(); }else{ ssg.ch[1].noiseOff(); };
		if( data&0x20 ){ ssg.ch[2].noiseOn(); }else{ ssg.ch[2].noiseOff(); };
		break;
		
	case 0x08: // amp level
	case 0x09:
	case 0x0a:
		ch = adrs-0x08;
		ssg.ch[ch].setLevel(data&0x0f);
		ssg.ch[ch].setUseEnv((data&0x10) ? true: false);
		break;

	case 0x0b: // env fine tune
		ssg.setEnvFineTune(data);
		break;
	case 0x0c: // env coarse tune
		ssg.setEnvCoarseTune(data);
		break;
	case 0x0d: // env type
		ssg.setEnvType(data);
		break;
	case 0x0e: // I/O portA
	case 0x0f: // I/O portB
		break;
	default:
		handled = false;
	}
	return handled;
}

bool COPNA::rhythmRegHandling( UCHAR adrs, UCHAR data )
{
	bool handled = true;
	
	switch(adrs){
	case 0x10: // keyon
		if( data&0x80 ){
			// dump
			if( data&0x1 ) rhythm.bd.off(); // bd
			if( data&0x2 ) rhythm.sd.off(); // sd
			if( data&0x4 ) rhythm.top.off(); // top
			if( data&0x8 ) rhythm.hh.off(); // hh
			if( data&0x10 ) rhythm.tom.off(); // tom
			if( data&0x20 ) rhythm.rim.off(); // rim
		}else{
			// keyon
			if( data&0x1 ) rhythm.bd.on(); // bd
			if( data&0x2 ) rhythm.sd.on(); // sd
			if( data&0x4 ) rhythm.top.on(); // top
			if( data&0x8 ) rhythm.hh.on(); // hh
			if( data&0x10 ) rhythm.tom.on(); // tom
			if( data&0x20 ) rhythm.rim.on(); // rim
		}
		break;
		
	case 0x11: // total level
		rhythm.setTotalLevel(data&0x3f);
		break;

	case 0x18: // bd
		rhythm.bd.setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		rhythm.bd.setLevel( data&0x1f );
		break;
	case 0x19: // sd
		rhythm.sd.setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		rhythm.sd.setLevel( data&0x1f );
		break;
	case 0x1a: // top
		rhythm.top.setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		rhythm.top.setLevel( data&0x1f );
		break;
	case 0x1b: // hh
		rhythm.hh.setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		rhythm.hh.setLevel( data&0x1f );
		break;
	case 0x1c: // tom
		rhythm.top.setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		rhythm.top.setLevel( data&0x1f );
		break;
	case 0x1d: // rim
		rhythm.rim.setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		rhythm.rim.setLevel( data&0x1f );
		break;
		
	default:
		handled = false;
	}
	return handled;
}

bool COPNA::adpcmRegHandling( UCHAR adrs, UCHAR data )
{
	if(adrs<=0x10)
		return true;

	return false;
	/*
	UINT h, l;
	bool handled = true;

	switch(adrs){
	case 0x02: /// start addr (L)
	case 0x03: /// start addr (H)
		h = reg[1][3];
		l = reg[1][2];
		adpcmStartAddr = (h<<13) | (l<<5);
		adpcmCurrentAddr = adpcmStartAddr;
		
		break;
	case 0x04: /// stop addr (L)
	case 0x05: /// stop addr (H)
		h = reg[1][5];
		l = reg[1][4];
		adpcmStopAddr = (h<<13) | (l<<5);
		break;
		
	case 0x0c: /// limit addr (L)
	case 0x0d: /// limit addr (H)
		h = reg[1][5];
		l = reg[1][4];
		adpcmLimitAddr = (h<<13) | (l<<5);
		break;

	case 0x08:
		if( reg[1][0] & 0x40 ){
			adpcm_data[adpcmCurrentAddr] = data;
			if( adpcmStopAddr > adpcmCurrentAddr ){
				if( adpcmLimitAddr > adpcmCurrentAddr ){
					adpcmCurrentAddr++;
				}else{
					adpcmCurrentAddr=0;
				}
			}
		}
		break;
		
	default:
		handled = false;
	}
	
	return handled;
		*/
}
