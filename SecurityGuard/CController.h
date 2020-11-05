#pragma once
#include "CMyView.h"
#include "CMD5Checksum.h"


static BOOL threadIsRun = FALSE;
static BOOL threadIsRunSearch = FALSE;

DWORD CALLBACK WorkerThread(LPVOID lpThreadParameter);
DWORD CALLBACK WorkerThread2(LPVOID lpThreadParameter);
DWORD CALLBACK WorkerThread4(LPVOID lpThreadParameter);
void CALLBACK TimerProcMy(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);

static __int64 iDirNum = 0, iFileNum = 0, iSize = 0;

class CController
{
public:
	CController();
	~CController();
	void Init(HWND hWnd);
	void DoSomeThingList(LPNMITEMACTIVATE& pNMItem);
	void DoSomeMenu(UINT nID);
	DWORD CALLBACK WorkerThread3(LPVOID lpThreadParameter);
	static void FileCallBack(LPFILEINFO pFileInfo);
	void DoSomeTreeRight(HTREEITEM& hTree, CPoint& point);
	void DoSomeEdit();
	BOOL CheckFileMd5(char pMd5[33]);

private:
	LPSERVICEINFO	_lpServiceInfo = nullptr;
	HANDLE			m_hthread = 0;
	HWND			m_wMain = 0;
};

extern CController gCtrl;