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

			menu->ModifyMenu(6, MF_BYPOSITION, ID_32777, buff);
			DrawMenuBar(wMain);
			//OutputDebugString(buff);
		}
	}
	HeapFree(GetProcessHeap(), 0, buff);
	OutputDebugString(_T("CPU��ȡ�߳̽�����\n"));
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

DWORD WorkerThread4(LPVOID lpThreadParameter)
{
	ULONG_PTR tIndex = (ULONG_PTR)lpThreadParameter;				//�������
	CListCtrl* CList = (CListCtrl*)gView.GetPVList();				//ͨ����ͼ�õ��б��
	int max = CList->GetItemCount(), i, tMax = threadIsRunSearch;	//��ʼ��ѭ������
	CString str;	CFile cFile;						//�����ַ������ļ�
	TCHAR tBuff[MAX_PATH];	char pMd5[33];				//���建����
	for (i = 0; i < max; i++)
	{
		if (threadIsRunSearch == 0)	return 0x01;	//��������̱߳�ʶ��Ϊ0���˳��߳�
		if (i % 2 != (int)tIndex)					//ȡ�߳���Ϊֵ��������
		{											//���������Ϊ����Ĳ���id
			continue;								//��ѭ��β
		}
		str = CList->GetItemText(i, 3);				//��ȡ�ļ���·��
		str += CList->GetItemText(i, 2);			//��ȡ�ļ���
		if (cFile.Open(str, CFile::modeRead) == 0)		//�����ʧ����ѭ��β
			continue;
		
		if (cFile.GetLength() > 0 &&						//����ļ�����>0
			CMD5Checksum::GetMd5(cFile, pMd5)	&&			//Md5������ļ�Md5
			mbstowcs_s(nullptr, tBuff, pMd5, 33) == 0) {		//char*תCStringW
			if (gCtrl.CheckFileMd5(pMd5)) {						//ͨ��������������
				CList->SetItemText(i, 5, _T("���ǲ���������"));	//��ȡ�ļ��Ƿ�Ϊ����
			}
			CList->SetItemText(i, 4, tBuff);
		}
		cFile.Close();
	}
	//gCtrl.DoSomeMenu(ID_32776);
	if (threadIsRunSearch)
		--threadIsRunSearch;
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
	UnregisterHotKey(this->m_wMain, 1234);
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
	RegisterHotKey(hWnd, 1234, MOD_CONTROL | MOD_ALT, 'Q');

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
		CMenu* pSubMenu1 = cMenu.GetSubMenu(3);
		_lpServiceInfo = &gView.m_serviceInfos[pNMItem->iItem];
		pSubMenu1->AppendMenu(MF_GRAYED, MF_POPUP,
			_lpServiceInfo->lpDisplayName);
		pSubMenu1->TrackPopupMenu(TPM_LEFTALIGN, xy.x, xy.y, gView.m_Main);
		
	}	//IF END��������
	else if (gView.m_Statu.tKind.hrTree == gView.m_tRoot->fFile.htTree) {
		gView.m_str = gView.m_PVList->GetItemText(pNMItem->iItem, 2);
		CString str = PathFindExtensionW(gView.m_str);
		str = str.MakeLower();
		if (0 == _tcscmp(str, _T(".dll")) ||
			0 == _tcscmp(str, _T(".exe")))
		{
			POINT xy = { pNMItem->ptAction.x, pNMItem->ptAction.y };
			gView.m_PVList->ClientToScreen(&xy);
			CMenu cMenu;
			cMenu.LoadMenuW(IDR_MENU1);
			CMenu* pSubMenu1 = cMenu.GetSubMenu(2);
			//_lpServiceInfo = &gView.m_serviceInfos[pNMItem->iItem];
			pSubMenu1->AppendMenu(MF_GRAYED, MF_POPUP, gView.m_str);
			gView.m_Statu.bForm = 2;
			pSubMenu1->TrackPopupMenu(TPM_LEFTALIGN, xy.x, xy.y, gView.m_Main);
		}
	}	//IF END��PE����
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
	case ID_32776: {		//ˢ��->�˳�
		if (threadIsRunSearch) {
			threadIsRunSearch = 0;
			gView.m_PVMenu->ModifyMenu(5, MF_BYPOSITION, ID_32776, _T("�˳�����"));
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
		if (IDYES == MessageBox(m_wMain, _T("ȷ��Ҫ�������ڴ���"),
			_T("��ʾ"), MB_YESNO | MB_ICONQUESTION)) {
			bool bRet = gView.m_CProcess.EnumProcess(nullptr, false, true);
			MessageBox(m_wMain, bRet ? _T("�ɹ�") : _T("ʧ��"), _T("��ʾ"), 0);
		}
	}break;
	case ID_32784: {		//����PE������
		if (gView.m_Statu.bForm == 2) {
			CTreeCtrl* pTree = gView.m_PVTree;
			CString str;
			gView.m_PVEdit->GetWindowTextW(str);
			str += gView.m_str;
			if (gView.m_CPE.Init(str.GetBuffer()))		//��ʼ��PE��
			{
				gView.m_PVEdit->SetWindowTextW(str);
				HTREEITEM tPE = gView.m_tRoot->fPE.htTree;
				pTree->Expand(gView.m_tRoot->fFile.htTree, TVE_COLLAPSE);
				pTree->Expand(tPE, TVE_EXPAND);
				tPE = pTree->GetNextItem(tPE, TVGN_CHILD);
				pTree->SelectItem(tPE);
				MyTreeInfo& tInfo =
					gView.m_tLeafs[tPE];
				gView.InitList(tInfo);
				gView.DoSomeThingTree(tPE);
			}
		}
		gView.m_Statu.bForm = 0;
	}break;
	case ID_32785: {		//����VS����
		if (!PathFileExists(gView.m_str))	return;		//����ļ��������򷵻�
		if (!PathIsDirectory(gView.m_str))	return;		//���Ŀ¼�������򷵻�

		CString strCMD = _T("/k title \"��ȷ������·����Ȼ�������ɾ����\" &&");
		strCMD += _T("echo \"��ȷ������·����Ȼ�������ɾ����\"&&echo %cd%\\&& pause &&");
		strCMD += _T("del /s  *.ilk *.pdb *.obj *.log *.pch *.tlog");
		strCMD += _T(" *.lastbuildstate *.sdf *.idb *.ipch *.res");
		strCMD += _T(" *.o *.lst *.knl *.img *.bin *.db");/* *.exe");*/
		//strCMD += _T("&& pause");
		ShellExecute(m_wMain,
			_T("open"), _T("C:\\Windows\\System32\\cmd.exe"),
			strCMD, gView.m_str, SW_SHOW);
	}break;
	case ID_32786: {		//����ɱ��
		DWORD tid = 0;
		CallEnumFilePaths tmpCall;
		_tcscpy_s(tmpCall.Path, MAX_PATH, gView.m_str.GetBuffer());
		tmpCall.CallAddress = FileCallBack;

		gView.InitList(gView.m_Statu.tKind, true);
		gView.m_PVList->InsertColumn(3, L"��·��", 0, 130);
		gView.m_PVList->InsertColumn(4, L"MD5", 0, 400);
		gView.m_PVList->DeleteAllItems();
		gView.m_PVEdit->SetWindowTextW(gView.m_str);


		CHeap cheap;
		iDirNum = 0; iFileNum = 0; iSize = 0;
		EnumFilePaths(FileCallBack, gView.m_str.GetBuffer(), &cheap);
		//WorkerThread3(gView.m_PVList);

		gView.m_PVMenu->ModifyMenu(5, MF_BYPOSITION, ID_32776, _T("ֹͣɨ��"));
		DrawMenuBar(m_wMain);
		threadIsRunSearch = 2;
		ULONG_PTR num = 0;
		HANDLE Thread = CreateThread(
			NULL,			// �̵߳İ�ȫ����
			NULL,			// Ĭ��ջ�Ĵ�С(�ֲ����������������ص�ַ)
			WorkerThread4,	// �̴߳������ʼλ��
			(LPVOID)num++,	// �̺߳����Ĳ�����������ݵ��ǵ�ַ����ô��Ҫ��֤����ڴ��ַ���������ڣ������Ǿֲ�������
			NULL,			// ������־
			&tid);			// �������߳�ID
		Thread = CreateThread(
			NULL,			// �̵߳İ�ȫ����
			NULL,			// Ĭ��ջ�Ĵ�С(�ֲ����������������ص�ַ)
			WorkerThread4,	// �̴߳������ʼλ��
			(LPVOID)num++,	// �̺߳����Ĳ�����������ݵ��ǵ�ַ����ô��Ҫ��֤����ڴ��ַ���������ڣ������Ǿֲ�������
			NULL,			// ������־
			&tid);			// �������߳�ID


	}break;
	case ID_32787: {		//�ƶ�ɱ��
		DWORD tid = 0;
		CallEnumFilePaths tmpCall;
		_tcscpy_s(tmpCall.Path, MAX_PATH, gView.m_str.GetBuffer());
		tmpCall.CallAddress = FileCallBack;

		gView.InitList(gView.m_Statu.tKind, true);
		gView.m_PVList->InsertColumn(3, L"��·��", 0, 99);
		gView.m_PVList->InsertColumn(4, L"MD5", 0, 266);
		gView.m_PVList->DeleteAllItems();
		gView.m_PVEdit->SetWindowTextW(gView.m_str);

		HANDLE Thread = CreateThread(
			NULL,			// �̵߳İ�ȫ����
			NULL,			// Ĭ��ջ�Ĵ�С(�ֲ����������������ص�ַ)
			WorkerThread4,	// �̴߳������ʼλ��
			&tmpCall,		// �̺߳����Ĳ�����������ݵ��ǵ�ַ����ô��Ҫ��֤����ڴ��ַ���������ڣ������Ǿֲ�������
			NULL,			// ������־
			&tid);			// �������߳�ID
		Sleep(10);
	}break;
	case ID_32790: {		//��������
		CString str;
		vector<PROCESSINFO> proInfos;
		if (gView.m_CProcess.EnumProcess(&proInfos)) {
			for (const auto& val : proInfos)
			{
				if (/*val.is32 == 64 &&*/
					0==_tcscmp(_T("peTest.exe"), val.name))
				{
					HMODULE module = GetModuleHandle(0);
					TCHAR buf[MAX_PATH];
					GetModuleFileName(module, buf, MAX_PATH);
					PathRemoveFileSpec(buf);
					//str.Format(_T("%s\\DLL_HOOK64.dll"));
					wcscat_s(buf, MAX_PATH, _T("\\DLL_HOOK64.dll"));
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, val.tPID);
					API_LoadDll(hProcess, buf);
					CloseHandle(hProcess);
					break;
				}
			}
		}

	}break;
	case ID_32791: {		//����
		if (MessageBoxW(m_wMain, L"ȷ��Ҫ�����𣿣�", L"���棡��",
			MB_YESNO | MB_ICONWARNING) == IDYES) {
			system("start shutdown -r -t 61");
			system("cmd /k echo if will cancel,do \'shutdown -a\'");
		}
	}break;
	case ID_32792: {		//�ػ�
		if (MessageBoxW(m_wMain, L"ȷ��Ҫ�ػ��𣿣�", L"���棡��",
			MB_YESNO | MB_ICONWARNING) == IDYES) {
			system("start shutdown -s -t 61");
			system("cmd /k echo if will cancel,do \'shutdown -a\'");
		}
	}break;
	case ID_32793: {		//ע��
		if (MessageBoxW(m_wMain,L"ȷ��Ҫע���𣿣�", L"���棡��",
			MB_YESNO | MB_ICONWARNING) == IDYES) {
			system("start shutdown /l");
		}
	}break;
	default:
		break;
	}
}
/*
	���������̲߳�ɱ
*/
DWORD CController::WorkerThread3(LPVOID lpThreadParameter)
{
	if (threadIsRunSearch)	return threadIsRunSearch;
	threadIsRunSearch = 1;


	CListCtrl* CList = (CListCtrl*)lpThreadParameter;
	gView.m_PVMenu->ModifyMenu(5, MF_BYPOSITION, ID_32776, _T("�˳�����"));
	DrawMenuBar(m_wMain);


	int max = CList->GetItemCount(), i;
	CString str;	CFile cFile;
	CMD5Checksum cMd5;	char pMd5[33];
	TCHAR buff[MAX_PATH];
	for (i = 0; i < max; i++)
	{
		str = CList->GetItemText(i, 3);
		str += CList->GetItemText(i, 2);
		if (cFile.Open(str, CFile::modeRead) == 0)
			continue;
		
		if (cFile.GetLength() > 0 &&
			cMd5.GetMd5(cFile, pMd5/*, &cMd5*/) &&
			mbstowcs_s(nullptr, buff, pMd5, 33) == 0) {
			CList->SetItemText(i, 4, buff);
		}
		cFile.Close();
	}
	this->DoSomeMenu(ID_32776);
	return 0;
}


