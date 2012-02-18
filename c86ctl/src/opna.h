/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#pragma once
#include "chip.h"
#include "if.h"

// ---------------------------------------------------------------------------------------
class COPNAFmSlot {
	friend class COPNAFmCh;
	friend class COPNAFm;
	friend class COPNA;
	
public:
	COPNAFmSlot(COPNA *parent) : pOPNA(parent){ reset(); };
	~COPNAFmSlot(){};

	void reset(void){
		detune = 0;
		multiple = 0;
		keyscale = 0;
		attackRate = 0;
		decayRate = 0;
		sustainRate = 0;
		releaseRate = 63;
		ssgegType = 0;
		sustainLevel = 63;
		totalLevel = 127;
		enable = false;
		AM = false;
	};
	
public:
	int getDetune(){ return detune; };
	double getDetuneHz();

	int getMultiple(){ return multiple; };
	double getMultipleRatio();
	
	int getKeyscale(){ return keyscale; };
	int getAttackRate(){ return attackRate; };
	int getDecayRate(){ return decayRate; };
	int getSustainRate(){ return sustainRate; };
	int getReleaseRate(){ return releaseRate; };
	int getSSGEGType(){ return ssgegType; };
	int getSustainLevel(){ return sustainLevel; };
	int getTotalLevel(){ return totalLevel; };
	
	bool isOn(){ return enable; };
	bool isAM(){ return AM; };

public:
	void sendDetune(int dt){
		setDetune(dt);
	};
	
protected:
	void setDetune(int dt){ detune = dt&0x07; };
	void setMultiple(int mult){ multiple = mult&0x7; };
	void setKeyscale(int scale){ keyscale = scale&0x3; };
	void setAttackRate(int ar){ attackRate = ar&0x1f; };
	void setDecayRate(int dr){ decayRate = dr&0x1f; };
	void setSustainRate(int sr){ sustainRate = sr&0x1f; };
	void setReleaseRate(int rr){ releaseRate = rr&0x1f; };
	void setSSGEGType(int type){ ssgegType = type&0xf; };
	void setSustainLevel(int sl){ sustainLevel = sl&0xf; };
	void setTotalLevel(int tl){ totalLevel = tl&0x7f; };
	void on(){ enable = true; };
	void off(){ enable = false; };
	void AMOn(){ AM = true; };
	void AMOff(){ AM = false; };

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

	COPNA *pOPNA;
};

// ---------------------------------------------------------------------------------------
class COPNAFmCh {
	friend class COPNAFm;
	friend class COPNA;
	
public:
	COPNAFmCh(COPNA *parent) : pOPNA(parent){
		for( int i=0; i<4; i++ )
			slot[i] = new COPNAFmSlot(parent);
		reset();
	};
	virtual ~COPNAFmCh(void){
		for( int i=0; i<4; i++ )
			if(slot[i]) delete slot[i];
	};

	void reset(){
		for( int i=0; i<4; i++ )
			slot[i]->reset();

		exmode = 0;
		feedback = 0;
		algorithm = 0;
		ams = 0;
		pms = 0;
		left = true;
		right = true;

		for( int i=0; i<4; i++ ){
			fnum[i] = 0;
			fblock[i] = 0;
			keyOnLevel[i] = 0;
		}
	};
	void update(){
		for( int i=0; i<4; i++ )
			if(keyOnLevel[i]) keyOnLevel[i]--;
	};
	int getKeyOnLevel(){ return keyOnLevel[3]; };
	int getKeyOnLevelEx(int idx){ return keyOnLevel[idx]; };

public:
	int getExMode(){ return exmode; };

	bool isKeyOn(){
		if( left || right ){
			for( int i=0; i<4; i++ )
				if( slot[i]->isOn() )
					return true;
		}
		return false;
	};
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

	
	int getAMS(){ return ams; };
	int getPMS(){ return pms; };
	int getFeedback(){ return feedback; };
	int getAlgorithm(){ return algorithm; };
	void getLR( bool &l, bool &r ){ l = left; r = right; };
	int getPan(){ // ±1
		if( left&&right ) return 0;
		else if(left) return -1;
		else if(right) return 1;
		else return -2;
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
			level+=slot[i]->getTotalLevel();
		}
		return level>>2;
	};

