/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#pragma once
#include "chip.h"

// ---------------------------------------------------------------------------------------
class COPNAFmSlot {
public:
	COPNAFmSlot(){ reset(); };
	~COPNAFmSlot(){};

	void reset(void){
		detune = 0;
		multiple = 0;
		keyscale = 0;
		attackRate = 0;
		decayRate = 0;
		sustainRate = 0;
		releaseRate = 0;
		ssgegType = 0;
		sustainLevel = 0;
		totalLevel = 0;
		enable = false;
		AM = false;
	};
	
public:
	void setDetune(int dt){ detune = dt&0x07; };
	int getDetune(){ return detune; };
	double getDetuneHz();
	
	void setMultiple(int mult){ multiple = mult&0x7; };
	int getMultiple(){ return multiple; };
	double getMultipleRatio();
	
	void setKeyscale(int scale){ keyscale = scale&0x3; };
	int getKeyscale(){ return keyscale; };
	void setAttackRate(int ar){ attackRate = ar&0x1f; };
	int getAttackRate(){ return attackRate; };
	void setDecayRate(int dr){ decayRate = dr&0x1f; };
	int getDecayRate(){ return decayRate; };
	void setSustainRate(int sr){ sustainRate = sr&0x1f; };
	int getSustainRate(){ return sustainRate; };
	void setReleaseRate(int rr){ releaseRate = rr&0x1f; };
	int getReleaseRate(){ return releaseRate; };
	void setSSGEGType(int type){ ssgegType = type&0xf; };
	int getSSGEGType(){ return ssgegType; };
	void setSustainLevel(int sl){ sustainLevel = sl&0xf; };
	int getSustainLevel(){ return sustainLevel; };
	void setTotalLevel(int tl){ totalLevel = tl&0x7f; };
	int getTotalLevel(){ return totalLevel; };
	
	void on(){ enable = true; };
	void off(){ enable = false; };
	bool isOn(){ return enable; };
	void AMOn(){ AM = true; };
	void AMOff(){ AM = false; };
	bool isAM(){ return AM; };
	
protected:
	int detune;			// 3bit
	int multiple;		// 4bit
	int keyscale;		// 2bit
	int attackRate;		// 5bit
	int decayRate;		// 5bit
	int sustainRate;	// 5bit
	int releaseRate;	// 4bit
	int ssgegType;		// 4bit
	
	int sustainLevel;	// 4bit
	int totalLevel;	// 7bit

	bool enable;
	bool AM;
};

// ---------------------------------------------------------------------------------------
class COPNAFm {
public:
	COPNAFm(void){ reset(); };
	virtual ~COPNAFm(void){};

	void reset(){
		for( int i=0; i<4; i++ )
			slot[i].reset();

		exmode = 0;
		feedback = 0;
		algorithm = 0;
		ams = 0;
		pms = 0;
		left = false;
		right = false;

		for( int i=0; i<4; i++ ){
			fnum[i] = 0;
			fblock[i] = 0;
		}
	};

public:
	void setExMode(int mode){ exmode = mode; };
	int getExMode(){ return exmode; };

	bool isKeyOn(){
		for( int i=0; i<4; i++ )
			if( slot[i].isOn() )
				return true;
		return false;
	};
	void keyOn( UCHAR slotsw ){
		for( int i=0; i<4; i++ ){ // D3:4, D2:3, D1:2, D0:1
			if( slotsw & 0x01 )
				slot[i].on();
			else
				slot[i].off();
			slotsw >>= 1;
		}
	};
	void setFByRegEx(int slot, UCHAR lo, UCHAR hi ){
		this->fnum[slot] = (hi<<8 | lo) & 0x7ff;
		this->fblock[slot] = hi>>3 & 0x7;
	};
	void setFByReg( UCHAR lo, UCHAR hi ){
		setFByRegEx(3, lo, hi);
	};
	void setFEx(int slot, int fnum, int fblock){
		this->fnum[slot] = fnum & 0x7ff;
		this->fblock[slot] = fblock & 0x7;
	};
	void setF(int fnum, int fblock){
		setFEx(3, fnum, fblock);
	};
	
	int getFNum(){ return fnum[3]; };
	int getFNumEx(int slot){ 
		if( !exmode ) slot = 3;
		return fnum[slot];
	};
	int getFBlock(){ return fblock[3]; };
	int getFBlockEx(int slot){ 
		if( !exmode ) slot = 3;
		return fblock[slot];
	};
	double getFreqEx(int slot){
		if( !exmode ) slot = 3;
		int n = getFNumEx(slot);
		int b = getFBlockEx(slot);
		b = 0<b ? 1<<(b-1) : 1;
		double mag = (8*10e6) / (144 * (double)(1<<20));
		return n*b*mag;
	};
	double getFreq(){ return getFreqEx(3); };

	
	int getAMS(){ return ams; };
	void setAMS(int ams){ this->ams = ams; };
	int getPMS(){ return pms; };
	void setPMS(int pms){ this->pms = pms; };
	int getFeedback(){ return feedback; };
	void setFeedback(int feedback){ this->feedback = feedback; };
	int getAlgorithm(){ return algorithm; };
	void setAlgorithm(int algorithm){ this->algorithm = algorithm; };
	void getLR( bool &l, bool &r ){ l = left; r = right; };
	void setLR( bool l, bool r ){ left = l; right = r; };
	int getPan(){ // ±1
		if( left&&right ) return 0;
		else if(left) return -1;
		else if(right) return 1;
		else return 0;
	};

