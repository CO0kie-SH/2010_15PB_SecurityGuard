#pragma once
#include "CInclude.h"

#define		gdefPCFunctions_MAX		0x03
#define		gdefMDFunctions_MAX		0x03
#define		gdefTHFunctions_MAX		0x05


#define		gdsz_ö�ٽ���				0x00
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


typedef struct _THREADINFO		//�ṹ��[�߳���Ϣ]
{
	DWORD   th32ThreadID;       //�߳�ID
	DWORD   th32OwnerProcessID; //����PID
	LONG    tpBasePri;			//�������ȼ�
	SYSTEMTIME thCreatTime;		//�̴߳���ʱ��
}THREADINFO, * LPTHREADINFO;


typedef struct _MODULEINFO {	//�ṹ��[ģ����Ϣ]
	DWORD	pPID;				//��������ID
	DWORD	size;				//ģ��Ĵ�С
	char*	modBaseAddr;		//ģ��ļ��ػ���ַ
	PTCHAR	name;
	PTCHAR	path;
}MODULEINFO, * LPMODULEINFO;


typedef struct _PROCESSINFO {	//�ṹ��[������Ϣ]
	DWORD	tPID;
	DWORD	pPID;
	DWORD	tTHs;
	DWORD	tMDs;
	BYTE	is32;
	LPMODULEINFO tPM;
	TCHAR	name[MAX_PATH];
	TCHAR	path[MAX_PATH];
}PROCESSINFO, * LPPROCESSINFO;


class DLLexp CMyProcess
{
public:
	CMyProcess();
	~CMyProcess();

	BOOL EnumProcess(vector<PROCESSINFO>& proInfos, bool bGetMod = false);
private:

};

