/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once
#include "interface/if.h"

namespace c86ctl{

class BaseFilter : public IByteInput
{
public:
	BaseFilter() : ds(0) {
	};

public:
	virtual void reset(){};
	virtual void update(){};
	
	virtual void connect(IByteInput *downstream)
	{
		ds = downstream;
	}
	virtual void disconnect()
	{
		ds = NULL;
	}

public:
	virtual void byteOut(UINT addr, UCHAR data){
		if(ds) ds->byteOut(addr, data);
	};

protected:
	IByteInput* ds;
};

};