	void getNoteEx(int exNo, int &oct, int &note){
		if( !exmode ) exNo = 3;
		// note(hz) = (fno * 2^(block-1) * mclock) / (144 * 2^20)
		const int lim[12] = {
			      // B3 <->C4  : 254.1775933119Hz   ,  fno: 599.680179190393 
			635,  // C4 <->C4# : 269.291779527024Hz ,  fno: 635.33901827099
			673,  // C4#<->D4  : 285.304702023222Hz ,  fno: 673.11824226458
			713,  // D4 <->D4# : 302.26980244078Hz  ,  fno: 713.143935819322
			756,  // D4#<->E4  : 320.243700225281Hz ,  fno: 755.549680966705
			800,  // E4 <->F4  : 339.286381589747Hz ,  fno: 800.477002939164
			848,  // F4 <->F4# : 359.461399713042Hz ,  fno: 848.075842497381
			899,  // F4#<->G4  : 380.836086842703Hz ,  fno: 898.505056343642
			952,  // G4 <->G4# : 403.481779010055Hz ,  fno: 951.932947291307
			1009, // G4#<->A4  : 427.474054107587Hz ,  fno: 1008.53782595981
			1069, // A4 <->A4# : 452.892984123137Hz ,  fno: 1068.50860586978
			1132, // A4#<->B4  : 479.823402372713Hz ,  fno: 1132.04543392433
			      // B4 <->C5  : 508.3551866238Hz   ,  fno: 1199.36035838079
			0x7fffffff
		};
		const int minlim =  599; // B3<->C4
		const int maxlim = 1199; // B4<->C5
		int n;
		
		oct = fblock[exNo];
		note = 0;
		n = fnum[exNo];
		// c, c#, d, d#, e, f, f#, g, g#, a, a#, b, c

		if( !n ) return;
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
	void getNote(int &oct, int &note){
		getNoteEx(3, oct, note);
	};

	int getMixLevel(){
		//if( !left && !right ) return 0;
		int level=0;
		for( int i=0; i<4; i++ ){
			level+=slot[i].getTotalLevel();
		}
		return level>>2;
	};
	
public:
	COPNAFmSlot slot[4];

protected:
	int exmode;
	int feedback;	// 3bit
	int algorithm;	// 3bit
	int ams;		// 2bit
	int pms;		// 3bit
	bool left;
	bool right;
	
	int fnum[4];	//11bit
	int fblock[4];	//3bit
	
};

// ---------------------------------------------------------------------------------------
class COPNASsgCh{
public:
	COPNASsgCh(){ reset(); };
	virtual ~COPNASsgCh(){};

	void reset(){
		fineTune = 0;
		coarseTune = 0;
		level = 0;
		useEnv = false;
		tone = false;
		noise = false;
	};

public:
	int getFineTune(){ return fineTune; };
	void setFineTune(int ft){ fineTune = ft; };
	int getCoarseTune(){ return coarseTune; };
	void setCoarseTune(int ct){ coarseTune = ct; };
	int getTune(){ return (coarseTune<<8 | fineTune); }; // 12bit
	
	int getLevel(){ return level; };
	void setLevel(int l){ level = l; };
	void setUseEnv( bool use ){ useEnv = use; };
	bool isUseEnv(){ return useEnv; };
	
	void toneOn(){ tone = true; };
	void toneOff(){ tone = false; };
	bool isToneOn(){ return tone; };
	
	void noiseOn(){ noise = true; };
	void noiseOff(){ noise = false; };
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
	int fineTune;	// 8bit
	int coarseTune;	// 4bit
	int level;		// 4bit
	bool useEnv;	// M
	bool tone;
	bool noise;
};

class COPNASsg{
public:
	COPNASsg(){ reset(); };
	virtual ~COPNASsg(){};

