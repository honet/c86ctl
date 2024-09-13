/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#include <cstdint>
#include "interface/if.h"
#include "interface/delay_filter.h"
#include "chip/chip.h"
#include "chip/opm.h"
#include "chip/opna.h"
#include "chip/opn3l.h"
#include "chip/opl3.h"
#include "chip/opll.h"
#include "chip/tms3631.h"
#include "chip/ymz280b.h"
#include "chip/generic.h"
#include "stream.h"

namespace c86ctl {

Stream::Stream() : module(0), chip(0), delay(0)
{
}
Stream::~Stream()
{
	if (chip) delete chip;
	if (delay) delete delay;
}

Stream* Stream::Build(BaseSoundModule* module)
{
	Chip* chip = NULL;
	ChipType type = module->getChipType();
	switch (type) {
	case CHIP_OPN3L:
		chip = new COPN3L();
		break;
	case CHIP_OPM:
		chip = new COPM();
		break;
	case CHIP_OPNA:
	case CHIP_YMF297_OPN3L:
	case CHIP_YM2608NOADPCM:
		chip = new COPNA();
		break;
	case CHIP_OPL3:
	case CHIP_YMF297_OPL3:
		chip = new COPL3();
		break;
	case CHIP_OPLL:
		chip = new COPLL();
		break;
		//case CHIP_SPC;

	//case CHIP_SN76489:
	//case CHIP_SN76496:
	//ncase CHIP_AY38910:
	case CHIP_YM2149:
	case CHIP_YM2203:
	case CHIP_Y8950:
	case CHIP_Y8950ADPCM:
	case CHIP_YM3526:
	case CHIP_YM3812:
	case CHIP_YMZ770C:
	case CHIP_YMZ771:
		chip = new CGenericChipBank1();
		break;
	case CHIP_YM2612:
	case CHIP_YMF271:
	case CHIP_YMF278B:
	case CHIP_YM2610B:
	case CHIP_YM3438:
		chip = new CGenericChipBank2();
		break;

	case CHIP_TMS3631RI104:
		chip = new CTMS3631();
		break;

	case CHIP_YMZ280B:
		chip = new CYMZ280B();
		break;
	}
	// 未対応モジュールの場合を考慮
	if (!chip)
		return NULL;


	Stream* s = new Stream();
	if (s) {
		s->module = module;
		s->chip = chip;
		s->delay = new DelayFilter();

		s->delay->connect(chip);
		s->chip->connect(module);
	}

	return s;
}

}

