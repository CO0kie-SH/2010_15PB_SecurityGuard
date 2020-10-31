#include "pch.h"
#include "CMyView.h"


CMyView gView;


CMyView::CMyView()
{
	this->m_tRoot = new MyFunction{ 0 };
	this->m_tRoot->hEND.htTree = (HTREEITEM)0xFF;
	m_tRoot->hFile.str = _T("�ļ�������");
	m_tRoot->hProcsss.str = _T("���̹�����");
	m_tRoot->hThread.str = _T("�̹߳�����");
	m_tRoot->hModdle.str = _T("ģ�鹦����");
	m_tRoot->hHwnd.str = _T("���ڹ�����");
	m_tRoot->hFile.str = _T("�ļ�������");
	m_tRoot->hPE.str = _T("PEͷ������");
	//m_tRoot->hEND.str		= _T("������");

	//m_CFile = new CMyFile();
}

CMyView::~CMyView()
{
	delete this->m_tRoot;
	//m_CFile->~CMyFile();
}

void CMyView::Init(CDialogEx* wMain)
{
	this->m_Main = wMain;
	m_PVList = (CListCtrl*)wMain->GetDlgItem(IDC_LIST1);
	m_PVTree = (CTreeCtrl*)wMain->GetDlgItem(IDC_TREE1);

	PMyTreeInfo tmp = (PMyTreeInfo)this->m_tRoot;
	for (size_t i = 0; i < defMAX_Function; ++i, ++tmp)
	{
		tmp->htTree = m_PVTree->InsertItem(tmp->str);
		this->Init(tmp->htTree);
		//OutputDebugString(tmp->str);
	}


}


void CMyView::Init(const HTREEITEM& hTree)
{
	MyTreeInfo tmp = MyTreeInfo{
		/*�����*/	0,
		/*�����*/	hTree,
		/*	���*/	1
	};
	if (hTree == this->m_tRoot->hFile.htTree) {
		size_t stLen = GetLogicalDriveStrings(0, NULL);
		PTCHAR buff = new TCHAR[stLen + 2];
		GetLogicalDriveStrings((DWORD)stLen, buff);
		PTCHAR pDriveStrings = buff;
		while (stLen = wcslen(pDriveStrings))
		{
			tmp.htTree = m_PVTree->InsertItem(
				pDriveStrings, hTree);
			tmp.str.Format(L"%s", pDriveStrings);
			m_tLeafs[tmp.htTree] = tmp;
			OutputDebugString(tmp.str);
			pDriveStrings += stLen + 1;
		}
		delete[] buff;
	}
	else if (hTree == this->m_tRoot->hPE.htTree) {
		for (size_t i = 0; i < defMAX_PEindex; i++)
		{
			tmp.str.Format(gszPEFunctions[i]);
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
		}
	}
}

/*
	��������ʼ���б�ؼ���Ϣ
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_15-45
*/
void CMyView::InitList(const HTREEITEM& hTree)
{
	if (hTree == this->m_LCKind)	return;		//�����ͬ�򷵻�
	this->m_LCKind = hTree;
	m_PVList->DeleteAllItems();
	while (m_PVList->DeleteColumn(0));

	if (hTree == this->m_tRoot->hFile.htTree) {
		m_PVList->InsertColumn(0, _T("�޸�ʱ��"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("����ʱ��"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("�ļ���"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("��С"), LVCFMT_CENTER, 88);
	}
	m_PVList->InsertColumn(0, _T("��"), LVCFMT_CENTER, 50);
}

void CMyView::InitList(vector<FILEINFO>& FLs)
{
	size_t max = FLs.size(), i = max;
	if (max == 0)	return;
	this->m_PVList->DeleteAllItems();
	CString str;	FILEINFO* fl = nullptr;
	TCHAR buff[MAX_PATH];	int count = (int)max;
	for (i = max, fl = &FLs[max - 1]; i--; --fl) {
		if (fl->Size) {
			wsprintf(buff, _T("%d"), count--);
			this->m_PVList->InsertItem(0, buff);
			this->m_PVList->SetItemText(0, 2, fl->data.cFileName);
			StrFormatByteSize64(fl->Size, buff, MAX_PATH);
			this->m_PVList->SetItemText(0, 1, buff);
		}
	}
	for (i = max, fl = &FLs[max - 1]; i--; --fl) {
		if (!fl->Size) {
			wsprintf(buff, _T("%d"), count--);
			this->m_PVList->InsertItem(0, buff);
			this->m_PVList->SetItemText(0, 2, fl->data.cFileName);
		}
	}
	//END
}

/*
	�����������ؼ���������
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_09-30
*/
void CMyView::DoSomeThingTree(HTREEITEM& hTree)
{
	MyTreeInfo& tInfo = m_tLeafs[hTree];		//map�ҵ���Ӧ������Ϣ
	InitList(tInfo.hrTree);						//��ʼ���б�ؼ�
	MyTreeInfo tmp = MyTreeInfo{				//�½���ʱ����
		/*�����*/	0,
		/*�����*/	tInfo.hrTree,
		/*	���*/	tInfo.uiDeep + 1
	};
	size_t i = 0;
	if (tInfo.hrTree == this->m_tRoot->hFile.htTree	//�������Ϊ�ļ�
 		&& tInfo.uiDeep > 0) {						//�����>0
		if (m_PVTree->ItemHasChildren(hTree))
			return;

		OutputDebugString(_T("\tDo�����ļ�\n"));
		CString strPath;	UINT deep = tInfo.uiDeep;	//��ʼ��·�������
		GetTreePath(tInfo.htTree, deep, strPath);		//ͨ�����ɻ�ȡ·��
		vector<FILEINFO> FLs;							//��ʼ������
		if (m_CFile.SearchPaths(FLs, strPath.GetBuffer()))
		{
			this->InitList(FLs);
			size_t max = FLs.size();
			LPFILEINFO pInfo = &FLs[0];
			do {
				tmp.htTree = m_PVTree->InsertItem(
					pInfo->data.cFileName, hTree);
				m_tLeafs[tmp.htTree] = tmp;
				++pInfo;
			} while (++i < max);
			m_PVTree->Expand(hTree, TVE_EXPAND);
		}
	}
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
