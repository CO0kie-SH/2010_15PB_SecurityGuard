#include "pch.h"
#include "CMyView.h"
#include "CController.h"

CMyView gView;

CMyView::CMyView()
{
	this->m_tRoot = new MyFunction{ 0 };
	this->m_tRoot->fEND.uiDeep = 0xFFFF;
	PMyTreeInfo tmp = (PMyTreeInfo)this->m_tRoot;
	for (BYTE i = 0; tmp->uiDeep == 0; ++tmp) {
		tmp->str = gszTreeFunctions[i++];
	}
}

CMyView::~CMyView()
{
	delete this->m_tRoot;
}

void CMyView::Init(CDialogEx* wMain)
{
	this->m_Main = wMain;
	this->m_PVMenu = wMain->GetMenu();
	this->m_PVList = (CListCtrl*)wMain->GetDlgItem(IDC_LIST1);
	this->m_PVList->SetExtendedStyle(/*LVS_EX_CHECKBOXES |*/ //���Ǹ�ѡ��
		LVS_EX_GRIDLINES |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	this->m_PVTree = (CTreeCtrl*)wMain->GetDlgItem(IDC_TREE1);
	this->m_PVEdit = (CEditView*)m_Main->GetDlgItem(IDC_EDIT1);
	PMyTreeInfo tmp = (PMyTreeInfo)this->m_tRoot;
	while (tmp->uiDeep == 0)
	{
		tmp->htTree = this->m_PVTree->InsertItem(tmp->str);
		this->Init(tmp->htTree);
		++tmp;
	}
	return;
}
#pragma region ���ؼ�����
/*
	��������ʼ������
	����һ������������
*/
void CMyView::Init(const HTREEITEM& hTree)
{
	MyTreeInfo tmp{			//��ʱ��Ҷ����
		0,					//�����
		hTree,				//�����
		1					//���
	};

	UINT i = 0;
	if (hTree == this->m_tRoot->fFile.htTree) {			//�ж�����Ϊ�ļ�
		size_t stLen = GetLogicalDriveStrings(0, NULL);	//��ȡ������
		PTCHAR buff = new TCHAR[stLen + 2];				//��ʼ������
		GetLogicalDriveStrings((DWORD)stLen, buff);		//���뻺����
		PTCHAR pDriveStrings = buff;					//��ʼ���ַ���ָ��


		while (stLen = wcslen(pDriveStrings))			//ѭ���ж�������
		{
			tmp.htTree = m_PVTree->InsertItem(			//������Ҷ
				pDriveStrings, hTree);					//��ֵ����ʱ����
			tmp.str = pDriveStrings;					//��ֵ��Ҷ�ַ���
			m_tLeafs[tmp.htTree] = tmp;					//��map��¼��Ҷ
			OutputDebugString(tmp.str);
			pDriveStrings += stLen + 1;					//�ַ���ָ��ƫ��
		}
		delete[] buff;									//�ͷŻ�����
	}	//IF END���ļ���������ʼ��
	else if (hTree == this->m_tRoot->fPE.htTree) {
		while (i < gdefPEFunctions_MAX)
		{
			tmp.str = gszPEFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END��PE����ʼ��
	else if (hTree == this->m_tRoot->fProcsss.htTree) {
		while (i < gdefPCFunctions_MAX)
		{
			tmp.str = gszPCFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END����������ʼ��
	else if (hTree == this->m_tRoot->fThread.htTree) {
		while (i < gdefTHFunctions_MAX)
		{
			tmp.str = gszTHFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END���߳�����ʼ��
	else if (hTree == this->m_tRoot->fModdle.htTree) {
		while (i < gdefMDFunctions_MAX)
		{
			tmp.str = gszMDFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END��ģ������ʼ��
	else if (hTree == this->m_tRoot->fHwnd.htTree) {
		while (i < gdefHWFunctions_MAX)
		{
			tmp.str = gszHWFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END����������ʼ��
	else if (hTree == this->m_tRoot->fRubbish.htTree) {
		while (i < gdefRBFunctions_MAX)
		{
			tmp.str = gszRBFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END����������ʼ��
	else if (hTree == this->m_tRoot->fService.htTree) {
		while (i < gdefSVFunctions_MAX)
		{
			tmp.str = gszSVFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END����������ʼ��
	return;
}

/*
	�����������ؼ���������
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_09-30
*/
void CMyView::DoSomeThingTree(HTREEITEM& hTree)
{
	MyTreeInfo& tInfo = m_tLeafs[hTree];	//map�ҵ���Ӧ��[��Ҷ��Ϣ]
	if (tInfo.uiDeep > 0)	InitList(tInfo);//���б�ؼ���ʼ��������

	size_t i = 0;			//��ʱѭ������
	MyTreeInfo tmp{			//��ʱ��Ҷ����
		0,					//�����
		tInfo.hrTree,		//�����
		tInfo.uiDeep + 1	//���
	};


	if (tInfo.hrTree == this->m_tRoot->fFile.htTree		//�������Ϊ�ļ�
		&& tInfo.uiDeep > 0) {							//�����>0
		if (m_PVTree->ItemHasChildren(hTree))			//�����Ҷ�ӽڵ�������
			return;										//�򷵻�

		OutputDebugString(_T("\tDo�����ļ�\n"));
		CString strPath;	UINT deep = tInfo.uiDeep;	//��ʼ��·�������
		GetTreePath(tInfo.htTree, deep, strPath);		//ͨ�����ɻ�ȡ·��
		
		this->m_PVEdit->SetWindowTextW(strPath);		//��·������Ϊ�༭��
		vector<FILEINFO> FLs;							//��ʼ������

		bool bSearchSucc = m_CFile.SearchPaths(FLs,		//����Ŀ¼+*
			strPath.GetBuffer(), _T("*"));
		if (bSearchSucc)
		{
			this->InitList(FLs);					//���б�ؼ��������
			size_t max = FLs.size();				//��ʼ����С
			LPFILEINFO pInfo = &FLs[0];				//��ʼ��ѭ��ָ��
			for (i = 0; i < max; ++i, ++pInfo)
			{
				if (!pInfo->isDir)	continue;
				tmp.htTree = m_PVTree->InsertItem(
					pInfo->data.cFileName, hTree);
				tmp.str = pInfo->data.cFileName;
				m_tLeafs[tmp.htTree] = tmp;
			}
			m_PVTree->Expand(hTree, TVE_EXPAND);			//ѭ����Ϻ�չ����֦
		}
	}	//IF END�����ļ����������Ŀ¼
	else if (tInfo.hrTree == this->m_tRoot->fPE.htTree		//�������ΪPE
		&& tInfo.uiDeep == 1) {								//�����==1

		if (tInfo.str == gszPEFunctions[gdsz_PE�ļ�ͷ]) {
			this->m_PVEdit->GetWindowTextW(this->m_str);
			if (!m_CPE.Init(this->m_str.GetBuffer())) {
				this->m_Main->MessageBox(L"��ʼ��PEʧ��");
				return;
			}
			if (m_CPE.GetNTHeadInfo()) {
				this->InitList(m_CPE.NTHead_Info);
			}
		}	//IF END��PENTͷ����
		else if (tInfo.str == gszPEFunctions[gdsz_PE������Ϣ]) {
			this->InitList(tInfo, true);

			vector<ZONE_INFO> zoneInfos;
			this->m_CPE.RvaToFoa(1, true, &zoneInfos);
			if (!zoneInfos.empty()) {
				i = zoneInfos.size();
				while (i)
				{
					m_str.Format(_T("%llu"), i--);
					this->m_PVList->InsertItem(0, m_str);
					this->m_PVList->SetItemText(0, 1, zoneInfos[i].chName);
					PDWORD pInfo = (PDWORD)&zoneInfos[i];
					for (BYTE j = 1; j < gdefZONEInfos_MAX; ++j)
					{
						m_str.Format(_T("%08lX"), pInfo[j]);
						this->m_PVList->SetItemText(0, j + 1, m_str);
					}
				}
				this->m_PVList->InsertItem(0, _T("0"));
				m_str.Format(_T("��%llu��"), zoneInfos.size());
				this->m_PVList->SetItemText(0, 1, m_str);
			}
		}	//IF END��PE���δ���
		else if (tInfo.str == gszPEFunctions[gdsz_PEĿ¼��Ϣ]) {
			this->InitList(tInfo, true);


			if (this->m_CPE.GetTableInfo()) {
				PDWORD pdwRVA = (PDWORD)&this->m_CPE.Table_Info.dwRVA;
				PDWORD pdwSize = (PDWORD)&this->m_CPE.Table_Info.dwSize;
				i = 16;
				do {
					m_str.Format(_T("%llu"), i-- );
					this->m_PVList->InsertItem(0, m_str);
					this->m_PVList->SetItemText(0, 1, gszTablesInfos[i]);
					m_str.Format(_T("%08lX"), pdwRVA[i]);
					this->m_PVList->SetItemText(0, 2, m_str);
					m_str.Format(_T("%08lX"), pdwSize[i]);
					this->m_PVList->SetItemText(0, 3, m_str);
				} while (i);
			}
		}	//IF END��PEĿ¼����
		else if (tInfo.str == gszPEFunctions[gdsz_PE��������Ϣ]) {
			this->InitList(tInfo, true);
			vector<PEExport_INFO> exportInfos;
			if (this->m_CPE.GetExportInfo(exportInfos)) {
				i = exportInfos.size();
				TCHAR buff[33];
				while (i--)
				{
					this->m_PVList->InsertItem(0, _T(""));
					if (exportInfos[i].dwIndex)
					{
						m_str.Format(_T("%03lu"), exportInfos[i].dwIndex);
						this->m_PVList->SetItemText(0, 0, m_str);
					}
					mbstowcs_s(nullptr, buff, exportInfos[i].pApiName, 33);
					this->m_PVList->SetItemText(0, 1, buff);
					m_str.Format(_T("%08lX"), exportInfos[i].dwRVA);
					this->m_PVList->SetItemText(0, 2, m_str);
					m_str.Format(_T("%08lX"), exportInfos[i].dwFOA);
					this->m_PVList->SetItemText(0, 3, m_str);
				}

			}
		}	//IF END��PE��������
		else if (tInfo.str == gszPEFunctions[gdsz_PE�������Ϣ]) {
			this->InitList(tInfo, true);
			vector<PEImport_INFO> importInfos;
			if (this->m_CPE.GetImportInfo(importInfos)) {
				i = importInfos.size();
				TCHAR buff[33];
				while (i--)
				{
					this->m_PVList->InsertItem(0, _T(""));
					mbstowcs_s(nullptr, buff, importInfos[i].pApiName, 33);
					this->m_PVList->SetItemText(0, 1, buff);
					m_str.Format(_T("%08lX"), importInfos[i].dwRVA);
					this->m_PVList->SetItemText(0, 2, m_str);
					m_str.Format(_T("%08lX"), importInfos[i].dwFOA);
					this->m_PVList->SetItemText(0, 3, m_str);
					m_str.Format(_T("%08lX"), importInfos[i].Thunk);
					this->m_PVList->SetItemText(0, 4, m_str);

					if (importInfos[i].dwIndex)
					{
						m_str.Format(_T("%03lu"), importInfos[i].dwIndex);
						this->m_PVList->SetItemText(0, 0, m_str);
						m_str.Format(_T("%08lX"), importInfos[i].wdHint);
						this->m_PVList->SetItemText(0, 5, m_str);
					}
				}	//While END��
			}
		}	//IF END��PE�������
	}	//IF END��PE��Ϣ����
	else if (tInfo.hrTree == this->m_tRoot->fProcsss.htTree	//�������Ϊ����
		&& tInfo.uiDeep == 1) {								//�����==1
		if (tInfo.str == gszPCFunctions[gdsz_ö�ٽ���]) {		//ö�ٽ���
			vector<PROCESSINFO> proInfos;
			if (m_CProcess.EnumProcess(&proInfos))
				this->InitList(proInfos);
		}
	}	//IF END��������Ϣ����
	else if (tInfo.hrTree == this->m_tRoot->fThread.htTree	//�������Ϊ�߳�
		&& tInfo.uiDeep == 1) {								//�����==1
		if (tInfo.str == gszTHFunctions[gdsz_ö���߳�]) {		//ö���߳�
			vector<THREADINFO> thInfos;
			if (m_CProcess.EnumThread(thInfos),GetCurrentProcessId())
				this->InitList(thInfos);
		}
	}	//IF END���߳���Ϣ����
	else if (tInfo.hrTree == this->m_tRoot->fModdle.htTree	//�������Ϊģ��
		&& tInfo.uiDeep == 1) {								//�����==1
		if (tInfo.str == gszMDFunctions[gdsz_ö��ģ��]) {		//ö��ģ��
			vector<MODULEINFO> mdInfos;
			if (m_CProcess.EnumModule(mdInfos),GetCurrentProcessId())
				this->InitList(mdInfos);
		}
	}	//IF END��ģ����Ϣ����
	else if (tInfo.hrTree == this->m_tRoot->fHwnd.htTree	//�������Ϊ����
		&& tInfo.uiDeep == 1) {								//�����==1
		if (tInfo.str == gszHWFunctions[gdsz_ö�ٴ���]) {		//ö��ģ��
			vector<HWND> hwndInfos;
			CMyHwnd cHwnd;
			if (cHwnd.EnumWindow(&hwndInfos))
				this->InitList(hwndInfos);
		}
	}	//IF END��������Ϣ����
	else if (tInfo.hrTree == this->m_tRoot->fRubbish.htTree		//�������Ϊ����
		&& tInfo.uiDeep == 1) {									//�����==1
		unsigned char idx = 0;
		if (tInfo.str == gszRBFunctions[gdsz_ϵͳ��������]) {
			SHEmptyRecycleBinA(this->m_Main->GetSafeHwnd(), "C:\\",
				/*SHERB_NOCONFIRMATION |*/ SHERB_NOPROGRESSUI | SHERB_NOSOUND);
			//SHEmptyRecycleBinA(this->m_Main->GetSafeHwnd(), "D:\\",
			//	SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
		}
		else if (tInfo.str == gszRBFunctions[gdsz_IE��������]) {
			ShellExecute(NULL, _T("open"), _T("rundll32.exe"),
				_T("InetCpl.cpl,ClearMyTracksByProcess 4351"), NULL, SW_SHOWNORMAL);
			//https://blog.csdn.net/hongweigg/article/details/52238411
		}
		else if (tInfo.str == gszRBFunctions[gdsz_VS��������]) {
			gCtrl.DoSomeMenu(ID_32785);
		}
	}	//IF END��������Ϣ����
	else if (tInfo.hrTree == this->m_tRoot->fService.htTree		//�������Ϊ������
		&& tInfo.uiDeep == 1) {									//�����==1
		if (tInfo.str == gszSVFunctions[gdsz_ö�ٷ���]) {			//�Ƿ�Ϊö�ٷ���
			if (m_CServices.EnumServices(m_serviceInfos)) {		//����ö�ٷ���
				this->InitList(m_serviceInfos);					//�ɹ����������
			}
		}
	}	//IF END��������Ϣ����
	return;
}


/*
	�������������ؼ�
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_10-40
*/
BOOL CMyView::GetTreePath(const HTREEITEM& htTree,
	UINT& uiCOunt, CString& str)
{
	if (!uiCOunt)
		return FALSE;
	str = m_PVTree->GetItemText(htTree) + _T("\\") + str;
	GetTreePath(m_PVTree->GetParentItem(htTree), --uiCOunt, str);
	return TRUE;
}
#pragma endregion


#pragma region �б�ؼ�����
/*
	��������ʼ���б�ؼ���Ϣ
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_15-45
*/
void CMyView::InitList(const MyTreeInfo& tInfo, bool isRef)
{
	if (tInfo.hrTree == this->m_Statu.tKind.hrTree &&	//���������ͬ�򷵻�
		isRef == false)
		return;
	this->m_Statu.tKind = tInfo;						//������Ϣ��ֵ����ǰ״̬

	while (m_PVList->DeleteColumn(0));					//�����
	m_PVList->DeleteAllItems();							//�����

	m_PVList->InsertColumn(0, _T("��ע"), LVCFMT_LEFT, 123);	//���ӱ�ע
	if (tInfo.hrTree == this->m_tRoot->fFile.htTree) {			//�������Ϊ�ļ�
		//m_PVList->InsertColumn(0, _T("�޸�ʱ��"), LVCFMT_LEFT, 222);
		//m_PVList->InsertColumn(0, _T("����ʱ��"), LVCFMT_LEFT, 222);
		m_PVList->InsertColumn(0, _T("�ļ���"), LVCFMT_LEFT, 222);
		m_PVList->InsertColumn(0, _T("��С"), LVCFMT_LEFT, 88);
	}	//IF END���ļ���Ϣ��
	else if (tInfo.hrTree == this->m_tRoot->fPE.htTree			//�������ΪPE
		&& tInfo.uiDeep == 1) {									//�����Ϊ1
		if (tInfo.str == gszPEFunctions[gdsz_PE�ļ�ͷ]) {		//NTͷ
			m_PVList->InsertColumn(0, _T("����ֵ"), LVCFMT_LEFT, 123);
			m_PVList->InsertColumn(0, _T("������"), LVCFMT_LEFT, 150);
		}
		else if (tInfo.str == gszPEFunctions[gdsz_PE������Ϣ]) {	//PE������Ϣ
			for (BYTE i = gdefZONEInfos_MAX; i--; )
			{
				m_PVList->InsertColumn(0, gszZONEInfos[i], LVCFMT_LEFT, 123);
			}
		}
		else if (tInfo.str == gszPEFunctions[gdsz_PEĿ¼��Ϣ]) {	//PEĿ¼��Ϣ
			this->m_PVList->InsertColumn(0, _T("��С"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("RVA"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("Ŀ¼��"), LVCFMT_LEFT, 123);
		}
		else if (tInfo.str == gszPEFunctions[gdsz_PE��������Ϣ]) {	//PE��������Ϣ
			this->m_PVList->InsertColumn(0, _T("FOA"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("RVA"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("Api��"), LVCFMT_LEFT, 123);
		}
		else if (tInfo.str == gszPEFunctions[gdsz_PE�������Ϣ]) {	//PE��������Ϣ
			this->m_PVList->InsertColumn(0, _T("Hint"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("Thunk"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("FOA"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("RVA"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("Api��"), LVCFMT_LEFT, 123);
		}
	}	//IF END��PE��Ϣ��
	else if (tInfo.hrTree == this->m_tRoot->fProcsss.htTree		//�������Ϊ����
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszPCFunctions[0]) {
			m_PVList->InsertColumn(0, _T("������"), LVCFMT_LEFT, 222);
			m_PVList->InsertColumn(0, _T("�߳���"), LVCFMT_LEFT, 77);
			m_PVList->InsertColumn(0, _T("������"), LVCFMT_LEFT, 77);
			m_PVList->InsertColumn(0, _T("������"), LVCFMT_LEFT, 77);
		}
	}	//IF END��������Ϣ��
	else if (tInfo.hrTree == this->m_tRoot->fThread.htTree		//�������Ϊ�߳�
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszTHFunctions[0]) {
			m_PVList->InsertColumn(0, _T("���ȼ�"), LVCFMT_LEFT, 77);
			m_PVList->InsertColumn(0, _T("TID"), LVCFMT_LEFT, 77);
			m_PVList->InsertColumn(0, _T("PID"), LVCFMT_LEFT, 77);
		}
	}	//IF END���߳���Ϣ��
	else if (tInfo.hrTree == this->m_tRoot->fModdle.htTree		//�������Ϊģ��
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszMDFunctions[0]) {
			m_PVList->InsertColumn(0, _T("ģ��·��"), LVCFMT_LEFT, 188);
			m_PVList->InsertColumn(0, _T("ģ������"), LVCFMT_LEFT, 188);
			m_PVList->InsertColumn(0, _T("ģ���С"), LVCFMT_LEFT, 100);
			m_PVList->InsertColumn(0, _T("ģ���ַ"), LVCFMT_LEFT, 200);
			m_PVList->InsertColumn(0, _T("PID"), LVCFMT_LEFT, 77);
		}
	}	//IF END��ģ����Ϣ��
	else if (tInfo.hrTree == this->m_tRoot->fHwnd.htTree		//�������Ϊ����
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszHWFunctions[0]) {
			m_PVList->InsertColumn(0, _T("��������"), LVCFMT_LEFT, 100);
			m_PVList->InsertColumn(0, _T("��������"), LVCFMT_LEFT, 200);
			m_PVList->InsertColumn(0, _T("���ھ��"), LVCFMT_LEFT, 100);
		}
	}	//IF END��������Ϣ��
	else if (tInfo.hrTree == this->m_tRoot->fService.htTree		//�����������
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszSVFunctions[0]) {
			m_PVList->InsertColumn(0, _T("��������"), LVCFMT_LEFT, 123);
			m_PVList->InsertColumn(0, _T("����״̬"), LVCFMT_LEFT, 123);
			m_PVList->InsertColumn(0, _T("��������"), LVCFMT_LEFT, 321);
			m_PVList->InsertColumn(0, _T("PID"), LVCFMT_LEFT, 100);
		}
	}	//IF END��������Ϣ��
	m_PVList->InsertColumn(0, _T("��"), LVCFMT_LEFT, 50);
}


void CMyView::InitList(vector<FILEINFO>& FLs)
{
	size_t max = FLs.size(), i = max;
	if (max == 0)	return;
	this->m_PVList->DeleteAllItems();
	FILEINFO* fl = nullptr;
	TCHAR buff[MAX_PATH];	int count = (int)max;
	for (i = max, fl = &FLs[max - 1]; i--; --fl) {
		if (!fl->isDir) {
			wsprintf(buff, _T("%d"), count--);
			this->m_PVList->InsertItem(0, buff);
			this->m_PVList->SetItemText(0, 2, fl->data.cFileName);
			StrFormatByteSize64(fl->Size, buff, MAX_PATH);
			this->m_PVList->SetItemText(0, 1, buff);
		}
	}
	for (i = max, fl = &FLs[max - 1]; i--; --fl) {
		if (fl->isDir) {
			wsprintf(buff, _T("%d"), count--);
			this->m_PVList->InsertItem(0, buff);
			this->m_PVList->SetItemText(0, 2, fl->data.cFileName);
			//this->m_PVList->SetItemText(0, 1, _T("Dir"));
		}
	}
	//END
	return;
}


void CMyView::InitList(vector<PROCESSINFO>& PCs)
{
	size_t max = PCs.size(), i = max;
	if (max-- == 0)	return;
	LPPROCESSINFO fl = &PCs[max];
	this->m_PVList->ShowWindow(SW_HIDE);
	while (i)
	{
		m_str.Format(_T("%llu"), i--);
		this->m_PVList->InsertItem(0, m_str);
		m_str.Format(_T("%lu"), fl->pPID);
		this->m_PVList->SetItemText(0, 1, m_str);
		m_str.Format(_T("%lu"), fl->tPID);
		this->m_PVList->SetItemText(0, 2, m_str);
		m_str.Format(_T("%lu"), fl->tTHs);
		this->m_PVList->SetItemText(0, 3, m_str);
		m_str = fl->name;
		this->m_PVList->SetItemText(0, 4, m_str);		//���ý�����
		--fl;
	}
	//this->m_PVList->EnsureVisible((int)max, FALSE);	//���ֵ���
	this->m_PVList->ShowWindow(SW_SHOW);
}


void CMyView::InitList(vector<THREADINFO>& THs)
{
	size_t max = THs.size(), i = max;
	if (max-- == 0)	return;
	LPTHREADINFO th = &THs[max];
	this->m_PVList->ShowWindow(SW_HIDE);
	while (i)
	{
		m_str.Format(_T("%llu"), i--);
		this->m_PVList->InsertItem(0, m_str);
		m_str.Format(_T("%lu"), th->th32OwnerProcessID);
		this->m_PVList->SetItemText(0, 1, m_str);
		m_str.Format(_T("%lu"), th->th32ThreadID);
		this->m_PVList->SetItemText(0, 2, m_str);
		m_str.Format(_T("%lu"), th->tpBasePri);
		this->m_PVList->SetItemText(0, 3, m_str);
		--th;
	}
	this->m_PVList->ShowWindow(SW_SHOW);
}


void CMyView::InitList(vector<MODULEINFO>& MDs)
{
	size_t max = MDs.size(), i = max;
	if (max-- == 0)	return;
	LPMODULEINFO th = &MDs[max];
	while (i)
	{
		m_str.Format(_T("%llu"), i--);
		this->m_PVList->InsertItem(0, m_str);
		m_str.Format(_T("%lu"), th->pPID);
		this->m_PVList->SetItemText(0, 1, m_str);
		m_str.Format(_T("0x%p"), th->modBaseAddr);
		this->m_PVList->SetItemText(0, 2, m_str);
		m_str.Format(_T("0x%06lX"), th->size);
		this->m_PVList->SetItemText(0, 3, m_str);
		this->m_PVList->SetItemText(0, 4, th->name);
		this->m_PVList->SetItemText(0, 5, th->path);
		--th;
	}
}

void CMyView::InitList(vector<HWND>& HWs)
{
	size_t max = HWs.size(), i = max;
	if (max-- == 0)	return;
	this->m_PVList->ShowWindow(SW_HIDE);
	WCHAR buff[MAX_PATH];
	HWND* hwnds = &HWs[max];
	while (i)
	{
		m_str.Format(_T("%llu"), i--);
		this->m_PVList->InsertItem(0, m_str);
		m_str.Format(_T("%llu"), (ULONG64)(*hwnds));
		this->m_PVList->SetItemText(0, 1, m_str);
		if (this->GetWindowTextMy(*hwnds, m_str))
			this->m_PVList->SetItemText(0, 2, m_str);
		if (GetClassNameW(*hwnds, buff, MAX_PATH))
			this->m_PVList->SetItemText(0, 3, buff);
		--hwnds;
	}
	this->m_PVList->ShowWindow(SW_SHOW);
}

/*
	���������б����������Ϣ
	���ߣ�CO0kieؼ
	ʱ�䣺2020-11-02_16-45
*/
void CMyView::InitList(vector<SERVICEINFO>& SVs)
{
	size_t max = SVs.size(), i = max;
	if (max-- == 0)	return;
	this->m_PVList->ShowWindow(SW_HIDE);
	//WCHAR buff[MAX_PATH];
	LPSERVICEINFO service = &SVs[max];
	while (i)
	{
		m_str.Format(_T("%llu"), i--);
		this->m_PVList->InsertItem(0, m_str);

		if (service->PID) {
			m_str.Format(_T("%lu"), service->PID);
			this->m_PVList->SetItemText(0, 1, m_str);
		}
		this->m_PVList->SetItemText(0, 2, service->lpDisplayName);

		if(service->pCurrentState)
			this->m_PVList->SetItemText(0, 3, service->pCurrentState);
		if(service->pServiceType)
			this->m_PVList->SetItemText(0, 4, service->pServiceType);

		--service;
	}
	this->m_PVList->ShowWindow(SW_SHOW);
}


void CMyView::InitList(_NTHead_INFO& NTHead, bool bClean /*= true*/)
{
	if (!NTHead.dwPEHead[0])	return;				//����ڵ�
	if (bClean)	this->m_PVList->DeleteAllItems();	//ɾ����ǰ�б�
	BYTE i = 16;
	while (i)
	{
		this->m_str.Format(_T("%d"), i--);
		this->m_PVList->InsertItem(0, m_str);
		this->m_PVList->SetItemText(0, 1, gszNTHeadInfos[i]);
		m_str.Format(_T("0x%08lX"), NTHead.dwPEHead[i]);
		if (Idx01�����ַ == i && NTHead.x64ImageBase)
			m_str.Format(_T("0x%8llX"), NTHead.x64ImageBase);
		this->m_PVList->SetItemText(0, 2, m_str);
	}
	this->m_PVList->InsertItem(0, _T("0"));
	m_str.Format(_T("%d"), NTHead.x64ImageBase ? 64 : 32);
	this->m_PVList->SetItemText(0, 1, _T("�ļ�λ����"));
	this->m_PVList->SetItemText(0, 2, m_str);
}
#pragma endregion

#pragma region ��ȡ��Ϣ����
bool CMyView::GetWindowTextMy(const HWND hWnd, CStringW& str, bool bAddEm)
{
	int len = GetWindowTextLengthW(hWnd) + 1;
	PWCHAR buff = new WCHAR[len];
	if (!buff)	return false;
	GetWindowTextW(hWnd, buff, len);
	str.Format(L"%s", buff);
	delete[] buff;
	if (!bAddEm && str == L"") {
		return false;
	}
	return true;
}
#pragma endregion
