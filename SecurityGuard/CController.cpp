#include "pch.h"
#include "CController.h"

CController gCtrl;

DWORD CALLBACK WorkerThread(LPVOID lpThreadParameter)
{
	// ��ȡ������Ĳ���
	HWND wMain = (HWND)lpThreadParameter;
	CMenu* menu = (CMenu*)gView.GetPVMenu();
	PTCHAR buff = (PTCHAR)HeapAlloc(GetProcessHeap(),
		0, sizeof(TCHAR) * MAX_PATH);
	if (!buff)	return 0x01;
	float fcpu = 0;
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	while (threadIsRun)
	{
		Sleep(0);
		fcpu = GetCpuUsage();
		if (!threadIsRun)	break;
		if (fcpu && ::GlobalMemoryStatusEx(&statex)) {
			swprintf_s(buff, MAX_PATH, L"CPU��%.1f%%���ڴ棺%lu%%",
				fcpu, statex.dwMemoryLoad);

			menu->ModifyMenu(3, MF_BYPOSITION, ID_32777, buff);
			DrawMenuBar(wMain);
			//OutputDebugString(buff);
		}
	}
	HeapFree(GetProcessHeap(), 0, buff);
	OutputDebugString(_T("CPU��ȡ�߳̽�����\n"));
	return 0x00;
}

void CALLBACK TimerProcMy(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	
}


#pragma region ���ں���
CController::CController()
{

}

CController::~CController()
{
	threadIsRun = FALSE;
	WaitForSingleObject(m_hthread, 3000);
}


/*
	��������������ʼ��������CPU����߳�
	ʱ�䣺2020-11-03_20-30
*/
void CController::Init(HWND hWnd)
{
	if (!EnableDebugPrivilege()) {
		MessageBoxW(hWnd, L"����Ȩ��ʧ�ܡ�", 0, 0);
		ExitProcess(0x01);
		return;
	}
	this->m_wMain = hWnd;
	threadIsRun = TRUE;
	//SetTimer(hWnd, 1, 1000, (TIMERPROC)TimerProcMy);
	DWORD tid;
	HANDLE Thread = CreateThread(
		NULL,			// �̵߳İ�ȫ����
		NULL,			// Ĭ��ջ�Ĵ�С(�ֲ����������������ص�ַ)
		WorkerThread,	// �̴߳������ʼλ��
		hWnd,			// �̺߳����Ĳ�����������ݵ��ǵ�ַ����ô��Ҫ��֤����ڴ��ַ���������ڣ������Ǿֲ�������
		NULL,			// ������־
		&tid);			// �������߳�ID
	this->m_hthread = Thread;
}

/*
	�������б����
	���ߣ�CO0kieؼ
	ʱ�䣺20201102-20-05
*/
void CController::DoSomeThingList(LPNMITEMACTIVATE& pNMItem)
{
	if (gView.m_Statu.tKind.hrTree == gView.m_tRoot->fService.htTree) {
		POINT xy = { pNMItem->ptAction.x, pNMItem->ptAction.y };
		gView.m_PVList->ClientToScreen(&xy);

		CMenu cMenu;
		cMenu.LoadMenuW(IDR_MENU1);
		CMenu* pSubMenu1 = cMenu.GetSubMenu(2);
		_lpServiceInfo = &gView.m_serviceInfos[pNMItem->iItem];
		pSubMenu1->AppendMenu(MF_GRAYED, MF_POPUP,
			_lpServiceInfo->lpDisplayName);
		pSubMenu1->TrackPopupMenu(TPM_LEFTALIGN, xy.x, xy.y, gView.m_Main);
		
	}
}


/*
	�������˵�������
	���ߣ�CO0kieؼ
	ʱ�䣺20201102-20-42
*/
void CController::DoSomeMenu(UINT nID)
{
	switch (nID)
	{
	case ID_32774:		//�򿪷���
		if (this->_lpServiceInfo)
			if (gView.m_CServices.OpenServiceMy(
				this->_lpServiceInfo)) {
				gView.m_PVList->DeleteAllItems();
				MessageBox(gView.m_Main->GetSafeHwnd(),
					_T("�����ɹ���"), _T("��������ɹ�"), 0);
				gView.DoSomeThingTree(gView.m_Statu.tKind.htTree);
			}
		break;
	case ID_32775:		//�رշ���
		if (this->_lpServiceInfo)
			if (gView.m_CServices.StopService(
				this->_lpServiceInfo)) {
				gView.m_PVList->DeleteAllItems();
				MessageBox(gView.m_Main->GetSafeHwnd(),
					_T("�رճɹ���"), _T("��������ɹ�"), 0);
				gView.DoSomeThingTree(gView.m_Statu.tKind.htTree);
			}
		break;
	case ID_32776: {		//ˢ��
	}break;
	case ID_32777: {		//CPU
		if (IDYES == MessageBox(m_wMain, _T("ȷ��Ҫ�������ڴ���"),
			_T("��ʾ"), MB_YESNO | MB_ICONQUESTION)) {
			bool bRet = gView.m_CProcess.EnumProcess(nullptr, false, true);
			MessageBox(m_wMain, bRet ? _T("�ɹ�") : _T("ʧ��"), _T("��ʾ"), 0);
		}
	}break;
	default:
		break;
	}
}
#pragma endregion


