/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#pragma once

extern TCHAR inipath[_MAX_PATH];

//#define INIFILE			TEXT(".\\c86ctl.ini")
#define INISC_MAIN		TEXT("c86ctl")
#define INISC_KEY		TEXT("c86key")
#define INISC_REG		TEXT("c86reg")
#define INISC_FMx		TEXT("c86fm%d")

#define INIKEY_WNDLEFT		TEXT("wndleft")
#define INIKEY_WNDTOP		TEXT("wndtop")
#define INIKEY_WNDVISIBLE	TEXT("wndvisible")

#define getConfigInt( section, key, defval )	GetPrivateProfileInt( section, key, defval, inipath )
#define writeConfig( section, key, val )		WritePrivateProfileString( section, key, val, inipath )

BOOL writeConfigInt( LPCTSTR section, LPCTSTR key, INT val );

