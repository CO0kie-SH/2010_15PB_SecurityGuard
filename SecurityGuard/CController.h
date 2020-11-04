#pragma once
#include "CMyView.h"
#include "CMD5Checksum.h"


static BOOL threadIsRun = FALSE;
DWORD CALLBACK WorkerThread(LPVOID lpThreadParameter);
void CALLBACK TimerProcMy(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);


class CController
{
public:
	CController();
	~CController();
	void Init(HWND hWnd);
	void DoSomeThingList(LPNMITEMACTIVATE& pNMItem);
	void DoSomeMenu(UINT nID);
	void DoSomeTreeRight(HTREEITEM& hTree, CPoint& point);
private:
	LPSERVICEINFO	_lpServiceInfo = nullptr;
	HANDLE			m_hthread = 0;
	HWND			m_wMain = 0;
};

extern CController gCtrl;