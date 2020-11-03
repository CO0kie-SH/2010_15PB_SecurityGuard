#include "pch.h"
#include "CApi.h"


#pragma region ����������
/*
	�������������̣���ȡ�ļ�Md5
	���أ��ɹ�ʱ������ʱ�����������򷵻�0
	�޸ģ�CO0kieؼ
	ʱ�䣺2020-10-29_21-35
	ԭ���ߣ�osc_ma60u6gg
	ԭʱ�䣺2018/02/28 13:28
	�ο�Դ��https://my.oschina.net/u/4401856/blog/4245706
*/
DWORD GetMd5_ByCertutil(PTCHAR pPath, char md5[33])
{
	if (!PathFileExists(pPath))	return false;
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
	STARTUPINFO si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.wShowWindow = SW_HIDE;
	//si.hStdInput = hPipeInputRead;
	si.hStdOutput = hPipeOutputWrite;
	si.hStdError = hPipeOutputWrite;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	ULONGLONG ullsysTime = GetTickCount64();
	TCHAR buff[MAX_PATH];		//��ʼ���ַ���
	if (wsprintf(buff,			//��ʽ��������
		_T("\"certutil\"  -hashfile  \"%s\" \"md5\""),
		pPath) > 250)			//������ڳ�·����·������
		return false;			//�򷵻�ʧ��
	if (!CreateProcess(NULL,
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
	char chbuff[MAX_PATH];
	if (len == 0x00) {
		if (ReadFile(
			hPipeOutputRead,	// handle of the read end of our pipe
			&chbuff,		// address of buffer that receives data
			260,		// number of bytes to read
			&len,		// address of number of bytes read
			NULL		// non-overlapped.
		) && len) {		//�ɹ���ȡ
			char* lpc = chbuff;				//��ʼ���ַ���ͷ
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
void* GetFilePtr(char* pPath, __int64& pSize, char* strSize /*= nullptr*/)
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
	//qwFileSize |= (((__int64)dwFileSize) << 32);	//����32λ�õ���λ��С
	if (qwFileSize == 0)		//�ж��ļ���С
		return nullptr;			//Ϊ0�򷵻�
	pSize = qwFileSize;			//ͨ��ʵ�η����ļ���С

	if (strSize) {					//�������ʵ��str���򱣴�str
		char buff[33] = {0};
		PSTR pstr = StrFormatByteSize64A(qwFileSize, buff, 32);
		wsprintfA(strSize, "%s", pstr);
	}
	HANDLE	hHeap = GetProcessHeap();	//��ȡĬ�϶�
	LPVOID	lpMem = HeapAlloc(hHeap, 0, (DWORD)qwFileSize);
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
	return nullptr;
}

double FILETIME2Double(const _FILETIME& fileTime)
{
	return double(fileTime.dwHighDateTime * 4.294967296e9) + double(fileTime.dwLowDateTime);
}



/*
	��������ȡCPU״̬
	���ߣ�Github��havocykp
*/
float GetCpuUsage()
{
	//		    �����¼�  �ں��¼�   �û��¼�
	_FILETIME  idleTime, kernelTime, userTime;
	//��ȡʱ��
	GetSystemTimes(&idleTime, &kernelTime, &userTime);
	//�ȴ�1000����
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!hEvent)	return 0.0f;
	WaitForSingleObject(hEvent, 1000);
	//��ȡ�µ�ʱ��
	_FILETIME  newIdleTime, newKernelTime, newUserTime;
	GetSystemTimes(&newIdleTime, &newKernelTime, &newUserTime);
	//������ʱ��ת��
	double dOldIdleTime = FILETIME2Double(idleTime);
	double dNewIdleTime = FILETIME2Double(newIdleTime);
	double dOldKernelTime = FILETIME2Double(kernelTime);
	double dNewKernelTime = FILETIME2Double(newKernelTime);
	double dOldUserTime = FILETIME2Double(userTime);
	double dNewUserTime = FILETIME2Double(newUserTime);
	//�����ʹ����
	return  float(100.0 - (dNewIdleTime - dOldIdleTime) /
		(dNewKernelTime - dOldKernelTime + dNewUserTime - dOldUserTime) * 100.0);
}



/*
	����������Ȩ��
	���ӣ�https://blog.csdn.net/paschen/article/details/52829867
*/
BOOL EnableDebugPrivilege()
{
	BOOL bRet = FALSE;
	HANDLE hToken;
	if (::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		LUID luid;
		if (::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
		{
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1UL;
			tp.Privileges[0].Luid = luid;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
			{
				bRet = TRUE;
			}
		}
		::CloseHandle(hToken);
	}
	return bRet;
}


/*
	������ö���ļ��ص�
	ʱ�䣺2020-11-03_23-20
*/
BOOL CALLBACK EnumFilePaths(void(*CALLBACKPROC)(LPFILEINFO),
	PTCHAR Path, CHeap* heap, PTCHAR Kind /*= nullptr*/)
{
	if (nullptr == Path)		return false;		//���·���������򷵻�
	//if (!PathFileExists(Path))	return false;		//����ļ��������򷵻�
	if (!PathIsDirectory(Path))	return false;		//���Ŀ¼�������򷵻�
	if (PathIsDirectoryEmpty(Path))	return false;	//�����Ŀ¼�򷵻�


	//CHeap cHeap;		//����һ���ѿ�����
	PTCHAR buff = (PTCHAR)heap->HeapAlloc(
		sizeof(TCHAR) * MAX_PATH);
	if (!buff)	return false;

	//��ʼ���ļ���Ϣ
	FILEINFO fl;
	ZeroMemory(&fl, sizeof(FILEINFO));
	_tcscpy_s(fl.path, MAX_PATH, Path);


	wsprintf(buff, _T("%s"), Path);
	_tcscat_s(buff, MAX_PATH, Kind ? Kind : _T("*"));	//��Ŀ¼�����������

	HANDLE FindHandle = FindFirstFile(buff, &fl.data);
	if (FindHandle == INVALID_HANDLE_VALUE)	return false;

	vector<PTCHAR>	vPathSave;
	//��ʼѭ��
	do {
		if (0 == lstrcmp(_T("."), fl.data.cFileName) ||
			0 == lstrcmp(_T(".."), fl.data.cFileName))
			continue;
		if (fl.data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			fl.isDir = true;
		else {
			fl.Size = fl.data.nFileSizeLow;		//��ȡ�ļ���С
			fl.Size |= (((__int64)fl.data.nFileSizeHigh) << 32);
		}
		wsprintf(buff, _T("%s\t%-22s\t���ļ���\t%s\n"), fl.isDir ?
			_T("�ļ���") : _T("�ļ�"), fl.data.cFileName, fl.path);
		OutputDebugString(buff);
		//��ص������ص�
		CALLBACKPROC(&fl);
		if (fl.isDir) {
			wsprintf(buff, _T("%s%s\\"), fl.path, fl.data.cFileName);
			EnumFilePaths(CALLBACKPROC, buff, heap);
		}
	} while (FindNextFile(FindHandle, &fl.data));
	FindClose(FindHandle);
	heap->HeapFree(buff);
	return TRUE;
}
#pragma endregion


