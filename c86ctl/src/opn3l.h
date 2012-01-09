/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#pragma once
#include "chip.h"

// ---------------------------------------------------------------------------------------
class COPN3L : public Chip
{
public:
	COPN3L(){ reset(); };
	virtual ~COPN3L(){};

	void reset(){};
	void update(){};

public:
	bool setReg( int addr, UCHAR data ){
		if( addr <= 0x0d ) // SSG
			regmem[addr] = data;
			return true;
		if( 0x10<=addr && addr<=0x1d ) // Rhythm
			regmem[addr] = data;
			return true;
		if( 0x20<=addr && addr<=0x29 ){ // FM
			if( addr == 0x21 ||		// Test
				addr == 0x26 )		// Timer-B
				return false;
			regmem[addr] = data;
			return true;
		}
		if( 0x30<=addr && addr<=0xb6 ) // FM param 1-3
			regmem[addr] = data;
			return true;
		//if( 0x110==addr ) return true; // flag control
		if( 0x130<=addr && addr<=0x1b6 ) // FM param 4-6
			regmem[addr] = data;
			return true;
		
		return false;
	};

	UCHAR getReg( int addr ){
		if( addr < 0x200 ){
			return regmem[addr];
		}
		return 0;
	};

protected:
	UCHAR regmem[512];
};


