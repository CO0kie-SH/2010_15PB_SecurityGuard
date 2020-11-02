#include "pch.h"
#include "CMyServer.h"


#pragma region 类内函数
CMyServer::CMyServer() :_dwHeapAlloc(0)
{
	this->_hHeap = HeapCreate(0, 0, 0);	//申请堆控制器
}

CMyServer::~CMyServer()
{
	HeapDestroy(this->_hHeap);
}


/*
	函数：枚举服务
	返回：成功true，失败false
	作者：CO0kie丶
	时间：2020-11-02_14-40
*/
BOOL CMyServer::EnumServices(vector< SERVICEINFO>& serviceInfos, bool bEx /*= true*/)
{
	this->CleanHeap();	serviceInfos.clear();
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (NULL == hSCM)  return false;
	
	DWORD dwServiceNum = 0, dwSize = 0, i;

	//枚举函数使用介绍
/*
_In_	SC_ENUM_TYPE	InfoLevel,				//要返回的属性
_In_	DWORD			dwServiceType,			//要枚举的服务类型。
_In_	DWORD			dwServiceState,			//要枚举的服务状态。
_Out_writes_bytes_opt_(cbBufSize)	LPBYTE	lpServices,	//接收状态信息的缓冲区
_In_	DWORD			cbBufSize,				//冲区大小
_Out_	LPDWORD			pcbBytesNeeded,			//接收返回剩余服务条目所需的字节数
_Out_	LPDWORD			lpServicesReturned,		//该变量接收返回的服务条目数
_Inout_opt_	PDWORD	lpResumeHandle,			//指向变量的指针
_In_opt_	LPCWSTR		pszGroupName		//加载顺序组名称
*/
	bool bRet = EnumServicesStatusEx(hSCM,
		SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32,
		SERVICE_STATE_ALL,
		NULL,
		0,
		&dwSize,
		&dwServiceNum,
		NULL, NULL
	);
	//结构体[服务][ENUM_SERVICE_STATUS_PROCESSW]说明
	/*
typedef struct _ENUM_SERVICE_STATUS_PROCESSW {
	LPWSTR                    lpServiceName;			//服务名称
	LPWSTR                    lpDisplayName;			//服务控制程序
	SERVICE_STATUS_PROCESS    ServiceStatusProcess;		//[服务状态信息]结构体
} ENUM_SERVICE_STATUS_PROCESSW, *LPENUM_SERVICE_STATUS_PROCESSW;
	*/
	LPENUM_SERVICE_STATUS_PROCESS pEnumServices =	//申请适当的服务信息空间
		(LPENUM_SERVICE_STATUS_PROCESS)HeapAlloc(this->_hHeap, 0, dwSize);
	if (!pEnumServices)	return false;				//申请空间失败


	bRet = EnumServicesStatusEx(hSCM, SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32, SERVICE_STATE_ALL, (PBYTE)(pEnumServices),
		dwSize, &i, &dwServiceNum, NULL, NULL
	);

	if (bRet) {
		SERVICEINFO tmp;
		DWORD len = 0;
		do{
			SERVICE_STATUS_PROCESS& svStatu = pEnumServices[i].ServiceStatusProcess;
			ZeroMemory(&tmp, sizeof(tmp));
			CopyMemory(&tmp, &svStatu, sizeof(svStatu));
			tmp.PID = svStatu.dwProcessId;

			if (bEx) {											//获取额外信息
				PTCHAR& ptrS = pEnumServices[i].lpServiceName;	//引用字符串
				len = (DWORD)(_tcslen(ptrS) + 1);				//获取字符串长度
				len *= sizeof(TCHAR);

				tmp.lpServiceName = (PTCHAR)HeapAlloc(
					this->_hHeap, HEAP_ZERO_MEMORY, len);		//申请堆空间
				if (tmp.lpServiceName) {
					++this->_dwHeapAlloc;
					memcpy_s(tmp.lpServiceName, len, ptrS, len);//格式化字符串
				}


				PTCHAR& ptrD = pEnumServices[i].lpDisplayName;	//引用字符串
				len = (DWORD)(_tcslen(ptrD) + 1);				//获取字符串长度
				len *= sizeof(TCHAR);

				tmp.lpDisplayName = (PTCHAR)HeapAlloc(
					this->_hHeap, HEAP_ZERO_MEMORY, len);		//申请堆空间
				if (tmp.lpDisplayName) {
					++this->_dwHeapAlloc;
					memcpy_s(tmp.lpDisplayName, len, ptrD, len);//格式化字符串
				}

				this->GetServiceString(&tmp);
			}
			serviceInfos.push_back(tmp);
		} while (++i < dwServiceNum);
	}
	CloseServiceHandle(hSCM);
	HeapFree(this->_hHeap, 0, pEnumServices);
	return !serviceInfos.empty();
}


