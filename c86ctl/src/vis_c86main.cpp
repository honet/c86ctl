/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <typeinfo>

#include "config.h"
#include "version.h"
#include "module.h"
#include "resource.h"
#include "vis_c86sub.h"
#include "vis_c86main.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


void CVisC86Main::attach( std::vector< std::shared_ptr<GimicIF> > &g )
{
	gimic = g;
	updateInfo();
}

void CVisC86Main::detach( void )
{
	gimic.clear();
}

void CVisC86Main::updateInfo(void)
{
	size_t sz = gimic.size();
	info.resize(sz);
	for( size_t i=0; i<sz; i++ ){
		gimic[i]->getMBInfo(&info[i].mbinfo);
		gimic[i]->getFWVer(&info[i].major, &info[i].minor, &info[i].rev, &info[i].build);
		gimic[i]->getModuleInfo(&info[i].chipinfo);
		gimic[i]->getPLLClock(&info[i].clock);
		gimic[i]->getModuleType(&info[i].chiptype);
	}
}


bool CVisC86Main::create(void)
{
	int left = gConfig.getInt( INISC_MAIN, INIKEY_WNDLEFT, INT_MIN );
	int top  = gConfig.getInt( INISC_MAIN, INIKEY_WNDTOP,  INT_MIN );

	// CHECKME: createの前にattachされてないといけない。
	size_t sz = info.size();

	// frame=19 + topinfo=40 + module=64*N + bottominfo=10
	windowHeight = 19+40+64*sz+10;
	windowWidth = 334;
	
	if ( !CVisWnd::create( left, top, windowWidth, windowHeight,
						   0, WS_POPUP | WS_CLIPCHILDREN ) ){
		return false;
	}

	::ShowWindow( hWnd, SW_SHOWNOACTIVATE );
	
	int y=40;
	
	for( size_t i=0; i<sz; i++ ){
		info[i].checkReg = CVisCheckBoxPtr(new CVisCheckBox(this,260,y, "REGISTER"));
		info[i].checkReg->changeEvent.push_back(
			[this, i](CVisWidget* w){
				hwinfo *info = &this->info[i];
				if( dynamic_cast<CVisCheckBox*>(w)->getValue() ){
					info->regView = visC86RegViewFactory(this->gimic[i]->getChip(), 0);
					info->regView->create(hWnd);
					this->manager->add( info->regView.get() );
				}else{
					this->manager->del( info->regView.get() );
					info->regView = 0;
				}
			} );
		widgets.push_back(info[i].checkReg);

		// TODO: OPNA以外を書く。
		if( info[i].chiptype == CHIP_OPNA ){
			info[i].checkKey = CVisCheckBoxPtr(new CVisCheckBox(this,180,y, "KEYBOARD"));
			info[i].checkKey->changeEvent.push_back(
				[this, i](CVisWidget* w){
					hwinfo *info = &this->info[i];
					if( dynamic_cast<CVisCheckBox*>(w)->getValue() ){
						info->keyView = visC86KeyViewFactory(this->gimic[i]->getChip(), 0);
						info->keyView->create(hWnd);
						this->manager->add( info->keyView.get() );
					}else{
						this->manager->del( info->keyView.get() );
						info->keyView = 0;
					}
				} );
			widgets.push_back(info[i].checkKey);

			for( int ch=0; ch<6; ch++ ){
				char str[10];
				sprintf(str, "FM%d", ch+1);
				info[i].checkFM[ch] = CVisCheckBoxPtr(new CVisCheckBox(this,180 + 40*(ch%3), y+((ch/3)+1)*14, str));
				info[i].checkFM[ch]->changeEvent.push_back(
					[this, i, ch](CVisWidget* w){
						hwinfo *info = &this->info[i];
						if( dynamic_cast<CVisCheckBox*>(w)->getValue() ){
							info->fmView[ch] = visC86FmViewFactory(this->gimic[i]->getChip(), i, ch);
							info->fmView[ch]->create(hWnd);
							this->manager->add( info->fmView[ch].get() );
						}else{
							this->manager->del( info->fmView[ch].get() );
							info->fmView[ch] = 0;
						}
					} );
				widgets.push_back(info[i].checkFM[ch]);
			}

			info[i].checkSSG = CVisCheckBoxPtr(new CVisCheckBox(this,180,y+42, "SSG"));
			widgets.push_back(info[i].checkSSG);

			info[i].checkRHYTHM = CVisCheckBoxPtr(new CVisCheckBox(this,220,y+42, "RHYTHM"));
			widgets.push_back(info[i].checkRHYTHM);

			info[i].checkADPCM = CVisCheckBoxPtr(new CVisCheckBox(this,280,y+42, "ADPCM"));
			widgets.push_back(info[i].checkADPCM);
		}
		y+=64;
	}

	return true;
}

