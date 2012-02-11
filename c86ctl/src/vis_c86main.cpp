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
	
	//auto bt1 = std::shared_ptr<CVisButton>(new CVisButton(50,50));
	//bt1->click.push_back( HANDLER_DELEGATE( OnButton1Down ) );
	//ADD_HANDLER( bt1->click, onButton1Down );
	//widgets.push_back(bt1);
	
	//auto knob1 = std::shared_ptr<CVisKnob>(new CVisKnob(this,50,100));
	//ADD_HANDLER( knob1->changeEvent, onButton1Down );
	//widgets.push_back(knob1);

	int y=40;
	
	for( size_t i=0; i<sz; i++ ){
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
/*
		info[i].checkFM2 = CVisCheckBoxPtr(new CVisCheckBox(this,220,y+14, "FM2"));
		widgets.push_back(info[i].checkFM2);

		info[i].checkFM3 = CVisCheckBoxPtr(new CVisCheckBox(this,260,y+14, "FM3"));
		widgets.push_back(info[i].checkFM3);

		info[i].checkFM4 = CVisCheckBoxPtr(new CVisCheckBox(this,180,y+28, "FM4"));
		widgets.push_back(info[i].checkFM4);

		info[i].checkFM5 = CVisCheckBoxPtr(new CVisCheckBox(this,220,y+28, "FM5"));
		widgets.push_back(info[i].checkFM5);

		info[i].checkFM6 = CVisCheckBoxPtr(new CVisCheckBox(this,260,y+28, "FM6"));
		widgets.push_back(info[i].checkFM6);
*/
		info[i].checkSSG = CVisCheckBoxPtr(new CVisCheckBox(this,180,y+42, "SSG"));
		widgets.push_back(info[i].checkSSG);
		
		info[i].checkRHYTHM = CVisCheckBoxPtr(new CVisCheckBox(this,220,y+42, "RHYTHM"));
		widgets.push_back(info[i].checkRHYTHM);
		
		info[i].checkADPCM = CVisCheckBoxPtr(new CVisCheckBox(this,280,y+42, "ADPCM"));
		widgets.push_back(info[i].checkADPCM);
		
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
	for( int i=0; i<gimic.size(); i++ ){
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

/*
VOID CVisC86Main::onButton1Down( CVisWidget *sender ){
	OutputDebugString(_T("click\r\n"));
}

void CVisC86Main::onCheckRegView(CVisWidget* w)
{
	if( 0<info.size() ){
		if( info[0].checkReg->getValue() ){
			info[0].regView = visC86RegViewFactory(gimic[0]->getChip(), 0);
			info[0].regView->create(hWnd);
			manager->add( info[0].regView.get() );
		}else{
			manager->del(info[0].regView.get() );
			info[0].regView = 0;
		}
	}
}
*/
#if 0
LRESULT CALLBACK CVisC86Main::wndProc(HWND hWnd , UINT msg , WPARAM wp , LPARAM lp)
{
	switch (msg) {
	case WM_CREATE:
		{
#if 0
			const int offsetx=20, offsety=20;
			TCHAR cname[128], wname[128];
			int left, top, enable;
			RECT rc;

//			regWnd.attach( chip );
//			keyWnd.attach( chip );
//			for( int i=0; i<6; i++ )
//				fmWnd[i].attach(&chip->fm[i]);
			if( typeid(chip) == typeid(COPNA*) ){
			::GetWindowRect(hWnd, &rc);
			left = gConfig.getInt( INISC_KEY, INIKEY_WNDLEFT, rc.left+offsetx );
			top  = gConfig.getInt( INISC_KEY, INIKEY_WNDTOP, rc.top+offsety );
			enable = gConfig.getInt( INISC_KEY, INIKEY_WNDVISIBLE, TRUE );
			keyWnd.create( TEXT("C86KEY"), TEXT("C86 KEYBOARD VIEW"), left, top, hWnd );
			if( enable )
				::ShowWindow( keyWnd.getFrameHWND(), SW_SHOWNOACTIVATE );
			
			left = gConfig.getInt( INISC_REG, INIKEY_WNDLEFT, left+offsetx );
			top  = gConfig.getInt( INISC_REG, INIKEY_WNDTOP,  top+offsety  );
			enable = gConfig.getInt( INISC_REG, INIKEY_WNDVISIBLE, TRUE );
			regWnd.create( TEXT("C86REG"), TEXT("C86 REGISTER VIEW"), left, top, hWnd );
			if( enable )
				::ShowWindow( regWnd.getFrameHWND(), SW_SHOWNOACTIVATE );

			for( int i=0; i<6; i++ ){
				_stprintf( cname, INISC_FMx, i+1 );
				left = gConfig.getInt( cname, INIKEY_WNDLEFT, left+offsetx );
				top  = gConfig.getInt( cname, INIKEY_WNDTOP,  top+offsety  );
				enable = gConfig.getInt( cname, INIKEY_WNDVISIBLE, TRUE );
				_stprintf( cname, TEXT("C86FM%d"), i+1 );
				_stprintf( wname, TEXT("C86 FM PARAMETER VIEW CH%d"), i+1 );
				fmWnd[i].create( cname, wname, left, top, hWnd );
				if(i==2) fmWnd[i].setExMode(true);
				if( enable )
					::ShowWindow( fmWnd[i].getFrameHWND(), SW_SHOWNOACTIVATE );
			}
			}
//			::SetTimer( hWnd, 0, 50, NULL );
#endif
		}
		break;
		
	case WM_CLOSE:
		{
#if 0
			detach();
			
			RECT rc;
			TCHAR cname[128];
			::GetWindowRect( getFrameHWND(), &rc );
			gConfig.writeInt( INISC_MAIN, INIKEY_WNDLEFT, rc.left );
			gConfig.writeInt( INISC_MAIN, INIKEY_WNDTOP, rc.top );
			gConfig.writeInt( INISC_MAIN, INIKEY_WNDVISIBLE, ::IsWindowVisible(getFrameHWND())?1:0 );

			::GetWindowRect( keyWnd.getFrameHWND(), &rc );
			gConfig.writeInt( INISC_KEY, INIKEY_WNDLEFT, rc.left );
			gConfig.writeInt( INISC_KEY, INIKEY_WNDTOP, rc.top );
			gConfig.writeInt( INISC_KEY, INIKEY_WNDVISIBLE, ::IsWindowVisible(keyWnd.getFrameHWND())?1:0 );

			::GetWindowRect( regWnd.getFrameHWND(), &rc );
			gConfig.writeInt( INISC_REG, INIKEY_WNDLEFT, rc.left );
			gConfig.writeInt( INISC_REG, INIKEY_WNDTOP, rc.top );
			gConfig.writeInt( INISC_REG, INIKEY_WNDVISIBLE, ::IsWindowVisible(regWnd.getFrameHWND())?1:0 );

			for( int i=0; i<6; i++ ){
				::GetWindowRect( fmWnd[i].getFrameHWND(), &rc );
				_stprintf( cname, INISC_FMx, i+1 );
				gConfig.writeInt( cname, INIKEY_WNDLEFT, rc.left );
				gConfig.writeInt( cname, INIKEY_WNDTOP,  rc.top  );
				gConfig.writeInt( cname, INIKEY_WNDVISIBLE, ::IsWindowVisible(fmWnd[i].getFrameHWND())?1:0 );
			}
#endif
		}
		return DefWindowProc(hWnd , msg , wp , lp);

	case WM_TIMER:
#if 0
		tick++;
		if(chip) chip->update();
		::InvalidateRect( hWnd, NULL, FALSE );
		regWnd.update();
		keyWnd.update();
		for( int i=0; i<6; i++ )
			fmWnd[i].update();
		break;
#endif
	case WM_PAINT:
		OnPaint();
		break;

	case WM_LBUTTONUP:
		OnLButtonUp( wp, LOWORD(lp), HIWORD(lp) );

	default:
		return DefWindowProc(hWnd , msg , wp , lp);
	}
	return 0;
}

#endif


