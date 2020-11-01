#include "pch.h"
#include "CMyView.h"


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
	this->m_PVList = (CListCtrl*)wMain->GetDlgItem(IDC_LIST1);
	this->m_PVList->SetExtendedStyle(/*LVS_EX_CHECKBOXES |*/ //这是复选框
		LVS_EX_GRIDLINES |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	this->m_PVTree = (CTreeCtrl*)wMain->GetDlgItem(IDC_TREE1);

	PMyTreeInfo tmp = (PMyTreeInfo)this->m_tRoot;
	while (tmp->uiDeep == 0)
	{
		tmp->htTree = this->m_PVTree->InsertItem(tmp->str);
		this->Init(tmp->htTree);
		++tmp;
	}
	return;
}
#pragma region 树控件函数
/*
	函数：初始化树干
	参数一：传入的树句柄
*/
void CMyView::Init(const HTREEITEM& hTree)
{
	MyTreeInfo tmp{			//临时树叶变量
		0,					//本句柄
		hTree,				//根句柄
		1					//深度
	};


	//	IF START：文件功能区初始化
	if (hTree == this->m_tRoot->fFile.htTree) {			//判断树根为文件
		size_t stLen = GetLogicalDriveStrings(0, NULL);	//获取驱动器
		PTCHAR buff = new TCHAR[stLen + 2];				//初始缓冲区
		GetLogicalDriveStrings((DWORD)stLen, buff);		//填入缓冲区
		PTCHAR pDriveStrings = buff;					//初始化字符串指针


		while (stLen = wcslen(pDriveStrings))			//循环判断驱动器
		{
			tmp.htTree = m_PVTree->InsertItem(			//插入树叶
				pDriveStrings, hTree);					//赋值给临时变量
			tmp.str = pDriveStrings;					//赋值树叶字符串
			m_tLeafs[tmp.htTree] = tmp;					//用map记录树叶
			OutputDebugString(tmp.str);
			pDriveStrings += stLen + 1;					//字符串指针偏移
		}
		delete[] buff;									//释放缓冲区
		//IF END：文件功能区初始化
	}


	//	IF START：PE功能区初始化
	else if (hTree == this->m_tRoot->fPE.htTree) {		//判断是否为PE功能区
		for (size_t i = 0; i < defMAX_PEindex; i++)
		{
			tmp.str = gszPEFunctions[i];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
		//IF END：PE功能区初始化
	}
	return;
}

/*
	函数：对树控件增加内容
	作者：CO0kie丶
	时间：2020-10-31_09-30
*/
void CMyView::DoSomeThingTree(HTREEITEM& hTree)
{
	MyTreeInfo& tInfo = m_tLeafs[hTree];	//map找到对应的[树叶信息]
	InitList(tInfo);						//向列表控件初始化该内容

	size_t i = 0;			//临时循环变量
	MyTreeInfo tmp{			//临时树叶变量
		0,					//本句柄
		tInfo.hrTree,		//根句柄
		tInfo.uiDeep + 1	//深度
	};


	//IF START：向文件功能区添加目录
	if (tInfo.hrTree == this->m_tRoot->fFile.htTree	//如果树根为文件
		&& tInfo.uiDeep > 0) {						//且深度>0
		if (m_PVTree->ItemHasChildren(hTree))		//如果该叶子节点有内容
			return;									//则返回

		OutputDebugString(_T("\tDo搜索文件\n"));
		CString strPath;	UINT deep = tInfo.uiDeep;	//初始化路径和深度
		GetTreePath(tInfo.htTree, deep, strPath);		//通过树干获取路径
		vector<FILEINFO> FLs;							//初始化数组


		if (m_CFile.SearchPaths(FLs, strPath.GetBuffer()))	//获取目录
		{
			this->InitList(FLs);		//向列表控件添加数据
			size_t max = FLs.size();	//初始化大小
			LPFILEINFO pInfo = &FLs[0];	//初始化循环指针
			for (i = 0; i < max; ++i, ++pInfo)
			{
				tmp.htTree = m_PVTree->InsertItem(
					pInfo->data.cFileName, hTree);
				m_tLeafs[tmp.htTree] = tmp;
			}
			m_PVTree->Expand(hTree, TVE_EXPAND);	//循环完毕后展开树枝
		}
		//IF END：向文件功能区添加目录
	}
	else if (tInfo.hrTree == this->m_tRoot->fPE.htTree	//如果树根为PE
		&& tInfo.uiDeep == 1) {							//且深度>0
		//处理，表明PE被点击，增加List数据
		CPE* pcPE = nullptr;
		if (tInfo.str == gszPEFunctions[0]) {		//PE头
			pcPE = new CPE(DLLP32);
			if (pcPE->GetNTHeadInfo())
				this->InitList(pcPE->NTHead_Info);
			pcPE->~CPE();
			pcPE = new CPE(DLLP64);
			if (pcPE->GetNTHeadInfo())
				this->InitList(pcPE->NTHead_Info, false);
		}
		else if (tInfo.str == gszPEFunctions[1]) {	//区段信息
			pcPE = new CPE(DLLP32);
			pcPE->RvaToFoa(0, true);
			pcPE->~CPE();
			pcPE = new CPE(DLLP64);
			pcPE->RvaToFoa(0, true);
		}

		if (pcPE)	pcPE->~CPE();
	}
	return;
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
#pragma endregion


#pragma region 列表控件函数
/*
	函数：初始化列表控件信息
	作者：CO0kie丶
	时间：2020-10-31_15-45
*/
void CMyView::InitList(const MyTreeInfo& tInfo)
{
	if (tInfo.hrTree == this->m_Statu.tKind)	return;		//如果相同则返回
	this->m_Statu.tKind = tInfo.hrTree;
	m_PVList->DeleteAllItems();
	while (m_PVList->DeleteColumn(0));

	if (tInfo.hrTree == this->m_tRoot->fFile.htTree) {		//如果树根为文件
		m_PVList->InsertColumn(0, _T("修改时间"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("创建时间"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("文件名"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("大小"), LVCFMT_CENTER, 88);
	}
	else if (tInfo.hrTree == this->m_tRoot->fPE.htTree		//如果树根为PE
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszPEFunctions[0]) {	//NT头
			m_PVList->InsertColumn(0, _T("备注"), LVCFMT_CENTER, 123);
			m_PVList->InsertColumn(0, _T("配置值"), LVCFMT_CENTER, 123);
			m_PVList->InsertColumn(0, _T("配置项"), LVCFMT_CENTER, 150);
		}
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
	return;
}

void CMyView::InitList(_NTHead_INFO& NTHead, bool bClean /*= true*/)
{
	if (!NTHead.dwPEHead[0])	return;				//无入口点
	if (bClean)	this->m_PVList->DeleteAllItems();	//

	BYTE i = 16;
	
	while (i--)
	{
		this->m_str.Format(_T("%d"), i + 1);
		this->m_PVList->InsertItem(0, m_str);
		this->m_PVList->SetItemText(0, 1, gszNTHeadInfos[i]);
		m_str.Format(_T("0x%08lX"), NTHead.dwPEHead[i]);
		if (Idx01镜像地址 == i && NTHead.x64ImageBase)
			m_str.Format(_T("0x%8llX"), NTHead.x64ImageBase);
		this->m_PVList->SetItemText(0, 2, m_str);
	}
	this->m_PVList->InsertItem(0, _T("0"));
	m_str.Format(_T("%d"), NTHead.x64ImageBase ? 64 : 32);
	this->m_PVList->SetItemText(0, 1, _T("文件位数："));
	this->m_PVList->SetItemText(0, 2, m_str);
}
#pragma endregion
