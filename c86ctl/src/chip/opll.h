/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#pragma once
#include "chip.h"

namespace c86ctl{

// ---------------------------------------------------------------------------------------
class COPLL : public Chip
{
public:
	COPLL(){ reset(); };
	virtual ~COPLL(){};

	void reset(){
		memset( reg, 0, 256 );
		memset( regATime, 0, 256 );
	};
	
	void update(){
		int dc = 8;
		for( int i=0; i<256; i++ ){
			UCHAR c=regATime[i];
			if( 64<c ){
				c-=dc;
				regATime[i] = 64<c? c : 64;
			}
		}
	};

public:
	void byteOut( UINT addr, UCHAR data ){
		if(setReg(addr,data))
			if(ds) ds->byteOut(addr,data);
	};
	
	UCHAR getReg( UINT addr ){
		if( addr < 0x100 )
			return reg[addr&0xff];
		return 0;
	};
	virtual void setMasterClock( UINT clock ){};

private:
	bool setReg( UINT addr, UCHAR data ){
		if( 0x100 <= addr )
			return false;
		
		addr &= 0xff;
		reg[addr] = data;
		regATime[addr] = 255;
		return true;
		//return false;
	};

public:
	UCHAR reg[256];
	UCHAR regATime[256];
};

};

