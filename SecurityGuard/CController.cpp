#include "pch.h"
#include "CController.h"

CController gCtrl;

DWORD CALLBACK WorkerThread(LPVOID lpThreadParameter)
{
	// 获取到传入的参数
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
			swprintf_s(buff, MAX_PATH, L"CPU：%.1f%%，内存：%lu%%",
				fcpu, statex.dwMemoryLoad);

			menu->ModifyMenu(3, MF_BYPOSITION, ID_32777, buff);
			DrawMenuBar(wMain);
			//OutputDebugString(buff);
		}
	}
	HeapFree(GetProcessHeap(), 0, buff);
	OutputDebugString(_T("CPU获取线程结束。\n"));
	return 0x00;
}

void CALLBACK TimerProcMy(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	
}


#pragma region 类内函数
CController::CController()
{

}

CController::~CController()
{
	threadIsRun = FALSE;
	WaitForSingleObject(m_hthread, 3000);
}


/*
	函数：控制器初始化，创建CPU监控线程
	时间：2020-11-03_20-30
*/
void CController::Init(HWND hWnd)
{
	if (!EnableDebugPrivilege()) {
		MessageBoxW(hWnd, L"提升权限失败。", 0, 0);
		ExitProcess(0x01);
		return;
	}
	this->m_wMain = hWnd;
	threadIsRun = TRUE;
	//SetTimer(hWnd, 1, 1000, (TIMERPROC)TimerProcMy);
	DWORD tid;
	HANDLE Thread = CreateThread(
		NULL,			// 线程的安全属性
		NULL,			// 默认栈的大小(局部变量、参数、返回地址)
		WorkerThread,	// 线程代码的起始位置
		hWnd,			// 线程函数的参数，如果传递的是地址，那么需要保证这块内存地址的生命周期（不能是局部变量）
		NULL,			// 创建标志
		&tid);			// 传出的线程ID
	this->m_hthread = Thread;
}

/*
	函数：列表框处理
	作者：CO0kie丶
	时间：20201102-20-05
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
	函数：菜单栏处理
	作者：CO0kie丶
	时间：20201102-20-42
*/
void CController::DoSomeMenu(UINT nID)
{
	switch (nID)
	{
	case ID_32774:		//打开服务
		if (this->_lpServiceInfo)
			if (gView.m_CServices.OpenServiceMy(
				this->_lpServiceInfo)) {
				gView.m_PVList->DeleteAllItems();
				MessageBox(gView.m_Main->GetSafeHwnd(),
					_T("启动成功。"), _T("服务操作成功"), 0);
				gView.DoSomeThingTree(gView.m_Statu.tKind.htTree);
			}
		break;
	case ID_32775:		//关闭服务
		if (this->_lpServiceInfo)
			if (gView.m_CServices.StopService(
				this->_lpServiceInfo)) {
				gView.m_PVList->DeleteAllItems();
				MessageBox(gView.m_Main->GetSafeHwnd(),
					_T("关闭成功。"), _T("服务操作成功"), 0);
				gView.DoSomeThingTree(gView.m_Statu.tKind.htTree);
			}
		break;
	case ID_32776: {		//刷新
	}break;
	case ID_32777: {		//CPU
		if (IDYES == MessageBox(m_wMain, _T("确认要退清理内存吗？"),
			_T("提示"), MB_YESNO | MB_ICONQUESTION)) {
			bool bRet = gView.m_CProcess.EnumProcess(nullptr, false, true);
			MessageBox(m_wMain, bRet ? _T("成功") : _T("失败"), _T("提示"), 0);
		}
	}break;
	default:
		break;
	}
}
#pragma endregion


