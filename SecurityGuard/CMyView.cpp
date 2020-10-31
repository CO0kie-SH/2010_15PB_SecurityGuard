#include "pch.h"
#include "CMyView.h"


CMyView gView;


CMyView::CMyView()
{
	this->m_tRoot = new MyFunction{ 0 };
	this->m_tRoot->hEND.htTree = (HTREEITEM)0xFF;
	m_tRoot->hFile.str = _T("文件功能区");
	m_tRoot->hProcsss.str = _T("进程功能区");
	m_tRoot->hThread.str = _T("线程功能区");
	m_tRoot->hModdle.str = _T("模块功能区");
	m_tRoot->hHwnd.str = _T("窗口功能区");
	m_tRoot->hFile.str = _T("文件功能区");
	m_tRoot->hPE.str = _T("PE头功能区");
	//m_tRoot->hEND.str		= _T("功能区");

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
		/*本句柄*/	0,
		/*根句柄*/	hTree,
		/*	深度*/	1
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
	函数：初始化列表控件信息
	作者：CO0kie丶
	时间：2020-10-31_15-45
*/
void CMyView::InitList(const HTREEITEM& hTree)
{
	if (hTree == this->m_LCKind)	return;		//如果相同则返回
	this->m_LCKind = hTree;
	m_PVList->DeleteAllItems();
	while (m_PVList->DeleteColumn(0));

	if (hTree == this->m_tRoot->hFile.htTree) {
		m_PVList->InsertColumn(0, _T("修改时间"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("创建时间"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("文件名"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("大小"), LVCFMT_CENTER, 88);
	}
	m_PVList->InsertColumn(0, _T("序"), LVCFMT_CENTER, 50);
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
	函数：对树控件增加内容
	作者：CO0kie丶
	时间：2020-10-31_09-30
*/
void CMyView::DoSomeThingTree(HTREEITEM& hTree)
{
	MyTreeInfo& tInfo = m_tLeafs[hTree];		//map找到对应的树信息
	InitList(tInfo.hrTree);						//初始化列表控件
	MyTreeInfo tmp = MyTreeInfo{				//新建临时变量
		/*本句柄*/	0,
		/*根句柄*/	tInfo.hrTree,
		/*	深度*/	tInfo.uiDeep + 1
	};
	size_t i = 0;
	if (tInfo.hrTree == this->m_tRoot->hFile.htTree	//如果树根为文件
 		&& tInfo.uiDeep > 0) {						//且深度>0
		if (m_PVTree->ItemHasChildren(hTree))
			return;

		OutputDebugString(_T("\tDo搜索文件\n"));
		CString strPath;	UINT deep = tInfo.uiDeep;	//初始化路径和深度
		GetTreePath(tInfo.htTree, deep, strPath);		//通过树干获取路径
		vector<FILEINFO> FLs;							//初始化数组
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
	函数：遍历树控件
	作者：CO0kie丶
	时间：2020-10-31_10-40
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
