#include "pch.h"
#include "CMyProcess.h"

#pragma region 类内函数
CMyProcess::CMyProcess()
{
	HMODULE module = GetModuleHandle(0);
	TCHAR buf[MAX_PATH];
	GetModuleFileName(module, buf, MAX_PATH);
	PathRemoveFileSpec(buf);
	//str.Format(_T("%s\\DLL_HOOK64.dll"));
	wcscat_s(buf, MAX_PATH, _T("\\DLL_HOOK64.dll"));
	wcscpy_s((wchar_t*)this->pTheDllPath, MAX_PATH, buf);
}

CMyProcess::~CMyProcess()
{
}

/*
	函数：枚举进程
	参数一：存储数据的数组
	参数二：是否枚举模块，默认为false
	返回：成功为true，否则为false
	作者：CO0kie丶
	时间：2020-11-01_22-35
*/
BOOL CMyProcess::EnumProcess(vector<PROCESSINFO>* proInfos,
	bool bGetMod /*= false*/, bool bCleanMem /*= false*/)
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
	if (proInfos && !proInfos->empty())	proInfos->clear();
	PROCESSINFO tmp;
	do {
		//循环添加数组
		if (proInfos) {
			tmp = PROCESSINFO{
				pe.th32ProcessID,
				pe.th32ParentProcessID,
				pe.cntThreads
			};
			wsprintf(tmp.name, _T("%s"), pe.szExeFile);
			proInfos->push_back(tmp);
		}
		//参考 https://blog.csdn.net/paschen/article/details/52829867
		if (bCleanMem) {
			HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
			if (hProcess)
			{
				::SetProcessWorkingSetSize(hProcess, (SIZE_T)-1, (SIZE_T)-1);
				::CloseHandle(hProcess);
			}
		}
	} while (Process32Next(hToolHelp, &pe));
	CloseHandle(hToolHelp);
	if (proInfos && !proInfos->empty())
		return true;
	if (bCleanMem)	return true;
	return false;
}

/*
	函数：枚举线程
	参数一：存储数据的数组
	参数二：进程PID，为0时枚举所有
	返回：成功为true，否则为false
	作者：CO0kie丶
	时间：2020-11-02_08-35
*/
BOOL CMyProcess::EnumThread(vector<THREADINFO>& threadInfos, DWORD dwPid)
{
	if (dwPid > 0 && dwPid < 5)	return false;
	HANDLE hToolHelp = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (hToolHelp == INVALID_HANDLE_VALUE)	return false;
	//结构体[线程信息][tagTHREADENTRY32]说明
	/*
typedef struct tagTHREADENTRY32
{
	DWORD   dwSize;				//结构体大小
	DWORD   cntUsage;			//该成员不再使用，并且始终设置为零。
	DWORD   th32ThreadID;       //TID
	DWORD   th32OwnerProcessID; //PID
	LONG    tpBasePri;			//分配给线程的内核基本优先级。
	LONG    tpDeltaPri;			//该成员不再使用，并且始终设置为零。
	DWORD   dwFlags;			//该成员不再使用，并且始终设置为零。
} THREADENTRY32;
	*/
	THREADENTRY32 ThreadInfo = { sizeof(THREADENTRY32) };

	if (Thread32First(hToolHelp, &ThreadInfo))
	{
		THREADINFO tmp;
		do {
			if (dwPid > 0 &&
				dwPid != ThreadInfo.th32OwnerProcessID)
				continue;
			else if (ThreadInfo.th32ThreadID == 0)
				continue;
			tmp = THREADINFO{
				ThreadInfo.th32ThreadID,
				ThreadInfo.th32OwnerProcessID,
				ThreadInfo.tpBasePri
			};
			threadInfos.push_back(tmp);
		} while (Thread32Next(hToolHelp, &ThreadInfo));
	}

	CloseHandle(hToolHelp);
	return !threadInfos.empty();
}

/*
	函数：枚举模块
	参数一：存储数据的数组
	参数二：进程PID，为0时枚举所有
	返回：成功为true，否则为false
	作者：CO0kie丶
	时间：2020-11-02_09-12
*/
BOOL CMyProcess::EnumModule(vector<MODULEINFO>& moduleInfos, DWORD dwPid)
{
	if (dwPid > 0 && dwPid < 5)	return false;
	HANDLE hToolHelp = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
	if (hToolHelp == INVALID_HANDLE_VALUE)	return false;
	//结构体[模块信息][tagMODULEENTRY32W]说明
	/*
typedef struct tagMODULEENTRY32W
{
	DWORD   th32ProcessID;      // 所属进程ID
	BYTE  * modBaseAddr;        // 模块的加载基地址
	DWORD   modBaseSize;        // 模块的大小
	HMODULE hModule;            // 模块的句柄(加载基址)
	WCHAR   szModule[MAX_MODULE_NAME32 + 1];	// 模块名
	WCHAR   szExePath[MAX_PATH];				// 所在路径
} MODULEENTRY32W;
	*/
	MODULEENTRY32 ModuleInfo = { sizeof(MODULEENTRY32) };
	if (Module32First(hToolHelp, &ModuleInfo))
	{
		MODULEINFO tmp;
		do {
			tmp = MODULEINFO{
				ModuleInfo.th32ProcessID,
				ModuleInfo.modBaseSize,
				ModuleInfo.modBaseAddr
			};
			wsprintf(tmp.name, _T("%s"), ModuleInfo.szModule);
			wsprintf(tmp.path, _T("%s"), ModuleInfo.szExePath);
			moduleInfos.push_back(tmp);
		} while (Module32Next(hToolHelp, &ModuleInfo));
	}
	CloseHandle(hToolHelp);
	return !moduleInfos.empty();
}
#pragma endregion
