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

			menu->ModifyMenu(5, MF_BYPOSITION, ID_32777, buff);
			DrawMenuBar(wMain);
			//OutputDebugString(buff);
		}
	}
	HeapFree(GetProcessHeap(), 0, buff);
	OutputDebugString(_T("CPU获取线程结束。\n"));
	return 0x00;
}

DWORD CALLBACK WorkerThread2(LPVOID lpThreadParameter)
{
	if (threadIsRunSearch)	return threadIsRunSearch;
	threadIsRunSearch = 2;
	LPCallEnumFilePaths fCall = (LPCallEnumFilePaths)lpThreadParameter;
	TCHAR buff[MAX_PATH];
	_tcsnccpy_s(buff, MAX_PATH, fCall->Path, MAX_PATH);
	FILECALLBACK CallAddress = fCall->CallAddress;
	CHeap cHeap;
	iDirNum = 0; iFileNum = 0; iSize = 0;
	EnumFilePaths(CallAddress, buff, &cHeap);
	threadIsRunSearch = 0;
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
	UnregisterHotKey(this->m_wMain, 1234);
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
	RegisterHotKey(hWnd, 1234, MOD_CONTROL | MOD_ALT, 'Q');

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
		CMenu* pSubMenu1 = cMenu.GetSubMenu(3);
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
	case ID_32776: {		//刷新->退出
		if (threadIsRunSearch) {
			threadIsRunSearch = 0;
			gView.m_PVMenu->ModifyMenu(5, MF_BYPOSITION, ID_32776, _T("退出程序"));
			DrawMenuBar(m_wMain);
			return;
		}
		threadIsRun = false;
		SendMessageW(this->m_wMain, 16, 0, 0);
		//CString strRet, strPath;
		//strPath = L"D:\\15pb\\del.bat";
		//strRet = CMD5Checksum::GetMD5(strPath);
		//OutputDebugString(strPath);
		//OutputDebugString(strRet);
	}break;
	case ID_32777: {		//CPU
		if (IDYES == MessageBox(m_wMain, _T("确认要退清理内存吗？"),
			_T("提示"), MB_YESNO | MB_ICONQUESTION)) {
			bool bRet = gView.m_CProcess.EnumProcess(nullptr, false, true);
			MessageBox(m_wMain, bRet ? _T("成功") : _T("失败"), _T("提示"), 0);
		}
	}break;
	case ID_32784: {		//清理缓存垃圾
		CString str = L"ABC";
		str = CMD5Checksum::GetMD5OfString(str);
	}break;
	case ID_32785: {		//清理VS垃圾
		if (!PathFileExists(gView.m_str))	return;		//如果文件不存在则返回
		if (!PathIsDirectory(gView.m_str))	return;		//如果目录不存在则返回

		CString strCMD = _T("/k title \"请确认以下路径，然后按任意键删除。\" &&");
		strCMD += _T("echo \"请确认以下路径，然后按任意键删除。\"&&echo %cd%\\&& pause &&");
		strCMD += _T("del /s  *.ilk *.pdb *.obj *.log *.pch *.tlog");
		strCMD += _T(" *.lastbuildstate *.sdf *.idb *.ipch *.res");
		strCMD += _T(" *.o *.lst *.knl *.img *.bin *.db");/* *.exe");*/
		//strCMD += _T("&& pause");
		ShellExecute(m_wMain,
			_T("open"), _T("C:\\Windows\\System32\\cmd.exe"),
			strCMD, gView.m_str, SW_SHOW);
	}break;
	case ID_32786: {		//本地杀毒
		DWORD tid = 0;
		CallEnumFilePaths tmpCall;
		_tcscpy_s(tmpCall.Path, MAX_PATH, gView.m_str.GetBuffer());
		tmpCall.CallAddress = FileCallBack;

		gView.InitList(gView.m_Statu.tKind, true);
		gView.m_PVList->InsertColumn(3, L"父路径", 0, 99);
		gView.m_PVList->InsertColumn(4, L"MD5", 0, 266);
		gView.m_PVList->DeleteAllItems();
		gView.m_PVEdit->SetWindowTextW(gView.m_str);


		CHeap cheap;
		iDirNum = 0; iFileNum = 0; iSize = 0;
		EnumFilePaths(FileCallBack, gView.m_str.GetBuffer(), &cheap);
		WorkerThread3(gView.m_PVList);


		//HANDLE Thread = CreateThread(
		//	NULL,			// 线程的安全属性
		//	NULL,			// 默认栈的大小(局部变量、参数、返回地址)
		//	WorkerThread2,	// 线程代码的起始位置
		//	&tmpCall,		// 线程函数的参数，如果传递的是地址，那么需要保证这块内存地址的生命周期（不能是局部变量）
		//	NULL,			// 创建标志
		//	&tid);			// 传出的线程ID


	}break;
	case ID_32787: {		//云端杀毒
		DWORD tid = 0;
		CallEnumFilePaths tmpCall;
		_tcscpy_s(tmpCall.Path, MAX_PATH, gView.m_str.GetBuffer());
		tmpCall.CallAddress = FileCallBack;

		gView.InitList(gView.m_Statu.tKind, true);
		gView.m_PVList->InsertColumn(3, L"父路径", 0, 99);
		gView.m_PVList->InsertColumn(4, L"MD5", 0, 266);
		gView.m_PVList->DeleteAllItems();
		gView.m_PVEdit->SetWindowTextW(gView.m_str);

		HANDLE Thread = CreateThread(
			NULL,			// 线程的安全属性
			NULL,			// 默认栈的大小(局部变量、参数、返回地址)
			WorkerThread2,	// 线程代码的起始位置
			&tmpCall,		// 线程函数的参数，如果传递的是地址，那么需要保证这块内存地址的生命周期（不能是局部变量）
			NULL,			// 创建标志
			&tid);			// 传出的线程ID
		Sleep(1000);
	}break;
	default:
		break;
	}
}
/*
	函数：多线程查杀
*/
DWORD CController::WorkerThread3(LPVOID lpThreadParameter)
{
	if (threadIsRunSearch)	return threadIsRunSearch;
	threadIsRunSearch = 1;


	CListCtrl* CList = (CListCtrl*)lpThreadParameter;
	gView.m_PVMenu->ModifyMenu(5, MF_BYPOSITION, ID_32776, _T("退出程序"));
	DrawMenuBar(m_wMain);


	int max = CList->GetItemCount(), i;
	CString str;
	CFile file;
	CMD5Checksum* MD5Checksum = nullptr;
	int nLength = 0;       //number of bytes read from the file
	const int nBufferSize = 1024; //checksum the file in blocks of 1024 bytes
	BYTE Buffer[nBufferSize];   //buffer for data read from the file
	for (i = 0; i < max; i++)
	{
		str = CList->GetItemText(i, 3);
		str += CList->GetItemText(i, 2);

		if (file.Open(str, CFile::modeRead) == 0)
			break;
 
		MD5Checksum = new CMD5Checksum();
		//checksum the file in blocks of 1024 bytes
		while ((nLength = file.Read(Buffer, nBufferSize)) > 0)
		{
			MD5Checksum->Update(Buffer, nLength);
		}

		file.Close();
		CList->SetItemText(i, 4, MD5Checksum->Final());
		delete MD5Checksum;	MD5Checksum = nullptr;

	}
	this->DoSomeMenu(ID_32776);
	return 0;
}


