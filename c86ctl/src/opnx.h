/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#pragma once
#include <assert.h>
#include "chip.h"
#include "if.h"
#include "opx.h"

class OPNA;
class OPN3L;
class OPM;

// ---------------------------------------------------------------------------------------
class COPNFmCh : public COPXFmCh {
	friend class COPNFm;
	friend class COPNA;
	friend class COPN3L;
	friend class COPM;
	
public:
	COPNFmCh(IRealChip2 *p) : COPXFmCh(p){
		reset();
	};
	virtual ~COPNFmCh(void){
	};

	virtual void reset(){
		COPXFmCh::reset();
		mclk = 8000000ULL;
		exmode = 0;
		for( int i=0; i<4; i++ ){
			fnum[i] = 0;
			fblock[i] = 0;
			fpacked[i] = 0;
		}
	};

public:
	int getExMode(){ return exmode; };

	int getFNum(){ return fnum[3]; };
	int getFNumEx(int slotno ){ 
		if( !exmode ) slotno = 3;
		return fnum[slotno];
	};
	int getFBlock(){ return fblock[3]; };
	int getFBlockEx(int slotno){ 
		if( !exmode ) slotno = 3;
		return fblock[slotno];
	};
	double getFreqEx(int slotno){
		if( !exmode ) slotno = 3;
		int n = getFNumEx(slotno);
		int b = getFBlockEx(slotno);
		b = 0<b ? 1<<(b-1) : 1;
		double mag = (8*10e6) / (144 * (double)(1<<20));
		return n*b*mag;
	};
	double getFreq(){ return getFreqEx(3); };

public:
#if 0
	void getNoteEx(int exNo, int &oct, int &note){
		// NOTE: k.kotajima 2012/02/29
		// mclkに依存しないテーブルにしたかったので64bit値テーブルにした。
		// fno = (144 * (440*(2**((2*x+1)/24.0))) * (1<<20)) / (1<<(block-1)) :: x=-9~2, block=4
		const uint64_t lim[12] = {
			                // B3<->C4   : 254.1775933119Hz ,  fno: 4797441433.52314
			5082712146ULL,  // C <->C#  : 269.291779527024Hz ,  fno: 5082712146.16792
			5384945938ULL,  // C#<->D   : 285.304702023222Hz ,  fno: 5384945938.11664
			5705151486ULL,  // D <->D#  : 302.26980244078Hz ,  fno: 5705151486.55457
			6044397447ULL,  // D#<->E   : 320.243700225281Hz ,  fno: 6044397447.73364
			6403816023ULL,  // E <->F   : 339.286381589747Hz ,  fno: 6403816023.51331
			6784606739ULL,  // F <->F#  : 359.461399713042Hz ,  fno: 6784606739.97905
			7188040450ULL,  // F#<->G   : 380.836086842703Hz ,  fno: 7188040450.74913
			7615463578ULL,  // G <->G#  : 403.481779010055Hz ,  fno: 7615463578.33046
			8068302607ULL,  // G#<->A   : 427.474054107587Hz ,  fno: 8068302607.6785
			8548068846ULL,  // A <->A#  : 452.892984123137Hz ,  fno: 8548068846.95824
			9056363471ULL,  // A#<->B   : 479.823402372713Hz ,  fno: 9056363471.39466
			                // B4 <->C5   : 508.3551866238Hz ,  fno: 9594882867.04628
			0xffffffffffffffffULL // stopper
		};
		const uint64_t minlim = 4797441433ULL;  // B3<->C4   : 254.1775933119Hz ,  fno: 4797441433.52314
		const uint64_t maxlim = minlim*2;
		
		oct = fblock[exNo];
		uint64_t n = static_cast<uint64_t>(fnum[exNo])*mclk;
		if( !n ){
			oct = 0;
			note = 0;
			return;
		}
		
		while( n<minlim ){
			oct--;
			n<<=1;
		}
		while( maxlim<n ){
			oct++;
			n>>=1;
		}
		int i;
		for( i=0; lim[i]<=n; i++ );
		note = i;
	};
#else
	void getNoteEx(int exNo, int &oct, int &note){
		const uint32_t lim[12] = {
			         // C#<->C   : 254.1775933119Hz
			269291,  // C <->C#  : 269.291779527024Hz
			285304,  // C#<->D   : 285.304702023222Hz
			302269,  // D <->D#  : 302.26980244078Hz
			320243,  // D#<->E   : 320.243700225281Hz
			339286,  // E <->F   : 339.286381589747Hz
			359461,  // F <->F#  : 359.461399713042Hz
			380836,  // F#<->G   : 380.836086842703Hz
			403481,  // G <->G#  : 403.481779010055Hz
			427474,  // G#<->A   : 427.474054107587Hz
			452892,  // A <->A#  : 452.892984123137Hz
			479823,  // A#<->B   : 479.823402372713Hz
			         // B <->C   : 508.3551866238Hz
			0xffffffff // stopper
			};
		const uint32_t minlim = 254177;  // C#<->C   : 254.1775933119Hz
		const uint32_t maxlim = minlim*2;
		
		uint64_t b = fblock[exNo];
		uint64_t n;
		if( oct )
			n = static_cast<uint64_t>(fnum[exNo])*mclk*(1ULL<<(b-1))*1000ULL / (144ULL*(1ULL<<20));
		else
			n = static_cast<uint64_t>(fnum[exNo])*mclk*1000ULL / (288ULL*(1ULL<<20));
		
		if( !n ){
			oct = 0;
			note = 0;
			return;
		}
		
		oct = 4;
		while( n<minlim ){
			oct--;
			n<<=1;
		}
		while( maxlim<n ){
			oct++;
			n>>=1;
		}
		int i;
		for( i=0; lim[i]<=n; i++ );
		note = i;
	};
#endif	

