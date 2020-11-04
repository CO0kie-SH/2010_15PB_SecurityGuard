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
	this->m_PVList->SetExtendedStyle(/*LVS_EX_CHECKBOXES |*/ //这是复选框
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

	UINT i = 0;
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
	}	//IF END：文件功能区初始化
	else if (hTree == this->m_tRoot->fPE.htTree) {
		while (i < gdefPEFunctions_MAX)
		{
			tmp.str = gszPEFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END：PE区初始化
	else if (hTree == this->m_tRoot->fProcsss.htTree) {
		while (i < gdefPCFunctions_MAX)
		{
			tmp.str = gszPCFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END：进程区初始化
	else if (hTree == this->m_tRoot->fThread.htTree) {
		while (i < gdefTHFunctions_MAX)
		{
			tmp.str = gszTHFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END：线程区初始化
	else if (hTree == this->m_tRoot->fModdle.htTree) {
		while (i < gdefMDFunctions_MAX)
		{
			tmp.str = gszMDFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END：模块区初始化
	else if (hTree == this->m_tRoot->fHwnd.htTree) {
		while (i < gdefHWFunctions_MAX)
		{
			tmp.str = gszHWFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END：窗口区初始化
	else if (hTree == this->m_tRoot->fRubbish.htTree) {
		while (i < gdefRBFunctions_MAX)
		{
			tmp.str = gszRBFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END：垃圾区初始化
	else if (hTree == this->m_tRoot->fService.htTree) {
		while (i < gdefSVFunctions_MAX)
		{
			tmp.str = gszSVFunctions[i++];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}	//IF END：服务区初始化
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
	if (tInfo.uiDeep > 0)	InitList(tInfo);//向列表控件初始化该内容

	size_t i = 0;			//临时循环变量
	MyTreeInfo tmp{			//临时树叶变量
		0,					//本句柄
		tInfo.hrTree,		//根句柄
		tInfo.uiDeep + 1	//深度
	};


	if (tInfo.hrTree == this->m_tRoot->fFile.htTree		//如果树根为文件
		&& tInfo.uiDeep > 0) {							//且深度>0
		if (m_PVTree->ItemHasChildren(hTree))			//如果该叶子节点有内容
			return;										//则返回

		OutputDebugString(_T("\tDo搜索文件\n"));
		CString strPath;	UINT deep = tInfo.uiDeep;	//初始化路径和深度
		GetTreePath(tInfo.htTree, deep, strPath);		//通过树干获取路径
		
		this->m_PVEdit->SetWindowTextW(strPath);		//将路径设置为编辑框
		vector<FILEINFO> FLs;							//初始化数组

		bool bSearchSucc = m_CFile.SearchPaths(FLs,		//搜索目录+*
			strPath.GetBuffer(), _T("*"));
		if (bSearchSucc)
		{
			this->InitList(FLs);					//向列表控件添加数据
			size_t max = FLs.size();				//初始化大小
			LPFILEINFO pInfo = &FLs[0];				//初始化循环指针
			for (i = 0; i < max; ++i, ++pInfo)
			{
				if (!pInfo->isDir)	continue;
				tmp.htTree = m_PVTree->InsertItem(
					pInfo->data.cFileName, hTree);
				tmp.str = pInfo->data.cFileName;
				m_tLeafs[tmp.htTree] = tmp;
			}
			m_PVTree->Expand(hTree, TVE_EXPAND);			//循环完毕后展开树枝
		}
	}	//IF END：向文件功能区添加目录
	else if (tInfo.hrTree == this->m_tRoot->fPE.htTree		//如果树根为PE
		&& tInfo.uiDeep == 1) {								//且深度==1

		if (tInfo.str == gszPEFunctions[gdsz_PE文件头]) {
			this->m_PVEdit->GetWindowTextW(this->m_str);
			if (!m_CPE.Init(this->m_str.GetBuffer())) {
				this->m_Main->MessageBox(L"初始化PE失败");
				return;
			}
			if (m_CPE.GetNTHeadInfo()) {
				this->InitList(m_CPE.NTHead_Info);
			}
		}	//IF END：PENT头处理
		else if (tInfo.str == gszPEFunctions[gdsz_PE区段信息]) {
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
				m_str.Format(_T("共%llu个"), zoneInfos.size());
				this->m_PVList->SetItemText(0, 1, m_str);
			}
		}	//IF END：PE区段处理
		else if (tInfo.str == gszPEFunctions[gdsz_PE目录信息]) {
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
		}	//IF END：PE目录处理
		else if (tInfo.str == gszPEFunctions[gdsz_PE导出表信息]) {
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
		}	//IF END：PE导出表处理
		else if (tInfo.str == gszPEFunctions[gdsz_PE导入表信息]) {
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
				}	//While END；
			}
		}	//IF END：PE导入表处理
	}	//IF END：PE信息处理
	else if (tInfo.hrTree == this->m_tRoot->fProcsss.htTree	//如果树根为进程
		&& tInfo.uiDeep == 1) {								//且深度==1
		if (tInfo.str == gszPCFunctions[gdsz_枚举进程]) {		//枚举进程
			vector<PROCESSINFO> proInfos;
			if (m_CProcess.EnumProcess(&proInfos))
				this->InitList(proInfos);
		}
	}	//IF END：进程信息处理
	else if (tInfo.hrTree == this->m_tRoot->fThread.htTree	//如果树根为线程
		&& tInfo.uiDeep == 1) {								//且深度==1
		if (tInfo.str == gszTHFunctions[gdsz_枚举线程]) {		//枚举线程
			vector<THREADINFO> thInfos;
			if (m_CProcess.EnumThread(thInfos),GetCurrentProcessId())
				this->InitList(thInfos);
		}
	}	//IF END：线程信息处理
	else if (tInfo.hrTree == this->m_tRoot->fModdle.htTree	//如果树根为模块
		&& tInfo.uiDeep == 1) {								//且深度==1
		if (tInfo.str == gszMDFunctions[gdsz_枚举模块]) {		//枚举模块
			vector<MODULEINFO> mdInfos;
			if (m_CProcess.EnumModule(mdInfos),GetCurrentProcessId())
				this->InitList(mdInfos);
		}
	}	//IF END：模块信息处理
	else if (tInfo.hrTree == this->m_tRoot->fHwnd.htTree	//如果树根为窗口
		&& tInfo.uiDeep == 1) {								//且深度==1
		if (tInfo.str == gszHWFunctions[gdsz_枚举窗口]) {		//枚举模块
			vector<HWND> hwndInfos;
			CMyHwnd cHwnd;
			if (cHwnd.EnumWindow(&hwndInfos))
				this->InitList(hwndInfos);
		}
	}	//IF END：窗口信息处理
	else if (tInfo.hrTree == this->m_tRoot->fRubbish.htTree		//如果树根为垃圾
		&& tInfo.uiDeep == 1) {									//且深度==1
		unsigned char idx = 0;
		if (tInfo.str == gszRBFunctions[gdsz_系统垃圾清理]) {
			SHEmptyRecycleBinA(this->m_Main->GetSafeHwnd(), "C:\\",
				/*SHERB_NOCONFIRMATION |*/ SHERB_NOPROGRESSUI | SHERB_NOSOUND);
			//SHEmptyRecycleBinA(this->m_Main->GetSafeHwnd(), "D:\\",
			//	SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
		}
		else if (tInfo.str == gszRBFunctions[gdsz_IE垃圾清理]) {
			ShellExecute(NULL, _T("open"), _T("rundll32.exe"),
				_T("InetCpl.cpl,ClearMyTracksByProcess 4351"), NULL, SW_SHOWNORMAL);
			//https://blog.csdn.net/hongweigg/article/details/52238411
		}
		else if (tInfo.str == gszRBFunctions[gdsz_VS垃圾清理]) {
			gCtrl.DoSomeMenu(ID_32785);
		}
	}	//IF END：垃圾信息处理
	else if (tInfo.hrTree == this->m_tRoot->fService.htTree		//如果树根为服务区
		&& tInfo.uiDeep == 1) {									//且深度==1
		if (tInfo.str == gszSVFunctions[gdsz_枚举服务]) {			//是否为枚举服务
			if (m_CServices.EnumServices(m_serviceInfos)) {		//调用枚举服务
				this->InitList(m_serviceInfos);					//成功则插入数据
			}
		}
	}	//IF END：服务信息处理
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
void CMyView::InitList(const MyTreeInfo& tInfo, bool isRef)
{
	if (tInfo.hrTree == this->m_Statu.tKind.hrTree &&	//如果树根相同则返回
		isRef == false)
		return;
	this->m_Statu.tKind = tInfo;						//将新信息赋值给当前状态

	while (m_PVList->DeleteColumn(0));					//清空列
	m_PVList->DeleteAllItems();							//清空行

	m_PVList->InsertColumn(0, _T("备注"), LVCFMT_LEFT, 123);	//增加备注
	if (tInfo.hrTree == this->m_tRoot->fFile.htTree) {			//如果树根为文件
		//m_PVList->InsertColumn(0, _T("修改时间"), LVCFMT_LEFT, 222);
		//m_PVList->InsertColumn(0, _T("创建时间"), LVCFMT_LEFT, 222);
		m_PVList->InsertColumn(0, _T("文件名"), LVCFMT_LEFT, 222);
		m_PVList->InsertColumn(0, _T("大小"), LVCFMT_LEFT, 88);
	}	//IF END：文件信息集
	else if (tInfo.hrTree == this->m_tRoot->fPE.htTree			//如果树根为PE
		&& tInfo.uiDeep == 1) {									//且深度为1
		if (tInfo.str == gszPEFunctions[gdsz_PE文件头]) {		//NT头
			m_PVList->InsertColumn(0, _T("配置值"), LVCFMT_LEFT, 123);
			m_PVList->InsertColumn(0, _T("配置项"), LVCFMT_LEFT, 150);
		}
		else if (tInfo.str == gszPEFunctions[gdsz_PE区段信息]) {	//PE区段信息
			for (BYTE i = gdefZONEInfos_MAX; i--; )
			{
				m_PVList->InsertColumn(0, gszZONEInfos[i], LVCFMT_LEFT, 123);
			}
		}
		else if (tInfo.str == gszPEFunctions[gdsz_PE目录信息]) {	//PE目录信息
			this->m_PVList->InsertColumn(0, _T("大小"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("RVA"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("目录名"), LVCFMT_LEFT, 123);
		}
		else if (tInfo.str == gszPEFunctions[gdsz_PE导出表信息]) {	//PE导出表信息
			this->m_PVList->InsertColumn(0, _T("FOA"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("RVA"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("Api名"), LVCFMT_LEFT, 123);
		}
		else if (tInfo.str == gszPEFunctions[gdsz_PE导入表信息]) {	//PE导出表信息
			this->m_PVList->InsertColumn(0, _T("Hint"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("Thunk"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("FOA"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("RVA"), LVCFMT_LEFT, 123);
			this->m_PVList->InsertColumn(0, _T("Api名"), LVCFMT_LEFT, 123);
		}
	}	//IF END：PE信息集
	else if (tInfo.hrTree == this->m_tRoot->fProcsss.htTree		//如果树根为进程
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszPCFunctions[0]) {
			m_PVList->InsertColumn(0, _T("进程名"), LVCFMT_LEFT, 222);
			m_PVList->InsertColumn(0, _T("线程数"), LVCFMT_LEFT, 77);
			m_PVList->InsertColumn(0, _T("本进程"), LVCFMT_LEFT, 77);
			m_PVList->InsertColumn(0, _T("父进程"), LVCFMT_LEFT, 77);
		}
	}	//IF END：进程信息集
	else if (tInfo.hrTree == this->m_tRoot->fThread.htTree		//如果树根为线程
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszTHFunctions[0]) {
			m_PVList->InsertColumn(0, _T("优先级"), LVCFMT_LEFT, 77);
			m_PVList->InsertColumn(0, _T("TID"), LVCFMT_LEFT, 77);
			m_PVList->InsertColumn(0, _T("PID"), LVCFMT_LEFT, 77);
		}
	}	//IF END：线程信息集
	else if (tInfo.hrTree == this->m_tRoot->fModdle.htTree		//如果树根为模块
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszMDFunctions[0]) {
			m_PVList->InsertColumn(0, _T("模块路径"), LVCFMT_LEFT, 188);
			m_PVList->InsertColumn(0, _T("模块名称"), LVCFMT_LEFT, 188);
			m_PVList->InsertColumn(0, _T("模块大小"), LVCFMT_LEFT, 100);
			m_PVList->InsertColumn(0, _T("模块基址"), LVCFMT_LEFT, 200);
			m_PVList->InsertColumn(0, _T("PID"), LVCFMT_LEFT, 77);
		}
	}	//IF END：模块信息集
	else if (tInfo.hrTree == this->m_tRoot->fHwnd.htTree		//如果树根为窗口
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszHWFunctions[0]) {
			m_PVList->InsertColumn(0, _T("窗口类名"), LVCFMT_LEFT, 100);
			m_PVList->InsertColumn(0, _T("窗口名称"), LVCFMT_LEFT, 200);
			m_PVList->InsertColumn(0, _T("窗口句柄"), LVCFMT_LEFT, 100);
		}
	}	//IF END：窗口信息集
	else if (tInfo.hrTree == this->m_tRoot->fService.htTree		//如果树根服务
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszSVFunctions[0]) {
			m_PVList->InsertColumn(0, _T("服务类型"), LVCFMT_LEFT, 123);
			m_PVList->InsertColumn(0, _T("服务状态"), LVCFMT_LEFT, 123);
			m_PVList->InsertColumn(0, _T("服务名称"), LVCFMT_LEFT, 321);
			m_PVList->InsertColumn(0, _T("PID"), LVCFMT_LEFT, 100);
		}
	}	//IF END：服务信息集
	m_PVList->InsertColumn(0, _T("序"), LVCFMT_LEFT, 50);
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
		this->m_PVList->SetItemText(0, 4, m_str);		//设置进程名
		--fl;
	}
	//this->m_PVList->EnsureVisible((int)max, FALSE);	//滚轮到底
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
	函数：向列表框加入服务信息
	作者：CO0kie丶
	时间：2020-11-02_16-45
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
	if (!NTHead.dwPEHead[0])	return;				//无入口点
	if (bClean)	this->m_PVList->DeleteAllItems();	//删除当前列表
	BYTE i = 16;
	while (i)
	{
		this->m_str.Format(_T("%d"), i--);
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

#pragma region 获取信息函数
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
