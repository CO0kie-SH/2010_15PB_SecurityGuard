// 2010_15PB_SecurityGuard.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "..\Dll_API\CApi.h"
#pragma comment(lib,"..\\x64\\Debug\\Dll_API.lib")

#include "..\DLL_HOOK64\CHook.h"
#pragma comment(lib,"..\\x64\\Debug\\DLL_HOOK64.lib")



ULONG_PTR _OldmsgBoxW;
LPVOID _MymsgBoxW;
LPVOID _IAT2msgW = nullptr;
BOOL	_isHook = false;


static int
WINAPI
MessageBoxWMy(
	 HWND hWnd,
	 LPCWSTR lpText,
	 LPCWSTR lpCaption,
	 UINT uType) {
	return	MessageBoxA(0, "被截获", 0, 0);
}

static BOOL DoHook() {
	if (!_IAT2msgW)	return false;
	memcpy(_IAT2msgW, &_MymsgBoxW, 8);
	ULONG_PTR	val;
	memcpy(&val, _IAT2msgW, 8);
	printf_s("请观察IAT %p\t%016llX\n", _IAT2msgW, val);
	_isHook = true;
	return true;
};
static BOOL UnHook() {
	if (_isHook) {
		memcpy(_IAT2msgW, &_OldmsgBoxW, 8);
		ULONG_PTR	val;
		memcpy(&val, _IAT2msgW, 8);
		printf_s("请观察IAT %p\t%016llX\n", _IAT2msgW, val);
		_isHook = false;
		return true;
	}
	return false;
};



class MyIATHook
{
public:
	MyIATHook();
	~MyIATHook();
	
	static int WINAPI MessageBoxWMy2(
		HWND hWnd,
		LPCWSTR lpText,
		LPCWSTR lpCaption,
		UINT uType) {
		return 0;
	};
private:
};

MyIATHook::MyIATHook()
{
	_OldmsgBoxW = (ULONG_PTR)MessageBoxW;
	_MymsgBoxW = (LPVOID)MessageBoxWMy;


	LPVOID ExeBase = GetModuleHandleA(0);
	char* _ModAdd = (char*)ExeBase;
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)_ModAdd;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + _ModAdd);
	//1 获取到导入表的数据目录结构
	PIMAGE_DATA_DIRECTORY dwImportDir = &pNt->OptionalHeader.DataDirectory[1];
	//1 获取到导入表结构
	PIMAGE_IMPORT_DESCRIPTOR pImportTable =
		(PIMAGE_IMPORT_DESCRIPTOR)(_ModAdd + dwImportDir->VirtualAddress);


	printf_s("IAT表地址 %p\n", pImportTable);
	PIMAGE_THUNK_DATA pIat = nullptr;
	while (pImportTable->Name != 0 && !_IAT2msgW) {
		char* pDllName = (pImportTable->Name + _ModAdd);
		HMODULE hModule = GetModuleHandleA(pDllName);
		printf_s("Name0x%p;Add=0x%p\t%s\n",
			pDllName, hModule, pDllName);
		if (hModule/* && strcmp("user32.dll", pDllName) == 0*/) {
			LPVOID dwTarAddress = (LPVOID)GetProcAddress(hModule, "MessageBoxW");
			//printf_s("\tmsgBoxW:0x%p\n", dwTarAddress);
			//循环IAT地址表
			pIat = (PIMAGE_THUNK_DATA)
				(pImportTable->FirstThunk + _ModAdd);
			while (pIat && pIat->u1.Function)
			{
				printf_s("%s\t%llX == ?原版0x%p\n",
					_OldmsgBoxW == pIat->u1.Function ? "Yes" : "",
					pIat->u1.Function,
					(LPVOID)MessageBoxW);
				if (_OldmsgBoxW == pIat->u1.Function) {
					ULONGLONG	val;
					memcpy(&val, pIat, 8);
					printf_s("请观察IAT %p\t%016llX\n", pIat, val);
					_IAT2msgW = pIat;
					DWORD old = 0;
					VirtualProtect(pIat, 4, PAGE_EXECUTE_READWRITE, &old);
					break;
				}
				pIat++;
			}
			//return false;
		}
		++pImportTable;
	}
	
}

MyIATHook::~MyIATHook()
{
}

CHook* mgHook = nullptr;

int main() {
	setlocale(LC_ALL, "chs");
	size_t i = 0;
	vector<PROCESSINFO> proInfos;
	CMyProcess cPro;
	PROCESSINFO   pInfo;
	if (cPro.EnumProcess(&proInfos)) {
		for (const auto& item : proInfos)
		{
			//printf_s("%llu\t%lu\t%lu\t", ++i,
			//	item.pPID, item.tPID);
			wprintf_s(L"%s\n", item.name);
			if (0 == wcscmp(item.name, L"peTest.exe")) {
				pInfo = item;
				break;
			}
		}

	}
	//MyIATHook iat;
	//DoHook();
	CHook* pHook = (CHook*)PrintInfo();
	pHook->Print();

	int key;
	HANDLE hPro;
	while (key = getchar())
	{
		printf_s("key=%d\n", key);
		if (key == 'e')    break;
		else if (key == 'm')
			MessageBoxA(0, "原始A", 0, 0);
		else if (key == 'M')
			MessageBoxW(0, L"原始W", 0, 0);
		else if (key == 'o')
			pHook->DoHook();
		else if (key == 'u')
			pHook->UnHook();
		else if (key == 'p')
			hPro=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, GetCurrentProcessId());
		else if (key == 'P')
			hPro=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, 2600);
	}
	return 0;
}