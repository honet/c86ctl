/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */

#include <vector>
//#include <WinUser.h>
#include <Dbt.h>
#include <ShellAPI.h>

#include "c86ctl.h"
#include "interface/if.h"



namespace c86ctl{

class C86CtlMain : public IRealChipBase
{
public:
	C86CtlMain() : 
	    isInitialized(false),
		hMainThread(0),
		mainThreadID(0),
		hSenderThread(0),
		senderThreadID(0),
		timerPeriod(0),
		terminateFlag(false),
		refCount(0)
	{
	};
	~C86CtlMain(){};

public:
	// IUnknown
	virtual HRESULT __stdcall QueryInterface( REFIID riid, LPVOID *ppvObj );
	virtual ULONG __stdcall AddRef(VOID);
	virtual ULONG __stdcall Release(VOID);

public:
	// IRealChipBase
	virtual int __stdcall initialize(void);
	virtual int __stdcall deinitialize(void);
	virtual int __stdcall getNumberOfChip(void);
	virtual HRESULT __stdcall getChipInterface( int id, REFIID riid, void** ppi );

public:
	int reset(void);
	void out( UINT addr, UCHAR data );
	UCHAR in( UINT addr );

public:
	static INT init(HINSTANCE h);
	static INT deinit(void);
	static HINSTANCE getInstanceHandle();
	
private:
	bool terminateFlag;
	std::vector< std::shared_ptr<GimicIF> > gGIMIC;

public:
	std::vector< std::shared_ptr<GimicIF> > &getGimics();
	
private:
	static unsigned int WINAPI threadMain(LPVOID param);
	static unsigned int WINAPI threadSender(LPVOID param);

private:
	HANDLE hMainThread;
	UINT mainThreadID;
	HANDLE hSenderThread;
	UINT senderThreadID;
	
	static HINSTANCE hInstance;
	static ULONG_PTR gdiToken;
	static Gdiplus::GdiplusStartupInput gdiInput;
	
	DWORD timerPeriod;
	bool isInitialized;

private:
	UINT refCount;
};

C86CtlMain* GetC86CtlMain(void);

};

