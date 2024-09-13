#include "stdafx.h"
#include <stdlib.h>
#include "ymz280b.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

using namespace c86ctl;

int c86ctl::CYMZ280BAdpcm::getLevel(UCHAR ch)
{
	return	(ch < 8) ? (int)reg[(ch * 4) + 2] : 0;
}

UINT c86ctl::CYMZ280BAdpcm::getStartAddr(UCHAR ch)
{
	return	(ch < 8) ? StartAddr[ch] : 0;
}

UINT c86ctl::CYMZ280BAdpcm::getEndAddr(UCHAR ch)
{
	return	(ch < 8) ? EndAddr[ch] : 0;
}

bool CYMZ280BAdpcm::setReg(UCHAR adrs, UCHAR data)
{
	bool handled = true;
	static bool isPlayed = false;

	UCHAR ch;
	reg[adrs] = data;

	switch (adrs) {
	case 0x01:
	case 0x05:
	case 0x09:
	case 0x0d:
	case 0x11:
	case 0x15:
	case 0x19:
	case 0x1d:
		ch = (adrs - 1) / 4;
		if (data & 0x80) {		//Key 0n
			isPlayed = true;
			StartAddr[ch] = (reg[(0x20 + (4 * ch))] << 16) | (reg[(0x40 + (4 * ch))] << 8) | reg[(0x60 + (4 * ch))];
			EndAddr[ch] = (reg[(0x23 + (4 * ch))] << 16) | (reg[(0x43 + (4 * ch))] << 8) | reg[(0x63 + (4 * ch))];
			keyOnLevel[ch] = reg[(ch * 4) + 2] >> 3;
			sw[ch] = true;
			if (data & 0x10) {	//Loop有効
				LoopStartAddr[ch] = (reg[(0x21 + (4 * ch))] << 16) | (reg[(0x41 + (4 * ch))] << 8) | reg[(0x61 + (4 * ch))];
				LoopEndAddr[ch] = (reg[(0x22 + (4 * ch))] << 16) | (reg[(0x42 + (4 * ch))] << 8) | reg[(0x62 + (4 * ch))];
			}
		}
		else {
			keyOnLevel[ch] = 0;
			sw[ch] = false;
		}
		break;
	case 0x84:
	case 0x85:
	case 0x86:
		currentAddr = (reg[0x84] << 16) | (reg[0x85] << 8) | reg[0x86];
		break;
	case 0x87:
		if (isPlayed) {
			memset(minimap, 0, minimapsize);
			isPlayed = false;
		}
//		sram[currentAddr] = data;
//		map[currentAddr] |= 0x01;
		minimap[currentAddr >> 15] |= 0x01;	//32KBブロック毎に使用チェック
		if (++currentAddr >= (1024 * 1024 * 16))	currentAddr = 0;
		break;

	default:
		handled = false;
	}

	return	handled;
}


////////////////////////////////////////////////////////////////////////////////

void CYMZ280B::byteOut(UINT addr, UCHAR data)
{
	int ch;
	if (0x100 <= addr) return;

	switch (addr) {
	case 0x02:
	case 0x06:
	case 0x0a:
	case 0x0e:
	case 0x12:
	case 0x16:
	case 0x1a:
	case 0x1e:
		ch = (addr - 2) / 4;
		if (getMixedMask(ch))	data = 0;
		break;
	}

	if (setReg(addr, data))
		if (ds) ds->byteOut(addr, data);
}

void CYMZ280B::applyMask(int ch)
{
	UCHAR data = 0;
	UINT adrs = 0;

	if (!ds) return;
	if ((ch < 0) || (ch > 7))	return;

	bool mask = getMixedMask(ch);

	data = (mask) ? 0 : adpcm->getLevel(ch);
	adrs = (ch * 4) + 2;	// TL reg
	ds->byteOut(adrs, data);
}


void CYMZ280B::setPartMask(int ch, bool mask)
{
	if (ch < 0 || 8 <= ch) return;

	if (mask) {
		partMask |= 1 << ch;
	}
	else {
		partMask &= ~(1 << ch);
	}
	applyMask(ch);
}

void CYMZ280B::setPartSolo(int ch, bool mask)
{
	if (ch < 0 || 8 <= ch) return;

	if (mask)	partSolo |= 1 << ch;
	else		partSolo &= ~(1 << ch);

	for (int i = 0; i < 8; i++)
		applyMask(i);
}

