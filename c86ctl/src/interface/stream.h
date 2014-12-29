/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#include "interface/if.h"
#include "interface/delay_filter.h"
#include "chip/chip.h"

namespace c86ctl{
	
class Stream {
public:
	Stream();
	~Stream();
	
public:
	DelayFilter *delay;
	Chip *chip;
	BaseSoundModule *module;
	
public:
	static Stream* Build(BaseSoundModule *module);
};

typedef std::shared_ptr<Stream> StreamPtr;

};

