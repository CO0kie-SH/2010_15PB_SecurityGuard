#pragma once

#include "resource.h"
#include "..\Dll_API\CMyFile.h"
#pragma comment(lib,"..\\x64\\Debug\\Dll_API.lib")


#include <map>
using std::map;

#define defMAX_Function		0x06
#define defMAX_PEindex		0x0B

const	PTCHAR	gszPEFunctions[] = {
	_T("PE�ļ�ͷ"),
	_T("������Ϣ"),
	_T("Ŀ¼��Ϣ"),
	_T("�� ��  ��"),
	_T("�� ��  ��"),
	_T("�� Դ  ��"),
	_T("�ض�λ��"),
	_T("�� ��  ��"),
	_T("T L S ��"),
	_T("��ַת��"),
	_T("ʱ��ת��")
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
	BOOL GetTreePath(				//���������ı�
		const HTREEITEM& htTree,	//�����β�����
		const HTREEITEM& hrTree,	//����Ľ�������
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