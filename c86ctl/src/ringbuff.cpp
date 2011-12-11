/***
	RingBuffer module
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#include "stdafx.h"
#include <windows.h>
#include <process.h>
#include "ringbuff.h"

CRingBuff::CRingBuff()
{
	hMutex = CreateMutex(NULL, FALSE, NULL);
	p = NULL;
	sz = 0;
	mask = 0;
	len = 0;
	widx = 0;
	ridx = 0;
}

CRingBuff::~CRingBuff()
{
	free();
}

// sz is must be a power of 2.
BOOL CRingBuff::alloc( UINT sz )
{
	free();
	p = new UCHAR[sz];
	if( p ){
		sz = sz;
		mask = sz - 1;
		widx = 0;
		ridx = 0;
		len = 0;
	}
	return p ? TRUE : FALSE;
}

VOID CRingBuff::free(VOID)
{
	if( hMutex )
		CloseHandle(hMutex);
	if( p )
		delete [] p;
	
	hMutex = NULL;
	p = NULL;
	sz = 0;
	mask = 0;
	len = 0;
	widx = 0;
	ridx = 0;
}

UINT CRingBuff::remain(VOID)
{
	return sz - len;
	
}

UINT CRingBuff::get_length(VOID)
{
	return len;
}

UCHAR CRingBuff::read1(VOID)
{
	UCHAR *pd = p + ridx;
	UCHAR d = *pd;
	
	WaitForSingleObject(hMutex, INFINITE);
	{
		ridx = (ridx+1)&mask;
		len--;
	}
	ReleaseMutex(hMutex);
	return d;
}

BOOL CRingBuff::read( UCHAR *data, UINT sz )
{
	UINT i;
	WaitForSingleObject(hMutex, INFINITE);
	{
		for( i=0; i<sz; i++ ){
			UCHAR *pd = p + ridx;
			data[i] = *pd;
			ridx = (ridx+1)&mask;
			len--;
		}
	}
	ReleaseMutex(hMutex);
	return TRUE;
}

VOID CRingBuff::write1( UCHAR data )
{
	UCHAR *pd = p + widx;
	*pd = data;
	
	WaitForSingleObject(hMutex, INFINITE);
	{
		widx = (widx+1)&mask;
		len++;
	}
	ReleaseMutex(hMutex);
}

BOOL CRingBuff::write( UCHAR *data, UINT sz )
{
	UINT i;
	
	WaitForSingleObject(hMutex, INFINITE);
	for( i=0; i<sz; i++ ){
		UCHAR *pd = p + widx;
		*pd = *data++;
		widx = (widx+1)&mask;
		len++;
	}
	ReleaseMutex(hMutex);
	return TRUE;
}
