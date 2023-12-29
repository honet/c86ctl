/***
	c86ctl
	ディレイフィルタ部
	
	Copyright (c) 2009-2014, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once
#include <memory>
#include "ringbuff.h"
#include "basefilter.h"


namespace c86ctl{

class DelayFilter : public BaseFilter
{
public:
	DelayFilter();
	~DelayFilter();

public:
	void reset(void);
	void tick(void);
	int getDelay(int *d);
	int setDelay(int d);

public:
	virtual void byteOut(UINT addr, UCHAR data);

private:
	struct REQ {
		UINT t;
		USHORT addr;
		UCHAR dat;
		UCHAR dummy;
	};

	int delay;
	CRingBuff<REQ> dqueue;
};

typedef std::shared_ptr<DelayFilter> DelayFilterPtr;
};

