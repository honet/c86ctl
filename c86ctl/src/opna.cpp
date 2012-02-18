/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <stdlib.h>
#include "opna.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

void COPNA::filter( int addr, UCHAR *data )
{
	int ch;
	if( 0x200 <= addr ) return;

	switch( addr ){
	case 0x29: // SCH/IRQ ENABLE
		*data |= 0x80;
	case 0xb4: // FM -- LR, AMS, PMS
	case 0xb5:
	case 0xb6:
		ch = addr - 0xb4;
		if( getMixedMask( ch ) )
			*data &= 0x3f;
		break;
			
	case 0x1b4: // FM -- LR, AMS, PMS
	case 0x1b5:
	case 0x1b6:
		ch = addr - 0x1b4;
		if( getMixedMask( 6+ch ) )
			*data &= 0x3f;
		break;

	case 0x08: // SSG amp level
	case 0x09:
	case 0x0a:
		ch = addr-0x08;
		if( getMixedMask( 9+ch ) )
			*data &= 0xf0;
		break;
		
	case 0x11: // Rhythm total level
		if( getMixedMask( 13 ) )
			*data = 0;
		break;

	case 0x10b: // ADPCM level control
		if( getMixedMask( 12 ) )
			*data = 0;
		break;
	}
}

bool COPNA::setReg( int addr, UCHAR data ){
	if( 0x200 <= addr ) return false;
	int idx = 0;
	if( 0x100 <= addr ){
		addr -= 0x100;
		idx = 1;
	}
	reg[idx][addr] = data;
	INT c = regATime[idx][addr];
	c+=64;
	regATime[idx][addr] = 255<c ? 255 : c;

	if( idx == 0 ){
		if( ssg->setReg( addr, data ) )
			return true;
		if( rhythm->setReg( addr, data ) )
			return true;
		if( fmCommonRegHandling( addr, data ) )
			return true;
	}else{
		if( adpcm->setReg( addr, data ) )
			return true;
	}
	if( fm->setReg( idx, addr, data ) )
		return true;

	return false;
};

UCHAR COPNA::getReg( int addr ){
	if( 0x200 <= addr ) return 0;
	int idx = 0;
	if( 0x100 <= addr ){
		addr -= 0x100;
		idx = 1;
	}
	return reg[idx][addr];
};