	void reset(){
		for( int i=0; i<3; i++ )
			ch[i].reset();
		envFineTune=0;
		envCoarseTune=0;
		envType=0;
		noisePeriod=0;
	};

public:
	COPNASsgCh ch[3];

public:
	int getEnvFineTune(){ return envFineTune; };
	void setEnvFineTune(int ft){ envFineTune = ft; };
	int getEnvCoarseTune(){ return envCoarseTune; };
	void setEnvCoarseTune(int ct){ envCoarseTune = ct; };
	int getEnvType(){ return envType; };
	void setEnvType(int type){ envType = type&0x7; };
	int getNoisePeriod(){ return noisePeriod; };
	void setNoisePeriod(int np){ noisePeriod = np; };
		
protected:
	int envFineTune;	// 8bit チャネル共通
	int envCoarseTune;	// 8bit チャネル共通
	int envType;		// 4bit チャネル共通
	int noisePeriod;	// 5bit チャネル共通
};

// ---------------------------------------------------------------------------------------
class COPNARhythmCh{
public:
	COPNARhythmCh(){ reset(); };
	virtual ~COPNARhythmCh(){};

	void reset(){
		left=false;
		right=false;
		level=0;
		sw=false;
	};
	
public:
	void setLevel(int l){ level = l; };
	int getLevel(){ return level; };
	void on(){ sw = true; limit = 3; };
	void off(){ sw = false; };
	bool isOn(){ return sw; };
	void setLR( bool l, bool r ){ left=l; right=r; };
	void getLR( bool &l, bool &r ){ l=left; r=right; };
	void update(){
		if( 0<limit ) limit--;
		else sw = false;
	};
	
protected:
	int level;
	bool sw;
	bool left;
	bool right;
	int limit;
};

class COPNARhythm{
public:
	COPNARhythm(){ reset(); };
	virtual ~COPNARhythm(){};

	void reset(){
		rim.reset();
		tom.reset();
		hh.reset();
		top.reset();
		sd.reset();
		bd.reset();
		tl=0;
	};

public:
	COPNARhythmCh rim;
	COPNARhythmCh tom;
	COPNARhythmCh hh;
	COPNARhythmCh top;
	COPNARhythmCh sd;
	COPNARhythmCh bd;
		
public:
	void setTotalLevel(int level){ tl = level; };
	int getTotalLevel(){ return tl; };
	void update(){
		rim.update();
		tom.update();
		hh.update();
		top.update();
		sd.update();
		bd.update();
	};
	
protected:
	int tl;
};

class COPNAAdpcm{
	
public:
	COPNAAdpcm(){ reset(); };
	virtual ~COPNAAdpcm(){};

	void reset(){
		startAddr = 0;
		stopAddr = 0;
		limitAddr = 0;
		prescale = 0;
		deltaN = 0;
		level = 0;
	};

protected:
	int startAddr;	//16bit
	int stopAddr;	//16bit
	int limitAddr;	//16bit
	int prescale;	//16bit
	int deltaN;		//16bit
	int level;		//8bit;
	
};

// ---------------------------------------------------------------------------------------
class COPNA : public Chip
{
public:
	COPNA(){ reset(); };
	virtual ~COPNA(){};

	void reset(){
		for( int i=0; i<6; i++ )
			fm[i].reset();
		for( int i=0; i<3; i++ )
			ssg.reset();
		rhythm.reset();
		adpcm.reset();

		for( int i=0; i<2; i++ ){
			memset( reg[i], 0, 256 );
			memset( regATime[i], 0, 256 );
		}
	};
	
	
public:
	void update(){
		int dc = 8;
		for( int j=0; j<2; j++ ){
			for( int i=0; i<256; i++ ){
				UCHAR c=regATime[j][i];
				regATime[j][i] = dc<c? c-dc : 0;
			}
		}
		rhythm.update();
	};

public:
	bool setReg( int addr, UCHAR data );
	UCHAR getReg( int addr );

	int getTimerA(){ return timerA; };
	int getTimerB(){ return timerB; };
	int getFMPrescale(){ return prescale_fm; };
	int getSSGPrescale(){ return prescale_ssg; };
	int getLFO(){ return lfo; };
	bool isLFOOn(){ return lfo_sw; };


public:
	COPNAFm fm[6];
	COPNASsg ssg;
	COPNAAdpcm adpcm;
	COPNARhythm rhythm;
	
	UCHAR reg[2][256];
	UCHAR regATime[2][256];

	UCHAR *adpcm_data; // not used at the present time...


protected:
	bool fmCommonRegHandling( UCHAR adrs, UCHAR data );
	bool fmRegHandling( UCHAR bank, UCHAR adrs, UCHAR data );
	bool adpcmRegHandling( UCHAR adrs, UCHAR data );
	bool ssgRegHandling( UCHAR adrs, UCHAR data );
	bool rhythmRegHandling( UCHAR adrs, UCHAR data );
	
protected:
	int timerA; //10bit
	int timerB; //8bit
	int prescale_fm;
	int prescale_ssg;
	
	int lfo; //3bit
	bool timerA_sw;
	bool timerB_sw;
	bool lfo_sw;
	//int ch3mode;
};