/*
	函数：寻找文件回调
*/
void CController::FileCallBack(LPFILEINFO pFileInfo)
{
	bool& isDir = pFileInfo->isDir;
	if (isDir)
		++iDirNum;
	else {
		++iFileNum;
		//iSize += pFileInfo->Size;
		PTCHAR pName = pFileInfo->data.cFileName;
		PTCHAR pKind = PathFindExtension(pName);
		CListCtrl* CList = gView.m_PVList;
		CString str;
		str.Format(L"%03llu", iFileNum);
		int idx = (int)iFileNum - 1;
		CList->InsertItem(idx, str);


		str = pFileInfo->data.cFileName;
		CList->SetItemText(idx, 2, str);



		str = pFileInfo->path;
		CList->SetItemText(idx, 3, str);

		StrFormatByteSizeW(pFileInfo->Size, str.GetBuffer(), MAX_PATH);
		CList->SetItemText(idx, 1, str);


		if (0== lstrcmp(_T("20200923_脱口秀大会第三季.mp4"), pFileInfo->data.cFileName))
			CList->SetItemText(idx, 5, _T("病毒文件！！！！"));

		//if (threadIsRunSearch == 2) {
		//	str = pFileInfo->path;
		//	str += pFileInfo->data.cFileName;
		//	if (pFileInfo->Size < 49271280 * 2) {
		//		str = CMD5Checksum::GetMD5(str);
		//		CList->SetItemText(idx, 4, str);
		//	}
		//	else
		//	{
		//		char buff[33];
		//		GetMd5_ByCertutil(str.GetBuffer(), buff);
		//	}
		//}
	}
}


void CController::DoSomeTreeRight(HTREEITEM& hTree, CPoint& point)
{
	MyTreeInfo& tInfo = gView.m_tLeafs[hTree];
	if (!tInfo.hrTree)	return;
	if (tInfo.hrTree == gView.m_tRoot->fFile.htTree) {
		CString strPath;	UINT deep = tInfo.uiDeep;
		gView.m_str = _T("");
		gView.GetTreePath(hTree, deep, gView.m_str);
		//gView.m_PVList->ClientToScreen(&point);

		CMenu cMenu;
		cMenu.LoadMenuW(IDR_MENU1);
		CMenu* pSubMenu1 = cMenu.GetSubMenu(2);

		pSubMenu1->AppendMenu(MF_GRAYED, MF_POPUP,
			gView.m_str);
		pSubMenu1->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, gView.m_Main);
	}
}
BOOL CController::GetCFileMd5(CString& str)
{

	return 0;
}
#pragma endregion