void CVisC86Main::close(void)
{
	saveConfig();
	::SendMessage( hWnd, WM_CLOSE, 0, 0 );
}

void CVisC86Main::saveConfig(void)
{
	RECT rc;
	::GetWindowRect( getHWND(), &rc );
	gConfig.writeInt( INISC_MAIN, INIKEY_WNDLEFT, rc.left );
	gConfig.writeInt( INISC_MAIN, INIKEY_WNDTOP, rc.top );
	gConfig.writeInt( INISC_MAIN, INIKEY_WNDVISIBLE, ::IsWindowVisible(getHWND())?1:0 );
}

void CVisC86Main::onPaintClient()
{
	CVisC86Skin *skin = &gVisSkin;
	const int maxlen = 256;
	char str[maxlen];

	int cw = clientCanvas->getWidth();
	int ch = clientCanvas->getHeight();

	// 背景消去
	visFillRect( clientCanvas, 0, 0, cw, ch, ARGB(255,0,0,0) );
	
	// ロゴ・バージョン
	skin->drawLogo( clientCanvas, 2, 2 );
	skin->drawStr( clientCanvas, 0, 130, 7, "OPx STATUS DISP FOR GIMIC/C86USB" );
	skin->drawStr( clientCanvas, 0, 130, 17, "(C)HONET " VERSION_MESSAGE );

	// 横線
	int w = clientCanvas->getWidth()-4;
	UINT *p = (UINT*)clientCanvas->getPtr(2,28);
	int step = clientCanvas->getStep()/2;
	int r=199,g=200,b=255;
	for( int x=0; x<w; x++ ){
		int d = tick*2+(w-x);
		int a = abs(d%512 - 255);
		*p = *(p+step) = ARGB(255,(r*a)>>8,(g*a)>>8,(b*a)>>8);
		p++;
	}
	tick++;

	int y=40;
	for( size_t i=0; i<gimic.size(); i++ ){
		// 枠線
		visFillRect( clientCanvas, 5, y-2, 5, 58, col_mid );
		visFillRect( clientCanvas, 5, y+54, 320, 2, col_mid );
		// デバイス名
		sprintf(str, "MODULE%d: %s Rev.%c", i, &info[i].mbinfo.Devname[0], info[i].mbinfo.Rev );
		skin->drawStr( clientCanvas, 1, 15, y, str );
		// Firmware version
		sprintf( str, "FW-VER : %d.%d.%d.%d", info[i].major, info[i].minor, info[i].rev, info[i].build );
		skin->drawStr( clientCanvas, 1, 15, y+10, str );
		// MODULE名
		sprintf(str, "CHIP   : %s Rev.%c", &info[i].chipinfo.Devname[0], info[i].chipinfo.Rev );
		skin->drawStr( clientCanvas, 1, 15, y+20, str );
		// PLL Clock
		sprintf(str, "CLOCK  : %d Hz", info[i].clock );
		skin->drawStr( clientCanvas, 1, 15, y+30, str );
		// カロリー(w
		sprintf(str, "CALORIE: %d CPS", gimic[i]->getCPS() );
		skin->drawStr( clientCanvas, 1, 15, y+40, str );
		y+=64;
	}

	// FPS
	sprintf(str, "FPS: %0.1f", manager->getCurrentFPS() );
	skin->drawStr( clientCanvas, 0, 260, ch-12, str );
	
}


