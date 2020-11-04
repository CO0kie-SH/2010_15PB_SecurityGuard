#include "pch.h"
#include "CMyProcess.h"

#pragma region ���ں���
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
	������ö�ٽ���
	����һ���洢���ݵ�����
	���������Ƿ�ö��ģ�飬Ĭ��Ϊfalse
	���أ��ɹ�Ϊtrue������Ϊfalse
	���ߣ�CO0kieؼ
	ʱ�䣺2020-11-01_22-35
*/
BOOL CMyProcess::EnumProcess(vector<PROCESSINFO>* proInfos,
	bool bGetMod /*= false*/, bool bCleanMem /*= false*/)
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
	if (proInfos && !proInfos->empty())	proInfos->clear();
	PROCESSINFO tmp;
	do {
		//ѭ���������
		if (proInfos) {
			tmp = PROCESSINFO{
				pe.th32ProcessID,
				pe.th32ParentProcessID,
				pe.cntThreads
			};
			wsprintf(tmp.name, _T("%s"), pe.szExeFile);
			proInfos->push_back(tmp);
		}
		//�ο� https://blog.csdn.net/paschen/article/details/52829867
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
	������ö���߳�
	����һ���洢���ݵ�����
	������������PID��Ϊ0ʱö������
	���أ��ɹ�Ϊtrue������Ϊfalse
	���ߣ�CO0kieؼ
	ʱ�䣺2020-11-02_08-35
*/
BOOL CMyProcess::EnumThread(vector<THREADINFO>& threadInfos, DWORD dwPid)
{
	if (dwPid > 0 && dwPid < 5)	return false;
	HANDLE hToolHelp = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (hToolHelp == INVALID_HANDLE_VALUE)	return false;
	//�ṹ��[�߳���Ϣ][tagTHREADENTRY32]˵��
	/*
typedef struct tagTHREADENTRY32
{
	DWORD   dwSize;				//�ṹ���С
	DWORD   cntUsage;			//�ó�Ա����ʹ�ã�����ʼ������Ϊ�㡣
	DWORD   th32ThreadID;       //TID
	DWORD   th32OwnerProcessID; //PID
	LONG    tpBasePri;			//������̵߳��ں˻������ȼ���
	LONG    tpDeltaPri;			//�ó�Ա����ʹ�ã�����ʼ������Ϊ�㡣
	DWORD   dwFlags;			//�ó�Ա����ʹ�ã�����ʼ������Ϊ�㡣
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
	������ö��ģ��
	����һ���洢���ݵ�����
	������������PID��Ϊ0ʱö������
	���أ��ɹ�Ϊtrue������Ϊfalse
	���ߣ�CO0kieؼ
	ʱ�䣺2020-11-02_09-12
*/
BOOL CMyProcess::EnumModule(vector<MODULEINFO>& moduleInfos, DWORD dwPid)
{
	if (dwPid > 0 && dwPid < 5)	return false;
	HANDLE hToolHelp = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
	if (hToolHelp == INVALID_HANDLE_VALUE)	return false;
	//�ṹ��[ģ����Ϣ][tagMODULEENTRY32W]˵��
	/*
typedef struct tagMODULEENTRY32W
{
	DWORD   th32ProcessID;      // ��������ID
	BYTE  * modBaseAddr;        // ģ��ļ��ػ���ַ
	DWORD   modBaseSize;        // ģ��Ĵ�С
	HMODULE hModule;            // ģ��ľ��(���ػ�ַ)
	WCHAR   szModule[MAX_MODULE_NAME32 + 1];	// ģ����
	WCHAR   szExePath[MAX_PATH];				// ����·��
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
