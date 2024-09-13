#pragma once
#include "chip.h"

namespace c86ctl {


// ---------------------------------------------------------------------------------------
class CYMZ280BAdpcm {
	friend class CYMZ280B;

	static const size_t ramsize = 16 * 1024 * 1024;	// 16 MB
	static const size_t minimapsize = 512;
public:
	CYMZ280BAdpcm() {
//		sram = new UCHAR[ramsize];
//		map = new UCHAR[ramsize];
//		wav = new SHORT[ramsize * 2];
		minimap = new UCHAR[minimapsize];
		reset();
	}
	virtual ~CYMZ280BAdpcm() {
//		if (sram) delete[] sram;
//		if (map) delete[] map;
//		if (wav) delete[] wav;
		if (minimap) delete[] minimap;
	}

	void reset() {
		for (int i = 0; i < 8; i++) {
			StartAddr[i] = 0;
			EndAddr[i] = 0;
			LoopStartAddr[i] = 0;
			LoopEndAddr[i] = 0;
			keyOnLevel[i] = 0;
			sw[i] = false;
		}
		currentAddr = 0;

//		memset(sram, 0, ramsize);
//		memset(map, 0, ramsize);
//		memset(wav, 0, ramsize * 2 * 2);
		memset(minimap, 0, minimapsize);
		memset(reg, 0, 256);
	}

	void update() {
		UINT i;
		for (i = 0; i < 8; i++) {
			if (keyOnLevel[i]) keyOnLevel[i]--;
		}
	}


	int getKeyOnLevel(UCHAR ch) { return keyOnLevel[ch]; }
	bool isOn(UCHAR ch) { return sw[ch]; }
	int getLevel(UCHAR ch);
	UINT getStartAddr(UCHAR ch);
	UINT getEndAddr(UCHAR ch);

protected:
	bool setReg(UCHAR addr, UCHAR data);

protected:
	UINT StartAddr[8];	//24bit
	UINT EndAddr[8];	//24bit
	UINT LoopStartAddr[8];	//24bit
	UINT LoopEndAddr[8];	//24bit

	UINT currentAddr;	//24bit (write pointer)
	int keyOnLevel[8];

	bool sw[8];
	UCHAR reg[256];

public:
//	UCHAR* sram;
//	UCHAR* map;
//	SHORT* wav;
	UCHAR* minimap;
};


// ---------------------------------------------------------------------------------------

class CYMZ280B : public Chip
{
public:
	CYMZ280B() {
		adpcm = new CYMZ280BAdpcm();
		partMask = 0;
		partSolo = 0;
		reset();
	}
	virtual ~CYMZ280B() {
		if (adpcm) delete adpcm;
	}

	void reset() {
		memset(reg, 0, 256);
		memset(regATime, 0, 256);
		if (adpcm)	adpcm->reset();
		for (int i = 0; i < 8; i++)
			applyMask(i);
	}

	void update() {
		int dc = 8;
		for (int i = 0; i < 256; i++) {
			UCHAR c = regATime[i];
			if (64 < c) {
				c -= dc;
				regATime[i] = 64 < c ? c : 64;
			}
		}
		if (adpcm)	adpcm->update();
	}

public:
	virtual void byteOut(UINT addr, UCHAR data);
	UCHAR getReg(UINT addr) {
		if (addr < 0x100)
			return reg[addr & 0xff];
		return 0;
	}
	virtual void setMasterClock(UINT clock) {}

public:
	void setPartMask(int ch, bool mask);
	void setPartSolo(int ch, bool mask);
	bool getPartMask(int ch) { return partMask & (1 << ch) ? true : false; }
	bool getPartSolo(int ch) { return partSolo & (1 << ch) ? true : false; }
	bool getMixedMask(int ch) {
		if (partSolo) return (((~partSolo) | partMask) & (1 << ch)) ? true : false;
		else return getPartMask(ch);
	}


private:
	bool setReg(UINT addr, UCHAR data) {

		if (0x100 <= addr)
			return false;

		addr &= 0xff;
		reg[addr] = data;
		INT c = regATime[addr];
		c += 64;
		regATime[addr] = 255 < c ? 255 : c;

		if (adpcm)	adpcm->setReg(addr, data);

		return	true;
	}

public:
	CYMZ280BAdpcm* adpcm;

	UCHAR reg[256];
	UCHAR regATime[256];

protected:
	void applyMask(int ch);

protected:
	UINT partMask;
	UINT partSolo;
};
}
