/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once
#include "interface/basefilter.h"

namespace c86ctl{

class Chip : public BaseFilter
{
public:
	Chip(){}
	virtual ~Chip(){}
	
public:
	virtual void reset(){}
	virtual void update(){}

public:
	virtual void byteOut( UINT addr, UCHAR data ){}
	virtual UCHAR getReg( int addr ){ return 0; }
	virtual void setMasterClock( UINT clock ){}
	virtual UINT getMasterClock(){ return 0; }
};

typedef std::shared_ptr<Chip> ChipPtr;
}
