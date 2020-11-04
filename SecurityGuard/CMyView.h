#pragma once

#include "resource.h"
#include "..\Dll_API\CApi.h"
#pragma comment(lib,"..\\x64\\Debug\\Dll_API.lib")

#include <map>
using std::map;

const	PTCHAR	gszTreeFunctions[] = {
	_T("进程功能区"),
	_T("线程功能区"),
	_T("模块功能区"),
	_T("窗口功能区"),
	_T("服务功能区"),
	_T("杀毒功能区"),
	_T("文件功能区"),
	_T("垃圾功能区"),
	_T("PE头功能区")
};


typedef struct _MyTreeInfo		//结构体[树叶信息]
{								//包含以下结构
	HTREEITEM	htTree = 0;		//定义本节点
	HTREEITEM	hrTree = 0;		//定义根节点
	UINT		uiDeep = 0;		//定义树深度
	CString		str;			//定义字符串
}MyTreeInfo, * PMyTreeInfo;

typedef struct _MyFunction		//结构体[支持的功能区索引]
{								//包含以下功能区
	MyTreeInfo	fProcsss;		//进程功能区
	MyTreeInfo	fThread;		//线程功能区
	MyTreeInfo	fModdle;		//模块功能区
	MyTreeInfo	fHwnd;			//窗口功能区
	MyTreeInfo	fService;		//服务功能区
	MyTreeInfo	fVirus;			//杀毒功能区
	MyTreeInfo	fFile;			//文件功能区
	MyTreeInfo	fRubbish;		//垃圾功能区
	MyTreeInfo	fPE;			//PE头功能区
	MyTreeInfo	fEND;			//尾部截断
}MyFunction, * PMyFunction;

typedef struct _WindStatu		//结构体[当前视图状态]
{
	MyTreeInfo	tKind;			//定义当前树控件的选项
	PCHAR		pFilePath = nullptr;	//定义文件路径
}WindStatu, * PWindStatu;

class CMyView
{
	friend class CController;
public:
	CMyView();
	~CMyView();

	void Init(CDialogEx* wMain);
	void Init(const HTREEITEM& hTree);


	void InitList(const MyTreeInfo& tInfo, bool isRef = false);
	void InitList(vector<FILEINFO>&		FLs);
	void InitList(vector<PROCESSINFO>&	PCs);
	void InitList(vector<THREADINFO>&	THs);
	void InitList(vector<MODULEINFO>&	MDs);
	void InitList(vector<HWND>&			HWs);
	void InitList(vector<SERVICEINFO>&	SVs);

	void InitList(_NTHead_INFO& NTHead,	bool bClean = true);


	void DoSomeThingTree(HTREEITEM& hTree);

	BOOL GetTreePath(				//遍历树干文本
		const HTREEITEM& htTree,	//传入的尾部句柄
		UINT& uiCount,				//传入的截至条件
		CString& str);				//传出的字符串
public:
	bool GetWindowTextMy(const HWND hWnd, CStringW& str,
		bool bAddEm = true);
	void* GetPVMenu() {
		return m_PVMenu;
	};
	void* GetPVList() {
		return m_PVList;
	};
protected:
	WindStatu	m_Statu;				//视图状态
	CMyFile		m_CFile;				//文件类
	CPE			m_CPE;					//PE类
	CMyProcess	m_CProcess;
	CMyServer	m_CServices;
	vector<SERVICEINFO> m_serviceInfos;	//服务信息
private:
	CDialogEx*	m_Main	 = nullptr;		//主窗口
	CTreeCtrl*	m_PVTree = nullptr;		//树控件
	CListCtrl*	m_PVList = nullptr;		//列表控件
	CMenu*		m_PVMenu = nullptr;		//菜单控件
	CEditView*	m_PVEdit = nullptr;		//编辑框控件


	PMyFunction	m_tRoot = nullptr;		//定义视图根节点
	DWORD		m_dw = 0;				//局部临时dw变量
	CString		m_str;					//局部临时str字符串
	map<HTREEITEM, MyTreeInfo> m_tLeafs;//定义树节点索引
};

extern CMyView gView;					//定义一个全局视图