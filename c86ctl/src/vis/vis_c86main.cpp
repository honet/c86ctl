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
#include "resource.h"
#include "vis_c86sub.h"
#include "vis_c86main.h"
#include "c86ctlmain.h"
#include "c86ctlmainwnd.h"
#include "../interface/if_c86usb_winusb.h"
#include "../interface/if_gimic_winusb.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define INIKEY_REGWND		TEXT("m%d_regwnd")
#define INIKEY_KEYWND		TEXT("m%d_keywnd")
#define INIKEY_FMWND		TEXT("m%d_fm%d_wnd")

using namespace c86ctl;
using namespace c86ctl::vis;

static const int modHeight = 94;

#define KEYBUFLEN 32

bool CVisC86Main::update()
{
	int sz = static_cast<int>(GetC86CtlMain()->getNStreams());
	int st = static_cast<int>(info.size());

	if (sz == st)
		return true;

#if 0
	auto devices = GetC86CtlMain()->getDevices();
	int sz = static_cast<int>(info.size());
	int st = sz;

	if (sz == devices.size())
		return true;

	// TODO:listで管理に
	auto it = info.begin();
	for (int i = 0; i < sz; i++) {
		int nmodule = devices[i]->getNumberOfModules();
		for (int k = 0; k < nmodule; k++) {
			if (it->id != devices[i]->nodeid) {
				// 新しいデバイス
				info.insert(it, newinfo);
			}
			it++;
		}
	}
#endif

	info.resize(sz);
	for (size_t i = 0; i < sz; i++) {
		Stream* s = GetC86CtlMain()->getStream(i);

		info[i].chiptype = s->module->getChipType();
		char str[128];

		IFirmwareVersionInfo* fwinfo = dynamic_cast<IFirmwareVersionInfo*>(s->module->getParentDevice());
		if (fwinfo) {
			UINT major, minor, rev, build;
			fwinfo->getFWVer(&major, &minor, &rev, &build);
			sprintf_s(str, sizeof(str), "%u.%u.%u.%u", major, minor, rev, build);
			info[i].verstr.assign(str);
		}


		switch (info[i].chiptype) {
		case CHIP_YM2608:
			info[i].module_name.assign("YM2608"); break;
		case CHIP_YM2608NOADPCM:
			info[i].module_name.assign("YM2608(NOADPCM)"); break;
		case CHIP_YM2151:
			info[i].module_name.assign("YM2151"); break;
		case CHIP_YMF288:
			info[i].module_name.assign("YMF288"); break;
		case CHIP_YMF262:
			info[i].module_name.assign("YMF262"); break;
		case CHIP_YM2413:
			info[i].module_name.assign("YMF2413"); break;
		case CHIP_SN76489:
			info[i].module_name.assign("SN76489"); break;
		case CHIP_SN76496:
			info[i].module_name.assign("SN76496"); break;
		case CHIP_AY38910:
			info[i].module_name.assign("AY-3-8910"); break;
		case CHIP_YM2149:
			info[i].module_name.assign("YM2149"); break;
		case CHIP_YM2203:
			info[i].module_name.assign("YM2203"); break;
		case CHIP_YM2612:
			info[i].module_name.assign("YM2612"); break;
		case CHIP_YM3526:
			info[i].module_name.assign("YM3526"); break;
		case CHIP_YM3812:
			info[i].module_name.assign("YM3812"); break;
		case CHIP_YMF271:
			info[i].module_name.assign("YMF271"); break;
		case CHIP_YMF278B:
			info[i].module_name.assign("YMF278"); break;
		case CHIP_YMZ280B:
			info[i].module_name.assign("YMZ280B"); break;
		case CHIP_YMF297_OPN3L:
			info[i].module_name.assign("YMF297(OPN3L)"); break;
		case CHIP_YMF297_OPL3:
			info[i].module_name.assign("YMF297(OPL3)"); break;
		case CHIP_YM2610B:
			info[i].module_name.assign("YM2610B"); break;
		case CHIP_Y8950:
			info[i].module_name.assign("Y8950"); break;
		case CHIP_Y8950ADPCM:
			info[i].module_name.assign("Y8950 (w/ADPCM)"); break;
		case CHIP_YM3438:
			info[i].module_name.assign("YM3438"); break;
		case CHIP_TMS3631RI104:
			info[i].module_name.assign("TMS3631-RI104(REMAP)"); break;
		}

		GimicWinUSB::GimicModuleWinUSB* gimic_module = NULL;
		C86WinUSB::C86ModuleWinUSB* c86 = NULL;

		if (gimic_module = dynamic_cast<GimicWinUSB::GimicModuleWinUSB*>(s->module)) {
			Devinfo chipinfo;
			gimic_module->getModuleInfo(&chipinfo);
			sprintf_s(str, sizeof(str), "%s Rev.%c", &chipinfo.Devname[0], chipinfo.Rev);
			info[i].board_name.assign(str);

			GimicWinUSB* gimic_dev = dynamic_cast<GimicWinUSB*>(s->module->getParentDevice());
			if (gimic_dev) {
				Devinfo mbinfo;
				gimic_dev->getMBInfo(&mbinfo);
				sprintf_s(str, sizeof(str), "%s Rev.%c", &mbinfo.Devname[0], mbinfo.Rev);
				info[i].device_name.assign(str);
			}
		} else if (c86 = dynamic_cast<C86WinUSB::C86ModuleWinUSB*>(s->module)) {
			char slotname[] = { 'A', 'B', 'C', 'D' };
			sprintf_s(str, sizeof(str), "C86BOX Slot.%c(%d)", slotname[c86->getSlotIndex()], c86->getChipIndex());
			info[i].device_name.assign(str);

			switch (c86->getBoardType() & 0xffff) {
			case CBUS_BOARD_UNKNOWN:
				info[i].board_name.assign("UNKNOWN"); break;
			case CBUS_BOARD_14:
				info[i].board_name.assign("PC-9801-14"); break;
			case CBUS_BOARD_26:
				info[i].board_name.assign("PC-9801-26K"); break;
			case CBUS_BOARD_SOUND_ORCHESTRA:
				info[i].board_name.assign("ORCHESTRA"); break;
			case CBUS_BOARD_SOUND_ORCHESTRA_L:
				info[i].board_name.assign("ORCHESTRA-L"); break;
			case CBUS_BOARD_SOUND_ORCHESTRA_V:
				info[i].board_name.assign("ORCHESTRA-V"); break;
			case CBUS_BOARD_SOUND_ORCHESTRA_VS:
				info[i].board_name.assign("ORCHESTRA-VS"); break;
			case CBUS_BOARD_SOUND_ORCHESTRA_LS:
				info[i].board_name.assign("ORCHESTRA-LS"); break;
			case CBUS_BOARD_SOUND_ORCHESTRA_MATE:
				info[i].board_name.assign("ORCHESTRA-MATE"); break;
			case CBUS_BOARD_MULTIMEDIA_ORCHESTRA:
				info[i].board_name.assign("MMO"); break;
			case CBUS_BOARD_LITTLE_ORCHESTRA:
				info[i].board_name.assign("LITTLE"); break;
			case CBUS_BOARD_LITTLE_ORCHESTRA_L:
				info[i].board_name.assign("LITTLE-L"); break;
			case CBUS_BOARD_LITTLE_ORCHESTRA_RS:
				info[i].board_name.assign("LITTLE-RS"); break;
			case CBUS_BOARD_LITTLE_ORCHESTRA_LS:
				info[i].board_name.assign("LITTLE-LS"); break;
			case CBUS_BOARD_LITTLE_ORCHESTRA_SS:
				info[i].board_name.assign("LITTLE-SS"); break;
			case CBUS_BOARD_LITTLE_ORCHESTRA_MATE:
				info[i].board_name.assign("LITTLE-MATE"); break;
			case CBUS_BOARD_LITTLE_ORCHESTRA_FELLOW:
				info[i].board_name.assign("LITTLE-FELLOW"); break;
			case CBUS_BOARD_JOY2:
				info[i].board_name.assign("JOY2"); break;
			case CBUS_BOARD_SOUND_GRANPRI:
				info[i].board_name.assign("GRANPRI"); break;
			case CBUS_BOARD_TN_F3FM:
				info[i].board_name.assign("TN-F3FM"); break;
			case CBUS_BOARD_73:
				info[i].board_name.assign("PC-9801-73"); break;
			case CBUS_BOARD_86:
				info[i].board_name.assign("PC-9801-86"); break;
			case CBUS_BOARD_ASB01:
				info[i].board_name.assign("ASB-01"); break;
			case CBUS_BOARD_SPEAKBOARD:
				info[i].board_name.assign("SPEAKBOARD"); break;
			case CBUS_BOARD_SOUNDPLAYER98:
				info[i].board_name.assign("SPB98"); break;
			case CBUS_BOARD_SECONDBUS86:
				info[i].board_name.assign("SB86"); break;
			case CBUS_BOARD_SOUNDEDGE:
				info[i].board_name.assign("SOUNDEDGE"); break;
			case CBUS_BOARD_WINDUO:
				info[i].board_name.assign("WINDUO"); break;
			case CBUS_BOARD_OTOMI:
				info[i].board_name.assign("OTOMI"); break;
			case CBUS_BOARD_WAVEMASTER:
				info[i].board_name.assign("WAVEMASTER"); break;
			case CBUS_BOARD_WAVESMIT:
				info[i].board_name.assign("WAVESIMIT"); break;
			case CBUS_BOARD_WAVESTAR:
				info[i].board_name.assign("WAVESTAR"); break;
			case CBUS_BOARD_WSN_A4F:
				info[i].board_name.assign("WSN-A4F"); break;
			case CBUS_BOARD_SB16:
				info[i].board_name.assign("SB16"); break;
			case CBUS_BOARD_SB16_2203:
				info[i].board_name.assign("SB16"); break;
			case CBUS_BOARD_SB16VALUE:
				info[i].board_name.assign("SB16VALUE"); break;
			case CBUS_BOARD_POWERWINDOW_T64S:
				info[i].board_name.assign("PW-T64S"); break;
			case CBUS_BOARD_PCSB2:
				info[i].board_name.assign("PC-SB2"); break;
			case CBUS_BOARD_WGS98S:
				info[i].board_name.assign("WGS98S"); break;
			case CBUS_BOARD_SXM_F:
				info[i].board_name.assign("SXM-F"); break;
			case CBUS_BOARD_SRB_G:
				info[i].board_name.assign("SRB-G"); break;
			case CBUS_BOARD_MIDI_ORCHESTRA_MIDI3:
				info[i].board_name.assign("MIDI-3"); break;
			case CBUS_BOARD_SB_AWE32:
				info[i].board_name.assign("SB-AWE32"); break;
			case CBUS_BOARD_118:
				info[i].board_name.assign("PC-9801-118"); break;
			}
		}
	}

	// frame=19 + topinfo=40 + module=74*N + bottominfo=10
	int windowHeight = 19 + 40 + modHeight * sz + 10;
	int windowWidth = 334;

	if (!CVisWnd::resize(windowWidth, windowHeight))
		return false;

	int y = 40 + modHeight * st;

	for (int i = st; i < sz; i++) {
		Stream* s = GetC86CtlMain()->getStream(i);

		TCHAR key[KEYBUFLEN];
		//Chip *pchip = s->chip;
		hwinfo* pinfo = &info[i];

		_sntprintf_s(key, _countof(key), KEYBUFLEN, INIKEY_REGWND, static_cast<int>(i));
		pinfo->regView = visC86RegViewFactory(s->chip, i);
		pinfo->regView->closeEvent.push_back(
			[pinfo](CVisWnd* h) {
			pinfo->checkReg->setCheck(0, false);
		});

		pinfo->checkReg = CVisCheckBoxPtr(new CVisCheckBox(this, 260, y, "REGISTER"));
		pinfo->checkReg->changeEvent.push_back(
			[this, pinfo, i](CVisWidget* w) {
			TCHAR key[KEYBUFLEN];
			_sntprintf_s(key, _countof(key), KEYBUFLEN, INIKEY_REGWND, static_cast<int>(i));

			if (dynamic_cast<CVisCheckBox*>(w)->getValue()) {
				if (pinfo->regView)
					pinfo->regView->create(hWnd);
				gConfig.writeInt(windowClass.c_str(), key, 1);
			} else {
				if (pinfo->regView)
					pinfo->regView->close();
				gConfig.writeInt(windowClass.c_str(), key, 0);
			}
		});
		widgets.push_back(pinfo->checkReg);
		if (gConfig.getInt(windowClass.c_str(), key, 0)) {
			info[i].checkReg->setCheck(1, true);
		}

		if (info[i].chiptype == CHIP_OPNA || info[i].chiptype == CHIP_YM2608NOADPCM ||
			info[i].chiptype == CHIP_OPN3L ||
			info[i].chiptype == CHIP_OPM || info[i].chiptype == CHIP_TMS3631RI104) {

			_sntprintf_s(key, _countof(key), KEYBUFLEN, INIKEY_KEYWND, i);
			pinfo->keyView = visC86KeyViewFactory(s->chip, i);
			pinfo->keyView->closeEvent.push_back(
				[pinfo](CVisWnd* h) {
				pinfo->checkKey->setCheck(0, false);
			});

			pinfo->checkKey = CVisCheckBoxPtr(new CVisCheckBox(this, 180, y, "KEYBOARD"));
			pinfo->checkKey->changeEvent.push_back(
				[this, pinfo, i](CVisWidget* w) {
				TCHAR key[KEYBUFLEN];
				_sntprintf_s(key, _countof(key), KEYBUFLEN, INIKEY_KEYWND, static_cast<int>(i));

				if (dynamic_cast<CVisCheckBox*>(w)->getValue()) {
					if (pinfo->keyView)
						pinfo->keyView->create(hWnd);
					gConfig.writeInt(windowClass.c_str(), key, 1);
				} else {
					if (pinfo->keyView)
						pinfo->keyView->close();
					gConfig.writeInt(windowClass.c_str(), key, 0);
				}
			});
			widgets.push_back(info[i].checkKey);
			if (gConfig.getInt(windowClass.c_str(), key, 0)) {
				info[i].checkKey->setCheck(1, true);
			}
		}

		if (info[i].chiptype == CHIP_OPNA || info[i].chiptype == CHIP_YM2608NOADPCM ||
			info[i].chiptype == CHIP_OPN3L || info[i].chiptype == CHIP_OPM) {

			int nch = 6;
			if (info[i].chiptype == CHIP_OPM)
				nch = 8;

			for (int ch = 0; ch < nch; ch++) {
				char str[10];
				sprintf_s(str, sizeof(str), "FM%d", ch + 1);

				_sntprintf_s(key, _countof(key), KEYBUFLEN, INIKEY_FMWND, static_cast<int>(i), ch + 1);
				pinfo->fmView[ch] = visC86FmViewFactory(s->chip, i, ch);
				pinfo->fmView[ch]->closeEvent.push_back(
					[pinfo, ch](CVisWnd* h) {
					pinfo->checkFM[ch]->setCheck(0, false);
				});

				pinfo->checkFM[ch] = CVisCheckBoxPtr(new CVisCheckBox(this, 180 + 40 * (ch % 3), y + ((ch / 3) + 1) * 14, str));
				pinfo->checkFM[ch]->changeEvent.push_back(
					[this, pinfo, i, ch](CVisWidget* w) {
					TCHAR key[KEYBUFLEN];
					_sntprintf_s(key, _countof(key), KEYBUFLEN, INIKEY_FMWND, static_cast<int>(i), ch + 1);

					if (dynamic_cast<CVisCheckBox*>(w)->getValue()) {
						if (pinfo->fmView[ch])
							pinfo->fmView[ch]->create(hWnd);
						gConfig.writeInt(windowClass.c_str(), key, 1);
					} else {
						if (pinfo->fmView[ch])
							pinfo->fmView[ch]->close();
						gConfig.writeInt(windowClass.c_str(), key, 0);
					}
				});
				widgets.push_back(info[i].checkFM[ch]);
				if (gConfig.getInt(windowClass.c_str(), key, 0)) {
					info[i].checkFM[ch]->setCheck(1, true);
				}
			}
		}

		/*
		info[i].checkSSG = CVisCheckBoxPtr(new CVisCheckBox(this,180,y+42, "SSG"));
		widgets.push_back(info[i].checkSSG);

		info[i].checkRHYTHM = CVisCheckBoxPtr(new CVisCheckBox(this,220,y+42, "RHYTHM"));
		widgets.push_back(info[i].checkRHYTHM);

		info[i].checkADPCM = CVisCheckBoxPtr(new CVisCheckBox(this,280,y+42, "ADPCM"));
		widgets.push_back(info[i].checkADPCM);
		 */

		y += modHeight;
	}

	return true;
}

