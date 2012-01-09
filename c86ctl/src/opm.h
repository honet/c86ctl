/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
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

	void reset(){};
	void update(){};

public:
	bool setReg( int addr, UCHAR data ){
		if( 0x20 <= addr )
			regmem[addr] = data;
			return true;
		switch( addr ){
		case 0x08:
		case 0x0f:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x14:
		case 0x18:
		case 0x19:
		case 0x1b:
			regmem[addr] = data;
			return true;
		}
		
		return false;
	}
	UCHAR getReg( int addr ){
		if( addr < 0x100 )
			return regmem[addr];
		return 0;
	};

protected:
	UCHAR regmem[256];
};