	void getNote(int &oct, int &note){
		getNoteEx(3, oct, note);
	};

protected:
	void setExMode(int mode){ exmode = mode; };
	void setFExLo(int slotno, UCHAR data){
		fpacked[slotno] = (fpacked[slotno] & 0xff00) | data;
		fnum[slotno] = fpacked[slotno] & 0x7ff;
		fblock[slotno] = fpacked[slotno]>>11 & 0x7;
	};
	void setFExHi(int slotno, UCHAR data){
		fpacked[slotno] = ((int)data<<8) | (fpacked[slotno] & 0xff);
		fnum[slotno] = fpacked[slotno] & 0x7ff;
		fblock[slotno] = fpacked[slotno]>>11 & 0x7;
	};
	void setFLo(UCHAR data){ setFExLo(3, data); };
	void setFHi(UCHAR data){ setFExHi(3, data); };
	

protected:
	uint64_t mclk;
	int exmode;
	int fnum[4];	//11bit
	int fblock[4];	//3bit
	int fpacked[4];
	
	IRealChip2 *pIF;
};

// ---------------------------------------------------------------------------------------
class COPNFm{
	friend class COPNA;
	friend class COPN3L;

public:
	COPNFm(IRealChip2 *p) : pIF(p){
		for( int i=0; i<6; i++ )
			ch[i] = new COPNFmCh(p);
		reset();
	};
	virtual ~COPNFm(){
		for( int i=0; i<6; i++ )
			if(ch[i]) delete ch[i];
	};
	
	void reset(){
		lfo = 0;
		lfo_sw = false;
		for( int i=0; i<6; i++ )
			ch[i]->reset();
	};
	void update(){
		for( int i=0; i<6; i++ )
			ch[i]->update();
	};
	
public:
	int getLFO(){ return lfo; };
	
protected:
	bool isLFOOn(){ return lfo_sw; };
	bool setReg( UCHAR bank, UCHAR addr, UCHAR data );
	

public:
	COPNFmCh *ch[6];
	
protected:
	int lfo; //3bit
	bool lfo_sw;

	IRealChip2 *pIF;
};


// ---------------------------------------------------------------------------------------
class COPNSsgCh{
	friend class COPNA;
	friend class COPN3L;
	friend class COPNSsg;

public:
	COPNSsgCh(IRealChip2 *p) : pIF(p){ reset(); };
	virtual ~COPNSsgCh(){};

