/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#pragma once
#include "chip.h"

// ---------------------------------------------------------------------------------------
class COPM : public Chip
{
public:
	COPM(){ reset(); };
	virtual ~COPM(){};

	void reset(){
		memset( reg, 0, 256 );
		memset( regATime, 0, 256 );
	};
	
	void update(){
		int dc = 8;
		for( int i=0; i<256; i++ ){
			UCHAR c=regATime[i];
			regATime[i] = dc<c? c-dc : 64;
		}
	};

public:
	bool setReg( int addr, UCHAR data ){
		if( 0x20 <= addr ){
			reg[addr] = data;
			regATime[addr] = 255;
			return true;
		}
		switch( addr ){
		case 0x01:
			data &= 0x02; // LFO RESET
			reg[addr] = data;
			regATime[addr] = 255;
			return true;
		case 0x08:
		case 0x0f:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x14:
		case 0x18:
		case 0x19:
		case 0x1b:
			reg[addr] = data;
			regATime[addr] = 255;
			return true;
		}
		
		return false;
	}
	UCHAR getReg( int addr ){
		if( addr < 0x100 )
			return reg[addr];
		return 0;
	};

public:
	UCHAR reg[256];
	UCHAR regATime[256];
};

