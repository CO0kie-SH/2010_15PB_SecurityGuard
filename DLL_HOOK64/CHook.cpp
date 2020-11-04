#include "pch.h"
#include "CHook.h"
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")


int WINAPI MyMessageBoxW(			//函数原型
	_In_opt_ HWND hWnd,						//父窗口句柄
	_In_opt_ LPCWSTR lpText,				//内容
	_In_opt_ LPCWSTR lpCaption,				//标题
	_In_ UINT uType)						//按钮信息等
{
	//CHook* dgHook = dgHook;
	lpText = (wchar_t*)L"这是修改内容";		//修改内容
	dgHook->UnHook();						//还原User32.BoxW
	int dwReturnCode = MessageBoxW(hWnd, lpText, lpCaption, uType);
	dgHook->DoHook();						//继续钩BoxW
	return dwReturnCode;					//返回弹窗按钮
};


HANDLE	WINAPI
MyOpenProcess(
	_In_ DWORD dwDesiredAccess,
	_In_ BOOL bInheritHandle,
	_In_ DWORD dwProcessId
) {
	char buff[MAX_PATH];
	wsprintfA(buff, "要打开进程%lu", dwProcessId);
	if (IDNO == MessageBoxA(0, buff, "过滤", MB_YESNO))
		return NULL;

	dgHook->UnHook();
	HANDLE hProcess =
		OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
	if (hProcess) {
		dgHook->DoHook();
		TCHAR buff[MAX_PATH];
		DWORD len = MAX_PATH;
		QueryFullProcessImageNameW(hProcess, 0, buff, &len);
		CloseHandle(hProcess);
		if (0 == wcscmp(dgPath, buff)) {
			return NULL;
		}
		dgHook->UnHook();
	}
	hProcess =
		OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	dgHook->DoHook();
	return hProcess;
}


CHook::CHook(HMODULE hModule)
{
	dgHook = this;
	TCHAR buff[MAX_PATH];
	GetModuleFileNameW(hModule, buff, MAX_PATH);
	PathRemoveFileSpec(buff);
	wcscat_s(buff, MAX_PATH, L"\\SecurityGuard.exe");
	wcscpy_s(this->path, MAX_PATH, buff);
	dgPath = this->path;
	//_BoxW = MsgBoxWHookInfo{
	//	(ULONG_PTR)MessageBoxW,
	//	MyMessageBoxW,0,0
	//};
	_OpenPro = {
		(ULONG_PTR)OpenProcess,
		MyOpenProcess,0,0
	};

	LPVOID ExeBase = GetModuleHandleA(0);
	char* _ModAdd = (char*)ExeBase;
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)_ModAdd;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + _ModAdd);
	//1 获取到导入表的数据目录结构
	PIMAGE_DATA_DIRECTORY dwImportDir = &pNt->OptionalHeader.DataDirectory[1];
	//1 获取到导入表结构
	PIMAGE_IMPORT_DESCRIPTOR pImportTable =
		(PIMAGE_IMPORT_DESCRIPTOR)(_ModAdd + dwImportDir->VirtualAddress);


	//printf_s("IAT表地址 %p\n", pImportTable);
	PIMAGE_THUNK_DATA pIat = nullptr;
	while (pImportTable->Name != 0) {
		char* pDllName = (pImportTable->Name + _ModAdd);
		HMODULE hModule = GetModuleHandleA(pDllName);
		//printf_s("Name0x%p;Add=0x%p\t%s\n",
		//	pDllName, hModule, pDllName);
		if (hModule/* && strcmp("user32.dll", pDllName) == 0*/) {
			//LPVOID dwTarAddress = (LPVOID)GetProcAddress(hModule, "MessageBoxW");
			LPVOID dwTarAddress = (LPVOID)GetProcAddress(hModule, "OpenProcess");
			


			if (!dwTarAddress) {
				++pImportTable;
				continue;
			}


			//循环IAT地址表
			pIat = (PIMAGE_THUNK_DATA)
				(pImportTable->FirstThunk + _ModAdd);
			while (pIat && pIat->u1.Function)
			{
				bool bT = (ULONGLONG)dwTarAddress == pIat->u1.Function;
				//printf_s("%s\t%llX ==\t原版0x%p\n",
				//	bT ? "Yes" : "",
				//	pIat->u1.Function,
				//	dwTarAddress);
				if (bT) {
					LPVOID	val;
					memcpy(&val, pIat, 8);
					//printf_s("请观察IAT %p\t%p\n", pIat, val);
					_OpenPro.pIAT = pIat;
					DWORD old = 0;
					BOOL ret = VirtualProtect(pIat, 8, PAGE_EXECUTE_READWRITE, &old);
					if (ret)	this->DoHook();
					/*char cbuff[MAX_PATH];
					wsprintfA(cbuff, "V=%d\n%p iat\n%p Old\n%p Open\n",
						ret, pIat, dwTarAddress, (LPVOID)OpenProcess);*/
					MessageBoxW(0, buff, L"注入成功，将保护", 0);
					//MessageBoxA(0, cbuff, "提示", 0);
					return;
				}
				pIat++;
			}
		}
		++pImportTable;
	}
}

CHook::~CHook()
{
}

void* PrintInfo()
{
	//printf_s("this指针%p\n", dgHook);
	return dgHook;
}
