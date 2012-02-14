/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
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

	void reset(){
		for( int i=0; i<2; i++ ){
			memset( reg[i], 0, 256 );
			memset( regATime[i], 0, 256 );
		}
	};
	void update(){
		int dc = 8;
		for( int j=0; j<2; j++ ){
			for( int i=0; i<256; i++ ){
				UCHAR c=regATime[j][i];
				regATime[j][i] = dc<c? c-dc : 0;
			}
		}
	};

public:
	bool setReg( int addr, UCHAR data ){
		if( addr <= 0x0d ) // SSG
			reg[0][addr] = data;
			regATime[0][addr] = 255;
			return true;
		if( 0x10<=addr && addr<=0x1d ) // Rhythm
			reg[0][addr] = data;
			regATime[0][addr] = 255;
			return true;
		if( 0x20<=addr && addr<=0x29 ){ // FM
			if( addr == 0x21 ||		// Test
				addr == 0x26 )		// Timer-B
				return false;
			reg[0][addr] = data;
			regATime[0][addr] = 255;
			return true;
		}
		if( 0x30<=addr && addr<=0xb6 ) // FM param 1-3
			reg[0][addr] = data;
			regATime[0][addr] = 255;
			return true;
		//if( 0x110==addr ) return true; // flag control
		if( 0x130<=addr && addr<=0x1b6 ) // FM param 4-6
			reg[1][addr] = data;
			regATime[1][addr] = 255;
			return true;
		
		return false;
	};

	UCHAR getReg( int addr ){
		
		if( addr < 0x100 ){
			return reg[0][addr];
		}else if( addr < 0x200 ){
			return reg[1][addr];
		}
		return 0;
	};

public:
	UCHAR reg[2][256];
	UCHAR regATime[2][256];
};