	void reset(){
		fineTune = 0;
		coarseTune = 0;
		level = 0;
		useEnv = false;
		tone = false;
		noise = false;
		keyOnLevel = 0;
	};
	void update(){
		if(keyOnLevel) keyOnLevel--;
	};
	int getKeyOnLevel(){ return keyOnLevel; };

public:
	int getFineTune(){ return fineTune; };
	int getCoarseTune(){ return coarseTune; };
	int getTune(){ return (coarseTune<<8 | fineTune); }; // 12bit
	
	int getLevel(){ return level; };
	bool isUseEnv(){ return useEnv; };
	
	bool isToneOn(){ return tone; };
	
	bool isNoiseOn(){ return noise; };
	bool isOn(){ return (tone|noise); };

	void getNote(int &oct, int &note){
		const int lim[12] = {
			     // B3 <->C4  : 254.1775933119Hz   ,  TP: 491.782136935309
			464, // C4 <->C4# : 269.291779527024Hz ,  TP: 464.180526488949
			438, // C4#<->D4  : 285.304702023222Hz ,  TP: 438.128075399983
			413, // D4 <->D4# : 302.26980244078Hz  ,  TP: 413.537836034712
			390, // D4#<->E4  : 320.243700225281Hz ,  TP: 390.327740755139
			368, // E4 <->F4  : 339.286381589747Hz ,  TP: 368.420328025855
			347, // F4 <->F4# : 359.461399713042Hz ,  TP: 347.742483893368
			328, // F4#<->G4  : 380.836086842703Hz ,  TP: 328.225197975078
			309, // G4 <->G4# : 403.481779010055Hz ,  TP: 309.803333143539
			292, // G4#<->A4  : 427.474054107587Hz ,  TP: 292.415408137356
			276, // A4 <->A4# : 452.892984123137Hz ,  TP: 276.003392373183
			260, // A4#<->B4  : 479.823402372713Hz ,  TP: 260.512512274054
			     // B4 <->C5  : 508.3551866238Hz   ,  TP: 245.891068467654
			0
		};
		const int maxlim = 492; // B3<->C4
		const int minlim = 246; // B4<->C5
		int n;

		oct = 4;
		note = 0;
		n = getTune();
		// c, c#, d, d#, e, f, f#, g, g#, a, a#, b, c

		if( !n ) return;
		while( n<minlim ){
			oct++;
			n<<=1;
		}
		while( maxlim<n ){
			oct--;
			n>>=1;
		}

		int i;
		for( i=0; n<lim[i]; i++ );
		note = i;
	};
	
protected:
	void setFineTune(int ft){ fineTune = ft; };
	void setCoarseTune(int ct){ coarseTune = ct; };
	void setLevel(int l){ level = l; keyOnLevel = (l<<1)|1; };
	void setUseEnv( bool use ){ useEnv = use; };

	void toneOn(){ tone = true; keyOnLevel = (level<<1)|1; };
	void toneOff(){ tone = false; };
	void noiseOn(){ noise = true; keyOnLevel = (level<<1)|1; };
	void noiseOff(){ noise = false; };
	
protected:
	int fineTune;	// 8bit
	int coarseTune;	// 4bit
	int level;		// 4bit
	bool useEnv;	// M
	bool tone;
	bool noise;
	int keyOnLevel;

	IRealChip2 *pIF;
};

// ---------------------------------------------------------------------------------------
class COPNSsg{
	friend class COPNA;
	friend class COPN3L;

public:
	COPNSsg(IRealChip2 *p) : pIF(p){
		for( int i=0; i<3; i++ )
			ch[i] = new COPNSsgCh(p);
		reset();
	};
	virtual ~COPNSsg(){
		for( int i=0; i<3; i++ )
			if( ch[i] ) delete ch[i];
	};

