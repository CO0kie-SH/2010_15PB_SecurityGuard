#pragma once

#include "resource.h"
#include "..\Dll_API\CMyFile.h"
#pragma comment(lib,"..\\x64\\Debug\\Dll_API.lib")


#include <map>
using std::map;

#define defMAX_Function		0x06
#define defMAX_PEindex		0x0B

const	PTCHAR	gszPEFunctions[] = {
	_T("PE文件头"),
	_T("区段信息"),
	_T("目录信息"),
	_T("导 入  表"),
	_T("导 出  表"),
	_T("资 源  表"),
	_T("重定位表"),
	_T("延 迟  表"),
	_T("T L S 表"),
	_T("地址转换"),
	_T("时间转换")
};

typedef struct _MyTreeInfo
{
	HTREEITEM	htTree = 0;
	HTREEITEM	hrTree = 0;
	UINT		uiDeep = 0;
	CString		str;
}MyTreeInfo, * PMyTreeInfo;

typedef struct _MyFunction
{
	MyTreeInfo	hProcsss;
	MyTreeInfo	hThread;
	MyTreeInfo	hModdle;
	MyTreeInfo	hHwnd;
	MyTreeInfo	hFile;
	MyTreeInfo	hPE;
	MyTreeInfo	hEND;
}MyFunction, * PMyFunction;

class CMyView
{
public:
	CMyView();
	~CMyView();

	void Init(CDialogEx* wMain);
	void Init(const HTREEITEM& hTree);
	void DoSomeThingTree(HTREEITEM& hTree);
	BOOL GetTreePath(				//遍历树干文本
		const HTREEITEM& htTree,	//传入的尾部句柄
		const HTREEITEM& hrTree,	//传入的截至条件
		CString& str);
protected:
	//CMyFile*	m_CFile		= nullptr;
private:
	CDialogEx* m_Main = nullptr;
	CListCtrl* m_PVList = nullptr;
	CTreeCtrl* m_PVTree = nullptr;
	PMyFunction	m_tRoot = nullptr;
	DWORD		m_dw = 0;
	CString		m_str;
	map<HTREEITEM, MyTreeInfo> m_tLeafs;
};

extern CMyView gView;