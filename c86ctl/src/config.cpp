/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include "config.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

class CC86CtlConfig gConfig;

BOOL CC86CtlConfig::writeInt( LPCTSTR section, LPCTSTR key, INT val )
{
	TCHAR cfg[128];
	_stprintf( cfg, TEXT("%d"), val );
	return ::WritePrivateProfileString( section, key, cfg, inipath );
}

