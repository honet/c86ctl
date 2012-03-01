/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#pragma once
#include "chip.h"
#include "opx.h"

// ---------------------------------------------------------------------------------------
#if 0
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
			if( 64<c ){
				c-=dc;
				regATime[i] = 64<c? c : 64;
			}
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
#endif


// ---------------------------------------------------------------------------------------

class COPMFm{
	friend class COPM;

public:
	COPMFm(IRealChip2 *p) : pIF(p){
		for( int i=0; i<9; i++ )
			ch[i] = new COPXFmCh(p);
		reset();
	};
	virtual ~COPMFm(){
		for( int i=0; i<9; i++ )
			if(ch[i]) delete ch[i];
	};
	
	void reset(){
		lfo = 0;
		lfo_sw = false;
		for( int i=0; i<9; i++ )
			ch[i]->reset();
	};
	
	void update(){
		for( int i=0; i<9; i++ )
			ch[i]->update();
	};
	
public:
	int getLFO(){ return lfo; };
	
protected:
	bool isLFOOn(){ return lfo_sw; };
	bool setReg( UCHAR bank, UCHAR addr, UCHAR data );
	

public:
	COPXFmCh *ch[9];
	
protected:
	int lfo; //3bit
	bool lfo_sw;

	IRealChip2 *pIF;
};


// ---------------------------------------------------------------------------------------

class COPM : public Chip
{
public:
	COPM(IRealChip2 *p) : pIF(p) {
		reset();
		fm = new COPMFm(p);
	};
	virtual ~COPM(){
		if(fm) delete fm;
	};
	
	void reset(){
		memset( reg, 0, 256 );
		memset( regATime, 0, 256 );
	}

public:
	virtual void filter( int addr, UCHAR *data );
	virtual bool setReg( int addr, UCHAR data );
	virtual UCHAR getReg( int addr );
	
	void setPartMask(int ch, bool mask);
	void setPartSolo(int ch, bool mask);
	bool getPartMask(int ch){ return partMask&(1<<ch) ? true : false; };
	bool getPartSolo(int ch){ return partSolo&(1<<ch) ? true : false; };
	bool getMixedMask(int ch){
		if( partSolo ) return (((~partSolo) | partMask) & (1<<ch)) ? true : false;
		else return getPartMask(ch);
	};
	
public:
	void update(){
		int dc = 4;
		for( int i=0; i<256; i++ ){
			UCHAR c=regATime[i];
			if( 64<c ){
				c-=dc;
				regATime[i] = 64<c ? c : 64;
			}
		}
	};

public:
	UCHAR reg[256];
	UCHAR regATime[256];
	
public:
	COPMFm *fm;
	
protected:
	bool fmCommonRegHandling( UCHAR adrs, UCHAR data );
	void applyMask(int ch);
	
protected:
	UINT partMask;
	UINT partSolo;
	IRealChip2 *pIF;
};

