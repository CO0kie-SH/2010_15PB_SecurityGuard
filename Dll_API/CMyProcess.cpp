#include "pch.h"
#include "CMyProcess.h"
//#include <strsafe.h>

#pragma region 类内函数
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
	//结构体[进程信息][tagPROCESSENTRY32W]说明
	/*
typedef struct tagPROCESSENTRY32W
{
	DWORD   dwSize;					//指向结构体的大小
	DWORD   cntUsage;				//该成员不再使用，并且始终设置为零。
	DWORD   th32ProcessID;          //【】PID
	ULONG_PTR th32DefaultHeapID;	//该成员不再使用，并且始终设置为零。
	DWORD   th32ModuleID;           //该成员不再使用，并且始终设置为零。
	DWORD   cntThreads;				//【】进程启动的执行线程数。
	DWORD   th32ParentProcessID;    //【】创建此流程的流程（其父流程）的标识符。
	LONG    pcPriClassBase;         //此进程创建的任何线程的基本优先级。
	DWORD   dwFlags;				//该成员不再使用，并且始终设置为零。
	WCHAR   szExeFile[MAX_PATH];    //【】进程的可执行文件的名称。
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