/*
	函数：展开服务信息
	作者：CO0kie丶
	时间：2020-11-02_16-30
*/
void CMyServer::GetServiceString(LPSERVICEINFO serviceInfo)
{
	DWORD& dwServiceType = serviceInfo->ServiceStatus[0];
	DWORD& dwCurrentState = serviceInfo->ServiceStatus[1];

	switch (dwServiceType)
	{
	case SERVICE_FILE_SYSTEM_DRIVER:
		serviceInfo->pServiceType = gszServiceType[0];
		break;
	case SERVICE_KERNEL_DRIVER:
		serviceInfo->pServiceType = gszServiceType[1];
		break;
	case SERVICE_WIN32_OWN_PROCESS:
		serviceInfo->pServiceType = gszServiceType[2];
		break;
	case SERVICE_WIN32_SHARE_PROCESS:
		serviceInfo->pServiceType = gszServiceType[3];
		break;
	default:
		serviceInfo->pServiceType = nullptr;
		break;
	}

	switch (dwCurrentState)
	{
	case SERVICE_CONTINUE_PENDING:
		serviceInfo->pCurrentState = gszCurrentState[0];
		break;
	case SERVICE_PAUSE_PENDING:
		serviceInfo->pCurrentState = gszCurrentState[1];
		break;
	case SERVICE_PAUSED:
		serviceInfo->pCurrentState = gszCurrentState[2];
		break;
	case SERVICE_RUNNING:
		serviceInfo->pCurrentState = gszCurrentState[3];
		break;
	case SERVICE_START_PENDING:
		serviceInfo->pCurrentState = gszCurrentState[4];
		break;
	case SERVICE_STOP_PENDING:
		serviceInfo->pCurrentState = gszCurrentState[5];
		break;
	case SERVICE_STOPPED:
		serviceInfo->pCurrentState = gszCurrentState[6];
		break;
	default:
		serviceInfo->pCurrentState = nullptr;
		break;
	}
}


//https://www.cnblogs.com/cposture/p/4717546.html
BOOL CMyServer::StartServiceMy(const LPSERVICEINFO serviceInfo)
{
	
	return FALSE;
}
BOOL CMyServer::OpenServiceMy(const LPSERVICEINFO serviceInfo)
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (NULL == hSCM)  return false;
	SC_HANDLE hService = OpenService(hSCM,
		serviceInfo->lpServiceName, SERVICE_PAUSE_CONTINUE);
	bool bRet = hService != NULL;
	if (bRet) {
		SERVICE_STATUS ServiceStatus = { 0 };
		bRet = ControlService(hService, SERVICE_CONTROL_CONTINUE, &ServiceStatus);
		CloseServiceHandle(hService);
		if (!bRet) {
			hService = OpenService(hSCM,
				serviceInfo->lpServiceName, SERVICE_START);
			bRet = StartService(hService, 0, NULL);
			CloseServiceHandle(hService);
		}
	}
	CloseServiceHandle(hSCM);
	return bRet;
}
BOOL CMyServer::StopService(const LPSERVICEINFO serviceInfo)
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (NULL == hSCM)  return false;
	SC_HANDLE hService = OpenService(hSCM,
		serviceInfo->lpServiceName, SERVICE_STOP);
	bool bRet = hService != NULL;
	if (bRet) {
		SERVICE_STATUS ServiceStatus = { 0 };
		bRet = ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);
		CloseServiceHandle(hService);
	}
	CloseServiceHandle(hSCM);
	return bRet;
}
#pragma endregion


