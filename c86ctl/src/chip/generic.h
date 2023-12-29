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
class CGenericChipBank1 : public Chip
{
public:
	CGenericChipBank1(){ reset(); }
	virtual ~CGenericChipBank1(){}

	void reset(){
		memset( reg, 0, 256 );
		memset( regATime, 0, 256 );
	}
	
	void update(){
		int dc = 8;
		for( int i=0; i<256; i++ ){
			UCHAR c=regATime[i];
			if( 64<c ){
				c-=dc;
				regATime[i] = 64<c? c : 64;
			}
		}
	}

public:
	void byteOut( UINT addr, UCHAR data ){
		if(setReg(addr,data))
			if(ds) ds->byteOut(addr,data);
	}
	
	UCHAR getReg( UINT addr ){
		if( addr < 0x100 )
			return reg[addr&0xff];
		return 0;
	}
	virtual void setMasterClock( UINT clock ){}

private:
	bool setReg( UINT addr, UCHAR data ){
		if( 0x100 <= addr )
			return false;
		
		addr &= 0xff;
		reg[addr] = data;
		INT c = regATime[addr];
		c+=64;
		regATime[addr] = 255<c ? 255 : c;
		return true;
		//return false;
	}

public:
	UCHAR reg[256];
	UCHAR regATime[256];
};


// ---------------------------------------------------------------------------------------
class CGenericChipBank2 : public Chip
{
public:
	CGenericChipBank2(){ reset(); }
	virtual ~CGenericChipBank2(){}

	void reset(){
		memset( reg, 0, 256*2 );
		memset( regATime, 0, 256*2 );
	}
	
	void update(){
		int dc = 8;
		for( int j=0; j<2; j++ ){
			for( int i=0; i<256; i++ ){
				UCHAR c=regATime[j][i];
				if( 64<c ){
					c-=dc;
					regATime[j][i] = 64<c? c : 64;
				}
			}
		}
	}

public:
	void byteOut( UINT addr, UCHAR data ){
		if(setReg(addr,data))
			if(ds) ds->byteOut(addr,data);
	}
	
	UCHAR getReg( UINT addr ){
		if( addr < 0x100 )
			return reg[0][addr&0xff];
		else if( addr < 0x200 )
			return reg[1][addr&0xff];
		return 0;
	}
	virtual void setMasterClock( UINT clock ){}

private:
	bool setReg( UINT addr, UCHAR data ){
		if( 0x200 <= addr )
			return false;
		int idx = 0;
		if( 0x100 <= addr ){
			addr -= 0x100;
			idx = 1;
		}
		
		reg[idx][addr] = data;
		INT c = regATime[idx][addr];
		c+=64;
		regATime[idx][addr] = 255<c ? 255 : c;
		return true;
	}

public:
	UCHAR reg[2][256];
	UCHAR regATime[2][256];
};

};