/*
	������Ѱ���ļ��ص�
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


		if (0== lstrcmp(_T("20200923_�ѿ����������.mp4"), pFileInfo->data.cFileName))
			CList->SetItemText(idx, 5, _T("�����ļ���������"));

		if (threadIsRunSearch == 2) {
			str = pFileInfo->path;
			str += pFileInfo->data.cFileName;
				str = CMD5Checksum::GetMD5(str);
			CList->SetItemText(idx, 4, str);
		}
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

void CController::DoSomeEdit()
{
	if (gView.m_Statu.tKind.str == gszPEFunctions[9]) {
		CString str;
		gView.m_PVEdit->GetWindowTextW(str);
		DWORD RVA = _tcstol(str, NULL, 16);
		DWORD FOA = gView.m_CPE.RvaToFoa(RVA);
		str.Format(L"%lX", RVA+gView.m_CPE.NTHead_Info.dwPEHead[1]);
		gView.m_PVList->SetItemText(0, 2, str);
		str.Format(L"%lX", RVA);
		gView.m_PVList->SetItemText(1, 2, str);
		str.Format(L"%lX", FOA);
		gView.m_PVList->SetItemText(2, 2, str);
	}
}
BOOL CController::CheckFileMd5(char pMd5[33])
{
	TCHAR tBuff[MAX_PATH];	CString str;
	if (mbstowcs_s(nullptr, tBuff, pMd5, 33) == 0) {
		str = tBuff;
		if (str.Find(L"B3C4221B573D4D403F23D310BB2E42C3") != -1)
		{
			return true;
		}
	}
	return 0;
}
#pragma endregion


