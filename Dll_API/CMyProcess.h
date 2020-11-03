#pragma once
#include "CInclude.h"

#define		gdefPCFunctions_MAX		0x01//3
#define		gdefMDFunctions_MAX		0x01//3
#define		gdefTHFunctions_MAX		0x01//5


#define		gdsz_ö�ٽ���				0x00
#define		gdsz_ö���߳�				0x00
#define		gdsz_ö��ģ��				0x00
constexpr	PTCHAR	gszPCFunctions[] = {
	(PTCHAR)_T("ö�ٽ���"),
	(PTCHAR)_T("��������"),
	(PTCHAR)_T("ǿ�ƽ�������")
};

constexpr	PTCHAR	gszMDFunctions[] = {
	(PTCHAR)_T("ö��ģ��"),
	(PTCHAR)_T("����ģ��"),
	(PTCHAR)_T("ж��ģ��")
};

constexpr	PTCHAR	gszTHFunctions[] = {
	(PTCHAR)_T("ö���߳�"),
	(PTCHAR)_T("�����߳�"),
	(PTCHAR)_T("�����߳�"),
	(PTCHAR)_T("�ָ��߳�"),
	(PTCHAR)_T("ǿ�ƽ����߳�")
};


typedef struct _THREADINFO {	//�ṹ��[�߳���Ϣ]
	DWORD   th32ThreadID;       //�߳�ID
	DWORD   th32OwnerProcessID; //����PID
	LONG    tpBasePri;			//�������ȼ�
	SYSTEMTIME thCreatTime;		//�̴߳���ʱ��
}THREADINFO, * LPTHREADINFO;


typedef struct _MODULEINFO {	//�ṹ��[ģ����Ϣ]
	DWORD	pPID;				//��������ID
	DWORD	size;				//ģ��Ĵ�С
	LPVOID	modBaseAddr;		//ģ��ļ��ػ���ַ
	WCHAR   name[MAX_PATH];		//ģ����
	WCHAR   path[MAX_PATH];		//����·��
}MODULEINFO, * LPMODULEINFO;


typedef struct _PROCESSINFO {	//�ṹ��[������Ϣ]
	DWORD	tPID;				//��PID
	DWORD	pPID;				//��PID
	DWORD	tTHs;				//�߳���
	DWORD	tMDs;				//ģ����
	BYTE	is32;				//����λ��
	//LPMODULEINFO pMD;			//ģ����Ϣ
	TCHAR	name[MAX_PATH];		//������
	//TCHAR	path[MAX_PATH];
}PROCESSINFO, * LPPROCESSINFO;


class DLLexp CMyProcess
{
public:
	CMyProcess();
	~CMyProcess();

	BOOL EnumProcess(vector<PROCESSINFO>* proInfos,
		bool bGetMod = false, bool bCleanMem = false);
	BOOL EnumThread(vector<THREADINFO>& threadInfos, DWORD dwPid = 0);
	BOOL EnumModule(vector<MODULEINFO>& moduleInfos, DWORD dwPid = 0);
private:

};

