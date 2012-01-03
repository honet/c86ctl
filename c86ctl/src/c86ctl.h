/***
	c86ctl
	
	Copyright (c) 2009-2010, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */


#ifndef _C86CTL_H
#define _C86CTL_H

#ifdef __cplusplus
extern "C" {
#endif


// エラーコード定義
#define C86CTL_ERR_NONE						0
#define C86CTL_ERR_UNKNOWN					-1
#define C86CTL_ERR_NOT_IMPLEMENTED			-9999
#define C86CTL_ERR_NODEVICE					-1000


// 公開ファンクション
int WINAPI c86ctl_initialize(void);
int WINAPI c86ctl_deinitialize(void);
int WINAPI c86ctl_reset(void);

void WINAPI c86ctl_out( UINT addr, UCHAR data );
UCHAR WINAPI c86ctl_in( UINT addr );

// ver1.1 追加ファンクション
//C86CTL_API INT c86ctl_get_version(UINT *ver);
//C86CTL_API INT c86ctl_out2(UINT module, UINT addr, UCHAR adata );
//C86CTL_API INT c86ctl_in2(UINT module, UINT addr, UCHAR data );
//C86CTL_API INT c86ctl_set_pll_clock(UINT module, UINT clock );
//C86CTL_API INT c86ctl_set_volume(UINT module, UINT ch, UINT vol );


	
#ifdef __cplusplus
}
#endif

#endif
