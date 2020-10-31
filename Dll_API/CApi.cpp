#include "pch.h"
#include "CApi.h"


#pragma region ����������
/*
	�������������̣���ȡ�ļ�Md5
	���أ���ȡ�ɹ�ʱ������ʱ�����������򷵻�0
	�޸ģ�CO0kieؼ
	ʱ�䣺2020-10-29_21-35
	ԭ���ߣ�osc_ma60u6gg
	ԭʱ�䣺2018/02/28 13:28
	�ο�Դ��https://my.oschina.net/u/4401856/blog/4245706
*/
DWORD GetMd5_ByCertutil(char* pPath, char* md5)
{
	if (!PathFileExistsA(pPath))	return false;
	HANDLE hPipeOutputRead = NULL;
	HANDLE hPipeOutputWrite = NULL;

	//�����ܵ� 
	SECURITY_ATTRIBUTES sa = { sizeof(sa) };
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	//Ϊ��׼����ض��򴴽��ܵ�
	CreatePipe(&hPipeOutputRead,  // read handle
		&hPipeOutputWrite, // write handle
		&sa,      // security attributes
		0      // number of bytes reserved for pipe - 0 default
	);

	//�������� 
	//ʹ�ӽ���ʹ��hPipeOutputWrite��Ϊ��׼���ʹ��hPipeInputRead��Ϊ��׼���룬��ʹ�ӽ����ں�̨����
	STARTUPINFOA si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.wShowWindow = SW_HIDE;
	//si.hStdInput = hPipeInputRead;
	si.hStdOutput = hPipeOutputWrite;
	si.hStdError = hPipeOutputWrite;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	ULONGLONG ullsysTime = GetTickCount64();
	char buff[MAX_PATH];		//��ʼ���ַ���
	if (wsprintfA(buff,			//��ʽ��������
		"\"certutil\"  -hashfile  \"%s\" \"md5\"",
		pPath) > 250)			//������ڳ�·����·������
		return false;			//�򷵻�ʧ��
	if (!CreateProcessA(NULL, (char*)
		buff,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si,
		&pi))
	{
		//puts("CreateProcess failed.");
		return false;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);	// Wait until child process exits.
	DWORD len = 0;							//��ʼ������ֵ��
	GetExitCodeProcess(pi.hProcess, &len);	//��ȡ�����˳���
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	if (len == 0x00) {
		if (ReadFile(
			hPipeOutputRead,	// handle of the read end of our pipe
			&buff,		// address of buffer that receives data
			260,		// number of bytes to read
			&len,		// address of number of bytes read
			NULL		// non-overlapped.
		) && len) {		//�ɹ���ȡ
			char* lpc = buff;				//��ʼ���ַ���ͷ
			while ('\n' != *(lpc++));		//ȡ������λ��
			memcpy(md5, lpc, 32);			//��ֵMd5
			md5[32] = '\0';					//�ض��ַ���
			//printf_s("%s", md5);			//����Md5��ʱ
			ullsysTime = GetTickCount64() - ullsysTime;
		}
		else ullsysTime = 0;
	}
	CloseHandle(hPipeOutputRead);
	CloseHandle(hPipeOutputWrite);
	return (DWORD)ullsysTime;
}
/*
	���������ļ������õ��ļ�ָ��
	���أ��ɹ�����[����Ĭ�϶�]�������ڴ�ָ�룬����Ϊnullptr
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-30_11-35
*/
void* GetFilePtr(char* pPath, __int64& pSize, char* str /*= nullptr*/)
{
	if (!PathFileExistsA(pPath))	return nullptr;		//����ļ��������򷵻�
	HANDLE handle = CreateFileA(pPath, GENERIC_ALL,		//���ļ����
		0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (handle == INVALID_HANDLE_VALUE)
		return nullptr;

	DWORD dwFileSize;
	__int64 qwFileSize = GetFileSize(handle, &dwFileSize);

	if (dwFileSize > 0) {
		OutputDebugStringA("��ʱ�޷��������ļ�\n");
		CloseHandle(handle);
		ExitProcess(defExitID�޷��������ļ�);
	}
	qwFileSize |= (((__int64)dwFileSize) << 32);	//����32λ�õ���λ��С
	if (qwFileSize == 0)		//�ж��ļ���С
		return nullptr;			//Ϊ0�򷵻�
	pSize = qwFileSize;			//ͨ��ʵ�η����ļ���С

	if (str) {					//�������ʵ��str���򱣴�str
		char buff[33];	buff[32] = '\0';
		PSTR pstr = StrFormatByteSize64A(qwFileSize, buff, 32);
		wsprintfA(str, "%s", pstr);
	}
	HANDLE	hHeap = GetProcessHeap();	//��ȡĬ�϶�
	LPVOID	lpMem = HeapAlloc(hHeap, HEAP_GENERATE_EXCEPTIONS, qwFileSize);
	if (!lpMem) {
		MessageBoxA(0, "����ѿռ�ʧ��\n", 0, 0);
	}
	else if (ReadFile(handle, lpMem, (DWORD)qwFileSize, &dwFileSize, NULL)
		&& dwFileSize)
	{
		if (dwFileSize == (DWORD)qwFileSize) {
			CloseHandle(handle);
			return lpMem;
		}
	}
	CloseHandle(handle);
	HeapFree(hHeap, 0, lpMem);	lpMem = nullptr;
	return lpMem;
}


#pragma endregion