protected:
	void setExMode(int mode){ exmode = mode; };
	void keyOn( UCHAR slotsw ){
		for( int i=0; i<4; i++ ){ // D3:4, D2:3, D1:2, D0:1
			if( slotsw & 0x01 ){
				slot[i]->on();
				if( exmode )
					keyOnLevel[i] = (127-slot[i]->getTotalLevel())>>2;
			}else
				slot[i]->off();
			slotsw >>= 1;
		}
		if( !exmode ){
			keyOnLevel[3] = (127-getMixLevel())>>2;
		}
	};
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
	
	void setAMS(int ams){ this->ams = ams; };
	void setPMS(int pms){ this->pms = pms; };
	void setFeedback(int feedback){ this->feedback = feedback; };
	void setAlgorithm(int algorithm){ this->algorithm = algorithm; };
	void setLR( bool l, bool r ){ left = l; right = r; };
	
public:
	COPNAFmSlot *slot[4];

protected:
	int exmode;
	int feedback;	// 3bit
	int algorithm;	// 3bit
	int ams;		// 2bit
	int pms;		// 3bit
	bool left;
	bool right;
	int keyOnLevel[4];
	
	int fnum[4];	//11bit
	int fblock[4];	//3bit
	int fpacked[4];
	
	COPNA *pOPNA;
};

// ---------------------------------------------------------------------------------------
class COPNAFm{
	friend class COPNA;

public:
	COPNAFm(COPNA *parent) : pOPNA(parent){
		for( int i=0; i<6; i++ )
			ch[i] = new COPNAFmCh(parent);
		reset();
	};
	virtual ~COPNAFm(){
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
	COPNAFmCh *ch[6];
	
protected:
	int lfo; //3bit
	bool lfo_sw;

	COPNA *pOPNA;
};


// ---------------------------------------------------------------------------------------
class COPNASsgCh{
	friend class COPNA;
	friend class COPNASsg;

public:
	COPNASsgCh(COPNA *parent) : pOPNA(parent){ reset(); };
	virtual ~COPNASsgCh(){};

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

		oct = 3;
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

	COPNA *pOPNA;
};

// ---------------------------------------------------------------------------------------
class COPNASsg{
	friend class COPNA;

public:
	COPNASsg(COPNA *parent) : pOPNA(parent){
		for( int i=0; i<3; i++ )
			ch[i] = new COPNASsgCh(parent);
		reset();
	};
	virtual ~COPNASsg(){
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
	COPNASsgCh *ch[3];

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
	
	COPNA *pOPNA;
};

// ---------------------------------------------------------------------------------------
class COPNARhythmCh{
	friend class COPNARhythm;
	friend class COPNA;

public:
	COPNARhythmCh(COPNA *parent) : pOPNA(parent){ reset(); };
	virtual ~COPNARhythmCh(){};

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

	COPNA *pOPNA;
};

// ---------------------------------------------------------------------------------------
class COPNARhythm{
	friend class COPNA;
	
public:
	COPNARhythm(COPNA *parent) : pOPNA(parent){
		rim = new COPNARhythmCh(parent);
		tom = new COPNARhythmCh(parent);
		hh = new COPNARhythmCh(parent);
		top = new COPNARhythmCh(parent);
		sd = new COPNARhythmCh(parent);
		bd = new COPNARhythmCh(parent);
		reset();
	};
	virtual ~COPNARhythm(){
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
	COPNARhythmCh *rim;
	COPNARhythmCh *tom;
	COPNARhythmCh *hh;
	COPNARhythmCh *top;
	COPNARhythmCh *sd;
	COPNARhythmCh *bd;
		
protected:
	void setTotalLevel(int level){ tl = level; };
	bool setReg( UCHAR addr, UCHAR data );
	
protected:
	int tl;

	COPNA *pOPNA;
};

// ---------------------------------------------------------------------------------------
class COPNAAdpcm{
	friend class COPNA;

	static const size_t ramsize = 256*1024;
public:
	COPNAAdpcm(COPNA *parent) : pOPNA(parent){
		dram = new UCHAR[ramsize];
		reset();
	};
	virtual ~COPNAAdpcm(){
		if( dram ) delete [] dram;
	};

