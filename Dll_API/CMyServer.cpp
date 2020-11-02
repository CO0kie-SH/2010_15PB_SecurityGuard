#include "pch.h"
#include "CMyServer.h"


#pragma region ���ں���
CMyServer::CMyServer() :_dwHeapAlloc(0)
{
	this->_hHeap = HeapCreate(0, 0, 0);	//����ѿ�����
}

CMyServer::~CMyServer()
{
	HeapDestroy(this->_hHeap);
}


/*
	������ö�ٷ���
	���أ��ɹ�true��ʧ��false
	���ߣ�CO0kieؼ
	ʱ�䣺2020-11-02_14-40
*/
BOOL CMyServer::EnumServices(vector< SERVICEINFO>& serviceInfos, bool bEx /*= true*/)
{
	this->CleanHeap();	serviceInfos.clear();
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	if (NULL == hSCM)  return false;
	
	DWORD dwServiceNum = 0, dwSize = 0, i;

	//ö�ٺ���ʹ�ý���
/*
_In_	SC_ENUM_TYPE	InfoLevel,				//Ҫ���ص�����
_In_	DWORD			dwServiceType,			//Ҫö�ٵķ������͡�
_In_	DWORD			dwServiceState,			//Ҫö�ٵķ���״̬��
_Out_writes_bytes_opt_(cbBufSize)	LPBYTE	lpServices,	//����״̬��Ϣ�Ļ�����
_In_	DWORD			cbBufSize,				//������С
_Out_	LPDWORD			pcbBytesNeeded,			//���շ���ʣ�������Ŀ������ֽ���
_Out_	LPDWORD			lpServicesReturned,		//�ñ������շ��صķ�����Ŀ��
_Inout_opt_	PDWORD	lpResumeHandle,			//ָ�������ָ��
_In_opt_	LPCWSTR		pszGroupName		//����˳��������
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
	//�ṹ��[����][ENUM_SERVICE_STATUS_PROCESSW]˵��
	/*
typedef struct _ENUM_SERVICE_STATUS_PROCESSW {
	LPWSTR                    lpServiceName;			//��������
	LPWSTR                    lpDisplayName;			//������Ƴ���
	SERVICE_STATUS_PROCESS    ServiceStatusProcess;		//[����״̬��Ϣ]�ṹ��
} ENUM_SERVICE_STATUS_PROCESSW, *LPENUM_SERVICE_STATUS_PROCESSW;
	*/
	LPENUM_SERVICE_STATUS_PROCESS pEnumServices =	//�����ʵ��ķ�����Ϣ�ռ�
		(LPENUM_SERVICE_STATUS_PROCESS)HeapAlloc(this->_hHeap, 0, dwSize);
	if (!pEnumServices)	return false;				//����ռ�ʧ��


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

			if (bEx) {											//��ȡ������Ϣ
				PTCHAR& ptrS = pEnumServices[i].lpServiceName;	//�����ַ���
				len = (DWORD)(_tcslen(ptrS) + 1);				//��ȡ�ַ�������
				len *= sizeof(TCHAR);

				tmp.lpServiceName = (PTCHAR)HeapAlloc(
					this->_hHeap, HEAP_ZERO_MEMORY, len);		//����ѿռ�
				if (tmp.lpServiceName) {
					++this->_dwHeapAlloc;
					memcpy_s(tmp.lpServiceName, len, ptrS, len);//��ʽ���ַ���
				}


				PTCHAR& ptrD = pEnumServices[i].lpDisplayName;	//�����ַ���
				len = (DWORD)(_tcslen(ptrD) + 1);				//��ȡ�ַ�������
				len *= sizeof(TCHAR);

				tmp.lpDisplayName = (PTCHAR)HeapAlloc(
					this->_hHeap, HEAP_ZERO_MEMORY, len);		//����ѿռ�
				if (tmp.lpDisplayName) {
					++this->_dwHeapAlloc;
					memcpy_s(tmp.lpDisplayName, len, ptrD, len);//��ʽ���ַ���
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
	������չ��������Ϣ
	���ߣ�CO0kieؼ
	ʱ�䣺2020-11-02_16-30
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


