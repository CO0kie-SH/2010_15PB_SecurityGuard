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
	�����������ؼ���������
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_09-30
*/
void CMyView::DoSomeThingTree(HTREEITEM& hTree)
{
	MyTreeInfo& tInfo = m_tLeafs[hTree];		//map�ҵ���Ӧ������Ϣ
	MyTreeInfo tmp = MyTreeInfo{				//�½���ʱ����
		/*�����*/	0,
		/*�����*/	hTree,
		/*	���*/	tInfo.uiDeep + 1
	};
	if (tInfo.hrTree == this->m_tRoot->hFile.htTree	//�������Ϊ�ļ�
		&& tInfo.uiDeep > 0) {						//�����>0
		OutputDebugString(_T("\tDo�����ļ�\n"));
		CString strPath;
		GetTreePath(tInfo.htTree, tInfo.hrTree, strPath);
	}
}

/*
	�������������ؼ�
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_10-40
*/
BOOL CMyView::GetTreePath(const HTREEITEM& htTree,
	const HTREEITEM& hrTree, CString& str)
{

	return FALSE;
}