bool CVisC86Main::create(HWND parent)
{
	// frame=19 + topinfo=40 + module=74*N + bottominfo=10
	int sz = static_cast<int>(info.size());
	int windowHeight = 19 + 40 + modHeight * sz + 10;
	int windowWidth = 334;

	if (!CVisWnd::create(windowWidth, windowHeight, 0, WS_POPUP | WS_CLIPCHILDREN, NULL)) {
		return false;
	}
	hMaster = parent;

	update();
	::ShowWindow(hWnd, SW_SHOWNOACTIVATE);

	return true;
}

void CVisC86Main::onPaintClient()
{
	// NOTE: devices.size != info.size() となることがあるので注意
	size_t sz = info.size();

	CVisC86Skin* skin = &gVisSkin;
	const int maxlen = 256;
	char str[maxlen];

	int cw = clientCanvas->getWidth();
	int ch = clientCanvas->getHeight();

	// 背景消去
	visFillRect(clientCanvas, 0, 0, cw, ch, ARGB(255, 0, 0, 0));

	// ロゴ・バージョン
	skin->drawLogo(clientCanvas, 2, 2);
	skin->drawStr(clientCanvas, 0, 130, 7, "OPx STATUS DISP FOR GIMIC/C86BOX");
	skin->drawStr(clientCanvas, 0, 130, 17, "(C)HONET " VERSION_MESSAGE);

	// 横線
	int w = clientCanvas->getWidth() - 4;
	UINT* p = (UINT*)clientCanvas->getPtr(2, 28);
	int step = clientCanvas->getStep() / 2;
	int r = 199, g = 200, b = 255;
	for (int x = 0; x < w; x++) {
		int d = tick * 2 + (w - x);
		int a = abs(d % 512 - 255);
		*p = *(p + step) = ARGB(255, (r * a) >> 8, (g * a) >> 8, (b * a) >> 8);
		p++;
	}
	tick++;

	int y = 40;
	for (int i = 0; i < static_cast<int>(sz); i++) {
		int dy = 0;
		// 枠線
		if (modHeight >= 14) {
			visFillRect(clientCanvas, 5, y - 2, 5, (modHeight - 6), skin->getPal(CVisC86Skin::IDCOL_MID));
			visFillRect(clientCanvas, 5, y + (modHeight - 10), 320, 2, skin->getPal(CVisC86Skin::IDCOL_MID));
		}

		BaseSoundModule* module = GetC86CtlMain()->getStream(i)->module;
		BaseSoundDevice* dev = module->getParentDevice();

		if (module->isValid()) {
			// デバイス名
			sprintf_s(str, sizeof(str), "MODULE%d: %s", i, info[i].device_name.c_str());
			skin->drawStr(clientCanvas, 1, 15, y + dy, str); dy += 10;
			// Firmware version
			sprintf_s(str, sizeof(str), "FW-VER : %s", info[i].verstr.c_str());
			skin->drawStr(clientCanvas, 1, 15, y + dy, str); dy += 10;
			// バスボード名
			sprintf_s(str, sizeof(str), "BOARD  : %s", info[i].board_name.c_str());
			skin->drawStr(clientCanvas, 1, 15, y + dy, str); dy += 10;
			// MODULE名
			sprintf_s(str, sizeof(str), "CHIP   : %s", info[i].module_name.c_str());
			skin->drawStr(clientCanvas, 1, 15, y + dy, str); dy += 10;

			GimicWinUSB::GimicModuleWinUSB* gimic = dynamic_cast<GimicWinUSB::GimicModuleWinUSB*>(module);
			if (gimic) {
				const GimicParam* param = gimic->getGimicParam();

				// PLL Clock
				sprintf_s(str, sizeof(str), "CLOCK  : %u Hz", param->clock);
				skin->drawStr(clientCanvas, 1, 15, y + dy, str); dy += 10;
				// SSG-Volume(書くところ足りない)
				//sprintf_s(str, sizeof(str), "SSG-VOL: %d", param->ssgVol );
				//skin->drawStr( clientCanvas, 1, 15, y+dy, str ); dy+=10;
			}

			// speed
			sprintf_s(str, sizeof(str), "SPEED  : %7u CPS", dev->getCPS());
			skin->drawStr(clientCanvas, 1, 15, y + dy, str); dy += 10;
		} else {
			sprintf_s(str, sizeof(str), "MODULE%d: DISCONNECTED", i);
			skin->drawStr(clientCanvas, 1, 15, y + dy, str); dy += 10;
			//skin->drawStr(clientCanvas, 1, 15, y + dy, "FW-VER :"); dy += 10;
			//skin->drawStr(clientCanvas, 1, 15, y + dy, "CHIP   :"); dy += 10;
			//skin->drawStr(clientCanvas, 1, 15, y + dy, "CLOCK  :"); dy += 10;
			//skin->drawStr( clientCanvas, 1, 15, y+dy, "SSG-VOL:" ); dy+=10;
			//skin->drawStr(clientCanvas, 1, 15, y + dy, "SPEED  :"); dy += 10;
		}
		y += modHeight;
	}

	if (sz == 0) {
		skin->drawStr(clientCanvas, 0, 5, ch - 12, "NO DEVICE FOUND.");
	}

	// FPS
	sprintf_s(str, sizeof(str), "FPS: %0.1f", CVisManager::getInstance()->getCurrentFPS());
	skin->drawStr(clientCanvas, 0, 260, ch - 12, str);

}

void CVisC86Main::onMouseEvent(UINT msg, WPARAM wp, LPARAM lp)
{
	POINT point;
	GetCursorPos(&point);

	switch (msg) {
	case WM_RBUTTONUP:
		::SetForegroundWindow(hWnd);

		HMENU hMenu = ::LoadMenu(C86CtlMain::getInstanceHandle(), MAKEINTRESOURCE(IDR_MENU_SYSPOPUP));
		if (!hMenu)
			break;
		HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
		if (!hSubMenu) {
			::DestroyMenu(hMenu);
			break;
		}
		::CheckMenuItem(hMenu, ID_POPUP_SHOWVIS, MF_BYCOMMAND | MFS_CHECKED);

		TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, point.x, point.y, 0, hWnd, NULL);
		::DestroyMenu(hMenu);
	}
}

void CVisC86Main::onCommand(HWND hwnd, DWORD id, DWORD notifyCode)
{
	switch (id) {
	case ID_POPUP_CONFIG:
		C86CtlMainWnd::getInstance()->openConfigDialog();
		break;
	case ID_POPUP_SHOWVIS:
		::PostMessage(hMaster, WM_COMMAND, (notifyCode << 16) | id, (LPARAM)hwnd);
		break;
	}
}
