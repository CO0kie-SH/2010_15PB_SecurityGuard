#include "pch.h"
#include "CApi.h"


#pragma region 导出函数段
/*
	函数：创建进程，获取文件Md5
	返回：成功时返回用时毫秒数，否则返回0
	修改：CO0kie丶
	时间：2020-10-29_21-35
	原作者：osc_ma60u6gg
	原时间：2018/02/28 13:28
	参考源：https://my.oschina.net/u/4401856/blog/4245706
*/
DWORD GetMd5_ByCertutil(PTCHAR pPath, char md5[33])
{
	if (!PathFileExists(pPath))	return false;
	HANDLE hPipeOutputRead = NULL;
	HANDLE hPipeOutputWrite = NULL;

	//创建管道 
	SECURITY_ATTRIBUTES sa = { sizeof(sa) };
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	//为标准输出重定向创建管道
	CreatePipe(&hPipeOutputRead,  // read handle
		&hPipeOutputWrite, // write handle
		&sa,      // security attributes
		0      // number of bytes reserved for pipe - 0 default
	);

	//创建进程 
	//使子进程使用hPipeOutputWrite作为标准输出使用hPipeInputRead作为标准输入，并使子进程在后台运行
	STARTUPINFO si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.wShowWindow = SW_HIDE;
	//si.hStdInput = hPipeInputRead;
	si.hStdOutput = hPipeOutputWrite;
	si.hStdError = hPipeOutputWrite;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	ULONGLONG ullsysTime = GetTickCount64();
	TCHAR buff[MAX_PATH];		//初始化字符串
	if (wsprintf(buff,			//格式化命令行
		_T("\"certutil\"  -hashfile  \"%s\" \"md5\""),
		pPath) > 250)			//如果存在长路径或路径攻击
		return false;			//则返回失败
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
	DWORD len = 0;							//初始化返回值长
	GetExitCodeProcess(pi.hProcess, &len);	//获取进程退出码
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
		) && len) {		//成功读取
			char* lpc = chbuff;				//初始化字符串头
			while ('\n' != *(lpc++));		//取到换行位置
			memcpy(md5, lpc, 32);			//赋值Md5
			md5[32] = '\0';					//截断字符串
			//printf_s("%s", md5);			//计算Md5用时
			ullsysTime = GetTickCount64() - ullsysTime;
		}
		else ullsysTime = 0;
	}
	CloseHandle(hPipeOutputRead);
	CloseHandle(hPipeOutputWrite);
	return (DWORD)ullsysTime;
}


/*
	函数：打开文件，并得到文件指针
	返回：成功返回[进程默认堆]创建的内存指针，否则为nullptr
	作者：CO0kie丶
	时间：2020-10-30_11-35
*/
void* GetFilePtr(char* pPath, __int64& pSize, char* strSize /*= nullptr*/)
{
	if (!PathFileExistsA(pPath))	return nullptr;		//如果文件不存在则返回
	HANDLE handle = CreateFileA(pPath, GENERIC_ALL,		//打开文件句柄
		0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (handle == INVALID_HANDLE_VALUE)
		return nullptr;

	DWORD dwFileSize;
	__int64 qwFileSize = GetFileSize(handle, &dwFileSize);

	if (dwFileSize > 0) {
		OutputDebugStringA("暂时无法解析大文件\n");
		CloseHandle(handle);
		ExitProcess(defExitID无法解析大文件);
	}
	//qwFileSize |= (((__int64)dwFileSize) << 32);	//左移32位得到高位大小
	if (qwFileSize == 0)		//判断文件大小
		return nullptr;			//为0则返回
	pSize = qwFileSize;			//通过实参返回文件大小

	if (strSize) {					//如果传入实参str，则保存str
		char buff[33] = {0};
		PSTR pstr = StrFormatByteSize64A(qwFileSize, buff, 32);
		wsprintfA(strSize, "%s", pstr);
	}
	HANDLE	hHeap = GetProcessHeap();	//获取默认堆
	LPVOID	lpMem = HeapAlloc(hHeap, 0, (DWORD)qwFileSize);
	if (!lpMem) {
		MessageBoxA(0, "分配堆空间失败\n", 0, 0);
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
	函数：获取CPU状态
	作者：Github：havocykp
*/
float GetCpuUsage()
{
	//		    空闲事件  内核事件   用户事件
	_FILETIME  idleTime, kernelTime, userTime;
	//获取时间
	GetSystemTimes(&idleTime, &kernelTime, &userTime);
	//等待1000毫秒
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!hEvent)	return 0.0f;
	WaitForSingleObject(hEvent, 1000);
	//获取新的时间
	_FILETIME  newIdleTime, newKernelTime, newUserTime;
	GetSystemTimes(&newIdleTime, &newKernelTime, &newUserTime);
	//将各个时间转换
	double dOldIdleTime = FILETIME2Double(idleTime);
	double dNewIdleTime = FILETIME2Double(newIdleTime);
	double dOldKernelTime = FILETIME2Double(kernelTime);
	double dNewKernelTime = FILETIME2Double(newKernelTime);
	double dOldUserTime = FILETIME2Double(userTime);
	double dNewUserTime = FILETIME2Double(newUserTime);
	//计算出使用率
	return  float(100.0 - (dNewIdleTime - dOldIdleTime) /
		(dNewKernelTime - dOldKernelTime + dNewUserTime - dOldUserTime) * 100.0);
}



/*
	函数：提升权限
	链接：https://blog.csdn.net/paschen/article/details/52829867
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
	函数：枚举文件回调
	时间：2020-11-03_23-20
*/
BOOL CALLBACK EnumFilePaths(void(*CALLBACKPROC)(LPFILEINFO),
	PTCHAR Path, CHeap* heap, PTCHAR Kind /*= nullptr*/)
{
	if (nullptr == Path)		return false;		//如果路径不存在则返回
	//if (!PathFileExists(Path))	return false;		//如果文件不存在则返回
	if (!PathIsDirectory(Path))	return false;		//如果目录不存在则返回
	if (PathIsDirectoryEmpty(Path))	return false;	//如果空目录则返回


	//CHeap cHeap;		//定义一个堆控制器
	PTCHAR buff = (PTCHAR)heap->HeapAlloc(
		sizeof(TCHAR) * MAX_PATH);
	if (!buff)	return false;

	//初始化文件信息
	FILEINFO fl;
	ZeroMemory(&fl, sizeof(FILEINFO));
	_tcscpy_s(fl.path, MAX_PATH, Path);


	wsprintf(buff, _T("%s"), Path);
	_tcscat_s(buff, MAX_PATH, Kind ? Kind : _T("*"));	//向目录添加搜索条件

	HANDLE FindHandle = FindFirstFile(buff, &fl.data);
	if (FindHandle == INVALID_HANDLE_VALUE)	return false;

	vector<PTCHAR>	vPathSave;
	//开始循环
	do {
		if (0 == lstrcmp(_T("."), fl.data.cFileName) ||
			0 == lstrcmp(_T(".."), fl.data.cFileName))
			continue;
		if (fl.data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			fl.isDir = true;
		else {
			fl.Size = fl.data.nFileSizeLow;		//获取文件大小
			fl.Size |= (((__int64)fl.data.nFileSizeHigh) << 32);
		}
		wsprintf(buff, _T("%s\t%-22s\t父文件夹\t%s\n"), fl.isDir ?
			_T("文件夹") : _T("文件"), fl.data.cFileName, fl.path);
		OutputDebugString(buff);
		//向回调函数回调
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


