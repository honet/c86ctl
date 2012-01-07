
// c86winDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "c86win.h"
#include "c86winDlg.h"
#include "afxdialogex.h"
#include "c86ctl.h"
#include <MMSystem.h>


#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// C86winDlg ダイアログ




C86winDlg::C86winDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(C86winDlg::IDD, pParent)
	, m_ssgVol(255)
	, m_pllClock(8000000)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hThread = NULL;
	threadID = 0;
	terminateFlag = 0;
}

void C86winDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_editFilePath);
	DDX_Control(pDX, IDC_STATIC_TICK, m_staticTick);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_editMessage);
	DDX_Text(pDX, IDC_EDIT_SSGVOL, m_ssgVol);
	DDX_Text(pDX, IDC_EDIT_PLLCLOCK, m_pllClock);
}

BEGIN_MESSAGE_MAP(C86winDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &C86winDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &C86winDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &C86winDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &C86winDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &C86winDlg::OnBnClickedButtonOpen)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_INITIALIZE, &C86winDlg::OnBnClickedButtonInitialize)
	ON_BN_CLICKED(IDC_BUTTON_DEINITIALIZE, &C86winDlg::OnBnClickedButtonDeinitialize)
	ON_BN_CLICKED(IDC_BUTTON_SSGVOL, &C86winDlg::OnBnClickedButtonSetSSGVol)
	ON_BN_CLICKED(IDC_BUTTON_PLLCLOCK, &C86winDlg::OnBnClickedButtonSetPllClock)
	ON_BN_CLICKED(IDC_BUTTON_MBINFO, &C86winDlg::OnBnClickedButtonMbinfo)
	ON_BN_CLICKED(IDC_BUTTON_MODULEINFO, &C86winDlg::OnBnClickedButtonModuleinfo)
	ON_BN_CLICKED(IDC_BUTTON_GET_SSGVOL, &C86winDlg::OnBnClickedButtonGetSsgvol)
	ON_BN_CLICKED(IDC_BUTTON_GET_PLLCLOCK, &C86winDlg::OnBnClickedButtonGetPllclock)
END_MESSAGE_MAP()


// C86winDlg メッセージ ハンドラー

BOOL C86winDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定


	// -----------------------------------------------------------------
	C86winApp *pApp = (C86winApp*)AfxGetApp();
	//pApp->pChipBase->initialize();
	//pApp->pChipBase->deinitialize();
	// -----------------------------------------------------------------
	UpdateData(FALSE);


	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void C86winDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void C86winDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR C86winDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


bool C86winDlg::terminateFlag;

// 演奏処理スレッド
unsigned int WINAPI C86winDlg::PlayerThread(LPVOID param)
{
	C86winDlg *pThis = reinterpret_cast<C86winDlg*>(param);
	DWORD next;
	next = ::timeGetTime()*1000;
	UINT tick = 0;
	UINT idx = 0;
	UINT delay = 0;
	CString str;
	UINT tpus; // tick per micro second
	UINT last_is_adpcm=0;

	IRealChip *pRC = NULL;

	// -----------------------------------------------------------------
	// IRealChip取得
	C86winApp *pApp = (C86winApp*)AfxGetApp();
	int nchip = pApp->pChipBase->getNumberOfChip();
	if( 0<nchip ){
		pApp->pChipBase->getChipInterface( 0, IID_IRealChip, (void**)&pRC );
	}
	pRC->reset();
	// -----------------------------------------------------------------

	tpus = (INT)(pThis->s98data.getTimerPrec() * 1000.0);
	if(tpus==0) tpus = 1;
	while(1){
		if( terminateFlag )
			break;
		
		DWORD now = ::timeGetTime()*1000;
		if(now < next){
			Sleep(1);
			continue;
		}
		if( next+(tpus*10) < now ){ // 転送が遅くて間に合わない場合のスキップ処理
			next = now + tpus;
			delay = 1;
		}else{
			next += tpus;
			delay = 0;
		}

		// update
		tick++;
		pThis->m_tick = tick;
		// ここでCEdit弄るとなんかスレッド終了時に固まる。。。。やっちゃダメなんだっけ？
//		str.Format( _T("%01d, INDEX:%5d, TICK:%5d, NOW:%05d, NEXT:%05d"), delay, idx, tick, now/1000, next/1000 );
//		pThis->m_staticTick.SetWindowText(str);
//		pThis->m_staticTick.UpdateWindow();

		auto prow = &pThis->s98data.row;
		if( idx < prow->size() ){
			auto pr = &prow->at(idx);
			while( pr->gtick <= tick ){
				if( pr->getDeviceNo() == 0 && pr->len == 3 ){
					UINT addr = pr->data[1];
					if( pr->isExtDevice() ) addr += 0x100;
					UCHAR data = pr->data[2];

#if 0
					// 直前の転送がadpcmデータで、今回のデータがadpcmデータでもtimer-A/Bセットでも無い場合に
					// tickをリセット。初音抜け対策でやってみたけどあんまり効果が無かったのでやめ。
					if( last_is_adpcm && addr != 0x108 && !(0x24<=addr && addr<=0x27) ){
						next = now+(tpus*1000);
						last_is_adpcm = 0;
						break;
					}
#endif
					//c86ctl_out(addr, data);
					pRC->out( addr, data );
					last_is_adpcm = ( addr == 0x108 );
				}
				if( ++idx >= prow->size() )
					break;
				pr = &prow->at(idx);
			}
		}

	}

	pRC->reset();
//	c86ctl_reset();
//	c86ctl_deinitialize();

	return 0;
}