bool COPNA::fmCommonRegHandling( UCHAR adrs, UCHAR data )
{
	bool handled = true;
	
	switch(adrs){
	case 0x10:	// STATUS MASK
		//data&0x80; // IRQ RESET
		//data&0x10; // MASK ZERO (ADPCM)
		//data&0x08; // MASK BRDY (ADPCM)
		//data&0x04; // MASK EOS (ADPCM)
		//data&0x02; // MASK TIMER-B
		//data&0x01; // MASK TIMER-A
		break;
		
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

bool COPNAFm::setReg( UCHAR bank, UCHAR adrs, UCHAR data )
{
	INT idx, slot, tmp, idxoffset = bank*3;
	bool handled = true;
	const INT stbl[4] = {0,2,1,3};

	if( bank == 0 ){
		switch(adrs){
		case 0x27:// Timer Control / Mode
			ch[2]->setExMode( (data>>6)&0x3 );
			break;

		case 0x28:
			switch(data&0x7){
			case 0x0: idx=0; break;
			case 0x1: idx=1; break;
			case 0x2: idx=2; break;
			case 0x4: idx=3; break;
			case 0x5: idx=4; break;
			case 0x6: idx=5; break;
			default: idx=-1; handled=false; break;
			}
			if(0<=idx){
				ch[idx]->keyOn( (data>>4) & 0xf );
			}
			break;

		case 0x22:	// LFO
			break;
		default:
			handled = false;
			break;
		}
		if( handled )
			return true;
	}

	handled = true;

	switch(adrs){
	// fno, block
	case 0xa0: // ch1
	case 0xa1: // ch2
	case 0xa2: // ch3
		idx = (adrs - 0xa0);
		ch[idxoffset+idx]->setFLo(data);
		break;
	case 0xa4:
	case 0xa5:
	case 0xa6:
		idx = (adrs - 0xa4);
		ch[idxoffset+idx]->setFHi(data);
		break;

	case 0xa8: // ch3 ex: slot3
		ch[idxoffset+2]->setFExLo(2, data);
		break;
	case 0xa9: // ch3 ex: slot1
		ch[idxoffset+2]->setFExLo(0, data);
		break;
	case 0xaa: // ch3 ex: slot2
		ch[idxoffset+2]->setFExLo(1, data);
		break;

	case 0xac:
		ch[idxoffset+2]->setFExHi(2, data);
		break;
	case 0xad:
		ch[idxoffset+2]->setFExHi(0, data);
		break;
	case 0xae:
		ch[idxoffset+2]->setFExHi(1, data);
		break;

	// algorithm, feedback
	case 0xb0:
	case 0xb1:
	case 0xb2:
		idx = adrs - 0xb0;
		ch[idxoffset+idx]->setAlgorithm( data & 0x07 );
		ch[idxoffset+idx]->setFeedback( (data >> 3) & 0x07 );
		break;

	// LR, AMS, PMS
	case 0xb4:
	case 0xb5:
	case 0xb6:
		idx = adrs - 0xb4;
		ch[idxoffset+idx]->setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		ch[idxoffset+idx]->setAMS( (data>>4)&0x03 );
		ch[idxoffset+idx]->setPMS( data&0x07 );
		break;

	// slot - detune/multi
	case 0x30:	case 0x31:	case 0x32:
	case 0x34:	case 0x35:	case 0x36:
	case 0x38:	case 0x39:	case 0x3a:
	case 0x3c:	case 0x3d:	case 0x3e:
		tmp = adrs - 0x30;
		idx = tmp&0x3;
		slot = stbl[tmp>>2];
		ch[idxoffset+idx]->slot[slot]->setMultiple( data&0xf );
		ch[idxoffset+idx]->slot[slot]->setDetune( (data>>4)&0x7 );
		break;

	// slot - total level
	case 0x40:	case 0x41:	case 0x42:
	case 0x44:	case 0x45:	case 0x46:
	case 0x48:	case 0x49:	case 0x4a:
	case 0x4c:	case 0x4d:	case 0x4e:
		tmp = adrs - 0x40;
		idx = tmp&0x3;
		slot = stbl[tmp>>2];
		ch[idxoffset+idx]->slot[slot]->setTotalLevel( data&0x7f );
		break;
		
	// slot - keyscale / attack rate
	case 0x50:	case 0x51:	case 0x52:
	case 0x54:	case 0x55:	case 0x56:
	case 0x58:	case 0x59:	case 0x5a:
	case 0x5c:	case 0x5d:	case 0x5e:
		tmp = adrs - 0x50;
		idx = tmp&0x3;
		slot = stbl[tmp>>2];
		ch[idxoffset+idx]->slot[slot]->setKeyscale( (data>>6)&0x3 );
		ch[idxoffset+idx]->slot[slot]->setAttackRate( data&0x1f );
		break;
		
	// slot - decay rate / amon
	case 0x60:	case 0x61:	case 0x62:
	case 0x64:	case 0x65:	case 0x66:
	case 0x68:	case 0x69:	case 0x6a:
	case 0x6c:	case 0x6d:	case 0x6e:
		tmp = adrs - 0x60;
		idx = tmp&0x3;
		slot = stbl[tmp>>2];
		ch[idxoffset+idx]->slot[slot]->setDecayRate( data&0x1f );
		if( data&0x80 )	ch[idx]->slot[slot]->AMOn();
		else			ch[idx]->slot[slot]->AMOff();
		break;
		
	// slot - sustain rate
	case 0x70:	case 0x71:	case 0x72:
	case 0x74:	case 0x75:	case 0x76:
	case 0x78:	case 0x79:	case 0x7a:
	case 0x7c:	case 0x7d:	case 0x7e:
		tmp = adrs - 0x70;
		idx = tmp&0x3;
		slot = stbl[tmp>>2];
		ch[idxoffset+idx]->slot[slot]->setSustainRate( data&0x1f );
		break;
		
	// slot - sustain level / rerease rate
	case 0x80:	case 0x81:	case 0x82:
	case 0x84:	case 0x85:	case 0x86:
	case 0x88:	case 0x89:	case 0x8a:
	case 0x8c:	case 0x8d:	case 0x8e:
		tmp = adrs - 0x80;
		idx = tmp&0x3;
		slot = stbl[tmp>>2];
		ch[idxoffset+idx]->slot[slot]->setSustainLevel( (data>>4)&0xf );
		ch[idxoffset+idx]->slot[slot]->setReleaseRate( data&0xf );
		break;
		
	// slot - ssgeg-type
	case 0x90:	case 0x91:	case 0x92:
	case 0x94:	case 0x95:	case 0x96:
	case 0x98:	case 0x99:	case 0x9a:
	case 0x9c:	case 0x9d:	case 0x9e:
		tmp = adrs - 0x90;
		idx = tmp&0x3;
		slot = stbl[tmp>>2];
		ch[idxoffset+idx]->slot[slot]->setSSGEGType( data&0xf );
		break;
		
	default:
		handled = false;
		break;
	}

	return handled;
};

bool COPNASsg::setReg( UCHAR adrs, UCHAR data )
{
	INT idx;
	bool handled = true;
	
	switch(adrs){
	case 0x00: // fine tune
	case 0x02:
	case 0x04:
		idx = adrs>>1;
		ch[idx]->setFineTune(data);
		break;
	case 0x01: // coarse tune
	case 0x03:
	case 0x05:
		idx = (adrs-1)>>1;
		ch[idx]->setCoarseTune(data&0x0f);
		break;
		
	case 0x06: // noise period
		setNoisePeriod( data&0x1f );
		break;
	case 0x07: // keyon
		if( data&0x01 ){ ch[0]->toneOn(); }else{ ch[0]->toneOff(); };
		if( data&0x02 ){ ch[1]->toneOn(); }else{ ch[1]->toneOff(); };
		if( data&0x04 ){ ch[2]->toneOn(); }else{ ch[2]->toneOff(); };
		if( data&0x08 ){ ch[0]->noiseOn(); }else{ ch[0]->noiseOff(); };
		if( data&0x10 ){ ch[1]->noiseOn(); }else{ ch[1]->noiseOff(); };
		if( data&0x20 ){ ch[2]->noiseOn(); }else{ ch[2]->noiseOff(); };
		break;
		
	case 0x08: // amp level
	case 0x09:
	case 0x0a:
		idx = adrs-0x08;
		ch[idx]->setLevel(data&0x0f);
		ch[idx]->setUseEnv((data&0x10) ? true: false);
		break;

	case 0x0b: // env fine tune
		setEnvFineTune(data);
		break;
	case 0x0c: // env coarse tune
		setEnvCoarseTune(data);
		break;
	case 0x0d: // env type
		setEnvType(data);
		break;
	case 0x0e: // I/O portA
	case 0x0f: // I/O portB
		break;
	default:
		handled = false;
	}
	return handled;
}

bool COPNARhythm::setReg( UCHAR adrs, UCHAR data )
{
	bool handled = true;
	
	switch(adrs){
	case 0x10: // keyon
		if( data&0x80 ){
			// dump
			if( data&0x1 ) bd->off(); // bd
			if( data&0x2 ) sd->off(); // sd
			if( data&0x4 ) top->off(); // top
			if( data&0x8 ) hh->off(); // hh
			if( data&0x10 ) tom->off(); // tom
			if( data&0x20 ) rim->off(); // rim
		}else{
			// keyon
			if( data&0x1 ) bd->on(); // bd
			if( data&0x2 ) sd->on(); // sd
			if( data&0x4 ) top->on(); // top
			if( data&0x8 ) hh->on(); // hh
			if( data&0x10 ) tom->on(); // tom
			if( data&0x20 ) rim->on(); // rim
		}
		break;
		
	case 0x11: // total level
		setTotalLevel(data&0x3f);
		break;

	case 0x18: // bd
		bd->setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		bd->setLevel( data&0x1f );
		break;
	case 0x19: // sd
		sd->setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		sd->setLevel( data&0x1f );
		break;
	case 0x1a: // top
		top->setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		top->setLevel( data&0x1f );
		break;
	case 0x1b: // hh
		hh->setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		hh->setLevel( data&0x1f );
		break;
	case 0x1c: // tom
		top->setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		top->setLevel( data&0x1f );
		break;
	case 0x1d: // rim
		rim->setLR( (data&0x80)?true:false, (data&0x40)?true:false );
		rim->setLevel( data&0x1f );
		break;
		
	default:
		handled = false;
	}
	return handled;
}

bool COPNAAdpcm::setReg( UCHAR adrs, UCHAR data )
{
	bool handled = true;

	switch(adrs){
	case 0x00: // control 1
		control1 = data;
		
		if((data&0xa0) == 0xa0){	// start & mem
			keyOnLevel = level>>3;
		}
		if(data&0x01){		// reset
			keyOnLevel = 0;
		}
		//(data&0x80) // start
		//(data&0x40) // rec
		//(data&0x20);// mem/data
		//(data&0x10);// repeat
		//(data&0x08);// sp off
		//(data&0x01);// reset
		break;
		
	case 0x01: // control 2
		control2 = data;
		if( data & 0xc0 )
			setLR( ((data&0x80)?true:false), ((data&0x40)?true:false) );
		//data&0x80; // L
		//data&0x40; // R
		//data&0x08; // SAMPLE
		//data&0x04; // DA/AD
		//data&0x02; // RAM TYPE
		//data&0x01; // ROM/DRAM
		break;
		
	case 0x02: /// start addr (L)
		startAddr = (startAddr & 0x1fe000) | ((int)data<<5);
		currentAddr = startAddr;
		break;
	case 0x03: /// start addr (H)
		startAddr = ((int)data<<13) | (startAddr & 0x1fe0);
		currentAddr = startAddr;
		break;
		
	case 0x04: /// stop addr (L)
		stopAddr = (startAddr & 0x1fe000) | ((int)data<<5);
		break;
	case 0x05: /// stop addr (H)
		stopAddr = ((int)data<<13) | (startAddr & 0x1fe0);
		break;

	case 0x06: // prescale(L)
		prescale = (prescale&0xff00) | data;
		break;
	case 0x07: // prescale(H)
		prescale = ((int)data<<8) | (prescale&0xff);
		break;
		
	case 0x08: // adpcm data.
		if( control1 & 0x60 ){
			if( 0<currentAddr && currentAddr < ramsize ) // ”O‚Ì‚½‚ß
				dram[currentAddr] = data;
			if( stopAddr > currentAddr ){
				if( limitAddr > currentAddr ){
					currentAddr++;
				}else{
					currentAddr=0;
				}
			}
		}
		break;
		
	case 0x09: // delta-N(L)
		deltaN = (deltaN&0xff00) | data;
	case 0x0a: // delta-N(H)
		deltaN = ((int)data<<8) | (deltaN&0xff);

	case 0x0b: // level
		level = data;
		break;
		
	case 0x0c: /// limit addr (L)
		limitAddr = (limitAddr<<13) | ((int)data<<5);
		break;
	case 0x0d: /// limit addr (H)
		limitAddr = ((int)data<<13) | (limitAddr<<5);
		break;

	case 0x0e: // dac data.
	case 0x0f: // pcm data.
		break;

	default:
		handled = false;
	}
	
	return handled;
}

void COPNA::applyMask(int ch)
{
	UCHAR data;
	bool mask = getMixedMask(ch);
	
	if( 0<=ch && ch<=2 ){ // FM 1~3
		int fmNo = ch;
		data = (fm->ch[fmNo]->getAMS()<<4) | fm->ch[fmNo]->getPMS();
		if( !mask ){
			bool l,r;
			fm->ch[fmNo]->getLR( l,r );
			data |= (l?0x80:0) | (r?0x40:0);
		}
		pIF->directOut( 0xb4+ch, data );
	}
	else if( 6<=ch && ch<=8 ){ // FM 4~6
		int fmNo = ch - 3;
		data = (fm->ch[fmNo]->getAMS()<<4) | fm->ch[fmNo]->getPMS();
		if( !mask ){
			bool l,r;
			fm->ch[fmNo]->getLR( l,r );
			data |= (l?0x80:0) | (r?0x40:0);
		}
		pIF->directOut( 0x1b4+(ch-6), data );
	}
	else if( 9<=ch && ch<=11 ){ // SSG 1~3
		int ssgNo = ch-9;
		data = mask ? 0 : ssg->ch[ssgNo]->getLevel();
		data |= ssg->ch[ssgNo]->isUseEnv() ? 0x10 : 0;
		pIF->directOut( 0x08+ssgNo, data );
	}
	else if( ch == 12 ){ // ADPCM
		data = mask ? 0 : adpcm->getLevel();
		pIF->directOut( 0x10b, data );
	}
	else if( ch == 13 ){ // RHYTHM
		data = mask ? 0 : rhythm->getTotalLevel();
		pIF->directOut( 0x11, data );
	}
}

void COPNA::setPartMask(int ch, bool mask)
{
	if( mask ){
		partMask |= 1<<ch;
	}else{
		partMask &= ~(1<<ch);
	}
	applyMask(ch);
}

void COPNA::setPartSolo(int ch, bool mask)
{
	if( mask )	partSolo |= 1<<ch;
	else		partSolo &= ~(1<<ch);
	
	for( int i=0; i<14; i++ )
		applyMask(i);
}

