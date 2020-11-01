#pragma once

#include "resource.h"
#include "..\Dll_API\CApi.h"
#include "..\Dll_API\MYINFO.h"
#pragma comment(lib,"..\\x64\\Debug\\Dll_API.lib")


#include <map>
using std::map;

#define defMAX_Function		0x06
#define defMAX_PEindex		0x0B

const	PTCHAR	gszTreeFunctions[] = {
	_T("���̹�����"),
	_T("�̹߳�����"),
	_T("ģ�鹦����"),
	_T("���ڹ�����"),
	_T("��������"),
	_T("ɱ��������"),
	_T("�ļ�������"),
	_T("����������"),
	_T("PEͷ������")
};


typedef struct _MyTreeInfo		//�ṹ��[��Ҷ��Ϣ]
{								//�������½ṹ
	HTREEITEM	htTree = 0;		//���屾�ڵ�
	HTREEITEM	hrTree = 0;		//������ڵ�
	UINT		uiDeep = 0;		//���������
	CString		str;			//�����ַ���
}MyTreeInfo, * PMyTreeInfo;

typedef struct _MyFunction		//�ṹ��[֧�ֵĹ���������]
{								//�������¹�����
	MyTreeInfo	fProcsss;		//���̹�����
	MyTreeInfo	fThread;		//�̹߳�����
	MyTreeInfo	fModdle;		//ģ�鹦����
	MyTreeInfo	fHwnd;			//���ڹ�����
	MyTreeInfo	fService;		//��������
	MyTreeInfo	fVirus;			//ɱ��������
	MyTreeInfo	fFile;			//�ļ�������
	MyTreeInfo	fRubbish;		//����������
	MyTreeInfo	fPE;			//PEͷ������
	MyTreeInfo	fEND;			//β���ض�
}MyFunction, * PMyFunction;

typedef struct _WindStatu		//�ṹ��[��ǰ��ͼ״̬]
{
	HTREEITEM	tKind	= nullptr;		//���嵱ǰ���ؼ���ѡ��
	CPE*		LPCPE	= nullptr;		//����һ��PE��
}WindStatu, * PWindStatu;

class CMyView
{
public:
	CMyView();
	~CMyView();

	void Init(CDialogEx* wMain);
	void Init(const HTREEITEM& hTree);


	void InitList(const MyTreeInfo& tInfo);
	void InitList(vector<FILEINFO>& FLs);
	void InitList(_NTHead_INFO& NTHead, bool bClean = true);


	void DoSomeThingTree(HTREEITEM& hTree);


	BOOL GetTreePath(				//���������ı�
		const HTREEITEM& htTree,	//�����β�����
		UINT& uiCount,				//����Ľ�������
		CString& str);				//�������ַ���
protected:
	CMyFile		m_CFile;				//�ļ�
	WindStatu	m_Statu;				//��ͼ״̬
private:
	CDialogEx*	m_Main	 = nullptr;		//������
	CTreeCtrl*	m_PVTree = nullptr;		//���ؼ�
	CListCtrl*	m_PVList = nullptr;		//�б�ؼ�

	PMyFunction	m_tRoot = nullptr;		//������ͼ���ڵ�
	DWORD		m_dw = 0;				//�ֲ���ʱdw����
	CString		m_str;					//�ֲ���ʱstr�ַ���
	map<HTREEITEM, MyTreeInfo> m_tLeafs;//�������ڵ�����
};

extern CMyView gView;					//����һ��ȫ����ͼ