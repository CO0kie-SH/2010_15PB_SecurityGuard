#include "pch.h"
#include "CHook.h"
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")


int WINAPI MyMessageBoxW(			//����ԭ��
	_In_opt_ HWND hWnd,						//�����ھ��
	_In_opt_ LPCWSTR lpText,				//����
	_In_opt_ LPCWSTR lpCaption,				//����
	_In_ UINT uType)						//��ť��Ϣ��
{
	//CHook* dgHook = dgHook;
	lpText = (wchar_t*)L"�����޸�����";		//�޸�����
	dgHook->UnHook();						//��ԭUser32.BoxW
	int dwReturnCode = MessageBoxW(hWnd, lpText, lpCaption, uType);
	dgHook->DoHook();						//������BoxW
	return dwReturnCode;					//���ص�����ť
};


HANDLE	WINAPI
MyOpenProcess(
	_In_ DWORD dwDesiredAccess,
	_In_ BOOL bInheritHandle,
	_In_ DWORD dwProcessId
) {
	char buff[MAX_PATH];
	wsprintfA(buff, "Ҫ�򿪽���%lu", dwProcessId);
	if (IDNO == MessageBoxA(0, buff, "����", MB_YESNO))
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
	//1 ��ȡ������������Ŀ¼�ṹ
	PIMAGE_DATA_DIRECTORY dwImportDir = &pNt->OptionalHeader.DataDirectory[1];
	//1 ��ȡ�������ṹ
	PIMAGE_IMPORT_DESCRIPTOR pImportTable =
		(PIMAGE_IMPORT_DESCRIPTOR)(_ModAdd + dwImportDir->VirtualAddress);


	//printf_s("IAT���ַ %p\n", pImportTable);
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


			//ѭ��IAT��ַ��
			pIat = (PIMAGE_THUNK_DATA)
				(pImportTable->FirstThunk + _ModAdd);
			while (pIat && pIat->u1.Function)
			{
				bool bT = (ULONGLONG)dwTarAddress == pIat->u1.Function;
				//printf_s("%s\t%llX ==\tԭ��0x%p\n",
				//	bT ? "Yes" : "",
				//	pIat->u1.Function,
				//	dwTarAddress);
				if (bT) {
					LPVOID	val;
					memcpy(&val, pIat, 8);
					//printf_s("��۲�IAT %p\t%p\n", pIat, val);
					_OpenPro.pIAT = pIat;
					DWORD old = 0;
					BOOL ret = VirtualProtect(pIat, 8, PAGE_EXECUTE_READWRITE, &old);
					if (ret)	this->DoHook();
					/*char cbuff[MAX_PATH];
					wsprintfA(cbuff, "V=%d\n%p iat\n%p Old\n%p Open\n",
						ret, pIat, dwTarAddress, (LPVOID)OpenProcess);*/
					MessageBoxW(0, buff, L"ע��ɹ���������", 0);
					//MessageBoxA(0, cbuff, "��ʾ", 0);
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
	//printf_s("thisָ��%p\n", dgHook);
	return dgHook;
}