void C86winDlg::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnOK();
}


void C86winDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnCancel();
}


void C86winDlg::OnBnClickedButtonPlay()
{
	if( hThread == 0 ){
		terminateFlag = 0;
		hThread = (HANDLE)_beginthreadex( NULL, 0, C86winDlg::PlayerThread, this, 0, &threadID );
		SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
	}
	SetTimer(0, 100, NULL);
}


void C86winDlg::OnBnClickedButtonStop()
{
	if( hThread ){
		terminateFlag = 1;
		WaitForSingleObject( hThread, INFINITE );
		hThread = 0;
		threadID = 0;
	}
	KillTimer(0);
}


void C86winDlg::OnBnClickedButtonOpen()
{
	CFileDialog dlg( TRUE, _T("s98"), NULL, NULL, _T("s98 files (*.s98)|*.s98||"), this );

	if( dlg.DoModal() == IDOK ){
		OnBnClickedButtonStop();
		m_editFilePath.SetWindowText( dlg.GetPathName() );
		s98data.loadFile(dlg.GetPathName());
	}
}


void C86winDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString str;
	//str.Format( _T("%01d, INDEX:%5d, TICK:%5d, NOW:%05d, NEXT:%05d"), delay, idx, tick, now/1000, next/1000 );
	str.Format( _T("%d"), m_tick );
	m_staticTick.SetWindowText(str);
	m_staticTick.UpdateWindow();

	CDialogEx::OnTimer(nIDEvent);
}


void C86winDlg::OnBnClickedButtonInitialize()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	C86winApp *pApp = (C86winApp*)AfxGetApp();
	pApp->pChipBase->initialize();

}


void C86winDlg::OnBnClickedButtonDeinitialize()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	C86winApp *pApp = (C86winApp*)AfxGetApp();
	pApp->pChipBase->deinitialize();
}


void C86winDlg::OnBnClickedButtonSetSSGVol()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData();
	C86winApp *pApp = (C86winApp*)AfxGetApp();
	IGimic *pGimicModule;
	if( S_OK == pApp->pChipBase->getChipInterface( 0, IID_IGimic, (void**)&pGimicModule ) ){
		pGimicModule->setSSGVolume(m_ssgVol);
		pGimicModule->Release();
	}
}


void C86winDlg::OnBnClickedButtonSetPllClock()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData();
	C86winApp *pApp = (C86winApp*)AfxGetApp();
	IGimic *pGimicModule;
	if( S_OK == pApp->pChipBase->getChipInterface( 0, IID_IGimic, (void**)&pGimicModule ) ){
		pGimicModule->setPLLClock(m_pllClock);
		pGimicModule->Release();
	}
}


void C86winDlg::OnBnClickedButtonMbinfo()
{
	UpdateData();
	C86winApp *pApp = (C86winApp*)AfxGetApp();
	IGimic *pGimicModule;
	if( S_OK == pApp->pChipBase->getChipInterface( 0, IID_IGimic, (void**)&pGimicModule ) ){
		struct Devinfo info;
		pGimicModule->getMBInfo(&info);

		CString str, devname, rev, serial;
		devname = info.Devname;
		rev = info.Rev;
		serial = info.Serial;
		str = devname + _T("\r\n") + rev + _T("\r\n") + serial;
		m_editMessage.SetWindowText(str);
		pGimicModule->Release();
	}
}


void C86winDlg::OnBnClickedButtonModuleinfo()
{
	UpdateData();
	C86winApp *pApp = (C86winApp*)AfxGetApp();
	IGimic *pGimicModule;
	if( S_OK == pApp->pChipBase->getChipInterface( 0, IID_IGimic, (void**)&pGimicModule ) ){
		struct Devinfo info;
		pGimicModule->getModuleInfo(&info);

		CString str, devname, rev, serial;
		devname = info.Devname;
		rev = info.Rev;
		serial = info.Serial;
		str = devname + _T("\r\n") + rev + _T("\r\n") + serial;
		m_editMessage.SetWindowText(str);
		pGimicModule->Release();
	}
}


void C86winDlg::OnBnClickedButtonGetSsgvol()
{
	C86winApp *pApp = (C86winApp*)AfxGetApp();
	IGimic *pGimicModule;
	if( S_OK == pApp->pChipBase->getChipInterface( 0, IID_IGimic, (void**)&pGimicModule ) ){
		UCHAR vol;
		pGimicModule->getSSGVolume(&vol);
		m_ssgVol = vol;
		UpdateData(FALSE);
		pGimicModule->Release();
	}
}


void C86winDlg::OnBnClickedButtonGetPllclock()
{
	C86winApp *pApp = (C86winApp*)AfxGetApp();
	IGimic *pGimicModule;
	if( S_OK == pApp->pChipBase->getChipInterface( 0, IID_IGimic, (void**)&pGimicModule ) ){
		UINT clock;
		pGimicModule->getPLLClock(&clock);
		m_pllClock = clock;
		UpdateData(FALSE);
		pGimicModule->Release();
	}
}
