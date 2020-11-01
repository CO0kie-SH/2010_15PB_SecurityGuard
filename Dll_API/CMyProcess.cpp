#include "pch.h"
#include "CMyProcess.h"
//#include <strsafe.h>

#pragma region ���ں���
CMyProcess::CMyProcess()
{
}

CMyProcess::~CMyProcess()
{
}
BOOL CMyProcess::EnumProcess(vector<PROCESSINFO>& proInfos, bool bGetMod /*= false*/)
{
	HANDLE hToolHelp = CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS, 0);
	if (hToolHelp == INVALID_HANDLE_VALUE)	return false;
	//�ṹ��[������Ϣ][tagPROCESSENTRY32W]˵��
	/*
typedef struct tagPROCESSENTRY32W
{
	DWORD   dwSize;					//ָ��ṹ��Ĵ�С
	DWORD   cntUsage;				//�ó�Ա����ʹ�ã�����ʼ������Ϊ�㡣
	DWORD   th32ProcessID;          //����PID
	ULONG_PTR th32DefaultHeapID;	//�ó�Ա����ʹ�ã�����ʼ������Ϊ�㡣
	DWORD   th32ModuleID;           //�ó�Ա����ʹ�ã�����ʼ������Ϊ�㡣
	DWORD   cntThreads;				//��������������ִ���߳�����
	DWORD   th32ParentProcessID;    //�������������̵����̣��丸���̣��ı�ʶ����
	LONG    pcPriClassBase;         //�˽��̴������κ��̵߳Ļ������ȼ���
	DWORD   dwFlags;				//�ó�Ա����ʹ�ã�����ʼ������Ϊ�㡣
	WCHAR   szExeFile[MAX_PATH];    //�������̵Ŀ�ִ���ļ������ơ�
} PROCESSENTRY32W;
	*/
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	if (!Process32First(hToolHelp, &pe)) {
		CloseHandle(hToolHelp);
		return false;
	}
	if (proInfos.size())	proInfos.clear();
	PROCESSINFO tmp;
	do {
		tmp = PROCESSINFO{
			pe.th32ProcessID,
			pe.th32ParentProcessID,
			pe.cntThreads
		};
		wsprintf(tmp.name, _T("%s"), pe.szExeFile);
		proInfos.push_back(tmp);
	} while (Process32Next(hToolHelp, &pe));
	CloseHandle(hToolHelp);
	return proInfos.size() != 0;
}
#pragma endregion
