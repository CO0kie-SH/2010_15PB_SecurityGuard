#include "pch.h"
#include "CApi.h"


#pragma region 导出函数段
/*
	函数：创建进程，获取文件Md5
	返回：获取成功时返回用时毫秒数，否则返回0
	修改：CO0kie丶
	时间：2020-10-29_21-35
	原作者：osc_ma60u6gg
	原时间：2018/02/28 13:28
	参考源：https://my.oschina.net/u/4401856/blog/4245706
*/
DWORD GetMd5_ByCertutil(char* pPath, char* md5)
{
	if (!PathFileExistsA(pPath))	return false;
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
	STARTUPINFOA si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.wShowWindow = SW_HIDE;
	//si.hStdInput = hPipeInputRead;
	si.hStdOutput = hPipeOutputWrite;
	si.hStdError = hPipeOutputWrite;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	ULONGLONG ullsysTime = GetTickCount64();
	char buff[MAX_PATH];		//初始化字符串
	if (wsprintfA(buff,			//格式化命令行
		"\"certutil\"  -hashfile  \"%s\" \"md5\"",
		pPath) > 250)			//如果存在长路径或路径攻击
		return false;			//则返回失败
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
	DWORD len = 0;							//初始化返回值长
	GetExitCodeProcess(pi.hProcess, &len);	//获取进程退出码
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	if (len == 0x00) {
		if (ReadFile(
			hPipeOutputRead,	// handle of the read end of our pipe
			&buff,		// address of buffer that receives data
			260,		// number of bytes to read
			&len,		// address of number of bytes read
			NULL		// non-overlapped.
		) && len) {		//成功读取
			char* lpc = buff;				//初始化字符串头
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
void* GetFilePtr(char* pPath, __int64& pSize, char* str /*= nullptr*/)
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
	qwFileSize |= (((__int64)dwFileSize) << 32);	//左移32位得到高位大小
	if (qwFileSize == 0)		//判断文件大小
		return nullptr;			//为0则返回
	pSize = qwFileSize;			//通过实参返回文件大小

	if (str) {					//如果传入实参str，则保存str
		char buff[33];	buff[32] = '\0';
		PSTR pstr = StrFormatByteSize64A(qwFileSize, buff, 32);
		wsprintfA(str, "%s", pstr);
	}
	HANDLE	hHeap = GetProcessHeap();	//获取默认堆
	LPVOID	lpMem = HeapAlloc(hHeap, HEAP_GENERATE_EXCEPTIONS, qwFileSize);
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
	return lpMem;
}


#pragma endregion


