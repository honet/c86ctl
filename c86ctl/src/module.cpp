/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include "module.h"

HINSTANCE gModule = 0;

HINSTANCE getModuleHandle()
{
	return gModule;
}
