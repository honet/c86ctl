/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */


#ifndef _C86CTL_H
#define _C86CTL_H

#ifdef C86CTL_EXPORTS
#define C86CTL_API __declspec(dllexport)
#else
#define C86CTL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

C86CTL_API int WINAPI c86ctl_initialize(void);
C86CTL_API int WINAPI c86ctl_deinitialize(void);
C86CTL_API int WINAPI c86ctl_reset(void);

C86CTL_API void WINAPI c86ctl_out( UINT addr, UCHAR data );
C86CTL_API UCHAR WINAPI c86ctl_in( UINT addr );

#ifdef __cplusplus
}
#endif

#endif