	void reset(){
		for( int i=0; i<3; i++ )
			ch[i]->reset();
		envFineTune=0;
		envCoarseTune=0;
		envType=0;
		noisePeriod=0;
	};
	void update(){
		for( int i=0; i<3; i++ )
			ch[i]->update();
	};

public:
	COPNSsgCh *ch[3];

public:
	int getEnvFineTune(){ return envFineTune; };
	int getEnvCoarseTune(){ return envCoarseTune; };
	int getEnvType(){ return envType; };
	int getNoisePeriod(){ return noisePeriod; };

protected:
	void setEnvFineTune(int ft){ envFineTune = ft; };
	void setEnvCoarseTune(int ct){ envCoarseTune = ct; };
	void setEnvType(int type){ envType = type&0x7; };
	void setNoisePeriod(int np){ noisePeriod = np; };
	bool setReg( UCHAR addr, UCHAR data );
	
protected:
	int envFineTune;	// 8bit チャネル共通
	int envCoarseTune;	// 8bit チャネル共通
	int envType;		// 4bit チャネル共通
	int noisePeriod;	// 5bit チャネル共通
	
	IRealChip2 *pIF;
};

// ---------------------------------------------------------------------------------------
class COPNRhythmCh{
	friend class COPNRhythm;
	friend class COPNA;
	friend class COPN3L;

public:
	COPNRhythmCh(IRealChip2 *p) : pIF(p){ reset(); };
	virtual ~COPNRhythmCh(){};

	void reset(){
		left=false;
		right=false;
		level=0;
		sw=false;
		keyOnLevel=0;
	};
	
public:
	int getLevel(){ return level; };
	bool isOn(){ return sw; };
	void getLR( bool &l, bool &r ){ l=left; r=right; };
	void update(){
		if( 0<limit ) limit--;
		if( 0<keyOnLevel ) keyOnLevel--;
		else sw = false;
	};
	int getKeyOnLevel(){ return keyOnLevel; };

protected:
	void setLevel(int l){ level = l; };
	void on(){ sw = true; limit = 3; keyOnLevel = level; };
	void off(){ sw = false; };
	void setLR( bool l, bool r ){ left=l; right=r; };
	
protected:
	int level;
	bool sw;
	bool left;
	bool right;
	int limit;
	int keyOnLevel;

	IRealChip2 *pIF;
};

// ---------------------------------------------------------------------------------------
class COPNRhythm{
	friend class COPNA;
	friend class COPN3L;
	
public:
	COPNRhythm(IRealChip2 *p) : pIF(p){
		rim = new COPNRhythmCh(p);
		tom = new COPNRhythmCh(p);
		hh = new COPNRhythmCh(p);
		top = new COPNRhythmCh(p);
		sd = new COPNRhythmCh(p);
		bd = new COPNRhythmCh(p);
		reset();
	};
	virtual ~COPNRhythm(){
		if(rim) delete rim;
		if(tom) delete tom;
		if(hh) delete hh;
		if(top) delete top;
		if(sd) delete sd;
		if(bd) delete bd;
	};

	void reset(){
		rim->reset();
		tom->reset();
		hh->reset();
		top->reset();
		sd->reset();
		bd->reset();
		tl=0;
	};
	void update(){
		rim->update();
		tom->update();
		hh->update();
		top->update();
		sd->update();
		bd->update();
	};
	
	int getTotalLevel(){ return tl; };
	int getKeyOnLevel(){
		int level, x;
		level = rim->getKeyOnLevel();
		x = tom->getKeyOnLevel(); if(level<x) level=x;
		x = hh->getKeyOnLevel();  if(level<x) level=x;
		x = top->getKeyOnLevel(); if(level<x) level=x;
		x = sd->getKeyOnLevel();  if(level<x) level=x;
		x = bd->getKeyOnLevel();  if(level<x) level=x;
		return level;
	};

public:
	COPNRhythmCh *rim;
	COPNRhythmCh *tom;
	COPNRhythmCh *hh;
	COPNRhythmCh *top;
	COPNRhythmCh *sd;
	COPNRhythmCh *bd;
		
protected:
	void setTotalLevel(int level){ tl = level; };
	bool setReg( UCHAR addr, UCHAR data );
	
protected:
	int tl;

	IRealChip2 *pIF;
};

