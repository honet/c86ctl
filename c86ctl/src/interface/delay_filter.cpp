/***
	c86ctl
	ディレイフィルタ
	
	Copyright (c) 2009-2013, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
*/


#include "stdafx.h"
#include <mmsystem.h>
#include "delay_filter.h"

namespace c86ctl {

DelayFilter::DelayFilter() : delay(0)
{
	dqueue.alloc(1024 * 128);
}

DelayFilter::~DelayFilter()
{
}

void DelayFilter::byteOut(UINT addr, UCHAR data)
{
	if (0 < delay) {
		REQ r = { ::timeGetTime() + delay, static_cast<USHORT>(addr), data };
		dqueue.push(r);
		return;
	} else {
		ds->byteOut(addr, data);
	}
}

void DelayFilter::reset(void)
{
	// ディレイキューの廃棄
	dqueue.flush();
}

void DelayFilter::tick(void)
{
	if (!dqueue.isempty()) {
		UINT t = timeGetTime();
		while (!dqueue.isempty() && t >= dqueue.front()->t) {
			//if( rbuff.remain()<4 ) break;
			REQ req;
			dqueue.pop(&req);
			//out2buf( req.addr, req.dat );
			ds->byteOut(req.addr, req.dat);
		}
	}

}
int DelayFilter::setDelay(int d)
{
	if (d != delay) {
		delay = d;
	}
	return C86CTL_ERR_NONE;
}

int DelayFilter::getDelay(int* d)
{
	if (d) {
		*d = delay;
		return C86CTL_ERR_NONE;
	}
	return C86CTL_ERR_INVALID_PARAM;
}

}