	void reset(){
		startAddr = 0;
		stopAddr = 0;
		limitAddr = 0;
		currentAddr = 0;
		prescale = 0;
		deltaN = 0;
		level = 0;
		keyOnLevel = 0;
	};
	void update(){
		if(keyOnLevel) keyOnLevel--;
	};

	void getLR( bool &l, bool &r ){ l = left; r = right; };
	int getPan(){ // ±1
		if( left&&right ) return 0;
		else if(left) return -1;
		else if(right) return 1;
		else return -2;
	};

	int getKeyOnLevel(){ return keyOnLevel; };
	
public:
	int getLevel(){ return level; };
	
protected:
	bool setReg( UCHAR addr, UCHAR data );
	void setLR( bool l, bool r ){ left = l; right = r; };

protected:
	int startAddr;	//21bit
	int stopAddr;	//21bit
	int limitAddr;	//21bit
	int currentAddr;//21bit (write pointer)
	int prescale;	//16bit
	int deltaN;		//16bit
	int level;		//8bit;
	int keyOnLevel;
	
	UCHAR control1;
	UCHAR control2;
	
	bool left;
	bool right;
	
	UCHAR *dram;

	COPNA *pOPNA;
};

// ---------------------------------------------------------------------------------------
class COPNA : public Chip
{
public:
	COPNA(GimicIF *p) : pIF(p) {
		fm = new COPNAFm(this);
		ssg = new COPNASsg(this);
		adpcm = new COPNAAdpcm(this);
		rhythm = new COPNARhythm(this);
		partMask = 0;
		partSolo = 0;
		reset();
	};
	virtual ~COPNA(){
		if(fm) delete fm;
		if(ssg) delete ssg;
		if(adpcm) delete adpcm;
		if(rhythm) delete rhythm;
	};

	void reset(){
		for( int i=0; i<2; i++ ){
			memset( reg[i], 0, 256 );
			memset( regATime[i], 0, 256 );
		}

		for( int i=0x40; i<=0x4e;i++ ) // FM TL=127
			reg[0][i] = reg[1][i] = 0x7f;
		for( int i=0x80; i<=0x8e; i++ ) // FM SL,RR
			reg[0][i] = reg[1][i] = 0xff;
		for( int i=0xb4; i<0xb6+1; i++) // FM PAN/AMS/PMS
			reg[0][i] = reg[1][i] = 0xc0;
		reg[0][0x27] = 0x30; // Timer Control
		reg[0][0x29] = 0x80; // FM4-6 Enable
		reg[0][0x07] = 0x38; // SSG ミキサ
		reg[0][0x10] = 0xBF;

		fm->reset();
		ssg->reset();
		rhythm->reset();
		adpcm->reset();

		// 強制的にOPNAモードに切り替え
		pIF->directOut( 0x29, 0x9f );
		reg[0][0x29] = 0x9f;

		for( int i=0; i<14; i++ )
			applyMask(i);
	};
	
	
public:
	void update(){
		int dc = 4;
		for( int j=0; j<2; j++ ){
			for( int i=0; i<256; i++ ){
				UCHAR c=regATime[j][i];
				if( 64<c ){
					c-=dc;
					regATime[j][i] = 64<c ? c : 64;
				}
			}
		}
		fm->update();
		ssg->update();
		adpcm->update();
		rhythm->update();
	};

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
	

	int getTimerA(){ return timerA; };
	int getTimerB(){ return timerB; };
	int getFMPrescale(){ return prescale_fm; };
	int getSSGPrescale(){ return prescale_ssg; };


public:
	COPNAFm *fm;
	COPNASsg *ssg;
	COPNAAdpcm *adpcm;
	COPNARhythm *rhythm;
	
	UCHAR reg[2][256];
	UCHAR regATime[2][256];

protected:
	bool fmCommonRegHandling( UCHAR adrs, UCHAR data );
	void applyMask(int ch);
	
protected:
	int timerA; //10bit
	int timerB; //8bit
	int prescale_fm;
	int prescale_ssg;
	
//	int lfo; //3bit
	bool timerA_sw;
	bool timerB_sw;
//	bool lfo_sw;
	//int ch3mode;
	
	UINT partMask;
	UINT partSolo;
	GimicIF *pIF;
};


