/***
	RingBuffer module
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#ifndef RINGBUFF_H_
#define RINGBUFF_H_


class CRingBuff {
public:
	CRingBuff();
	~CRingBuff();
	
	BOOL alloc( UINT sz );
	VOID free(VOID);
	UINT remain(VOID);
	UINT get_length(VOID);
	UCHAR read1(VOID);
	BOOL read( UCHAR *data, UINT sz );
	VOID write1( UCHAR data );
	BOOL write( UCHAR *data, UINT sz );

protected:
	HANDLE hMutex;
	UCHAR *p;
	UINT sz;
	UINT mask;
	volatile UINT len;
	volatile UINT widx;
	volatile UINT ridx;
};

#endif

