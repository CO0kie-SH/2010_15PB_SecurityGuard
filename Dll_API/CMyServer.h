#pragma once
#include "CInclude.h"

#define		gdefSVFunctions_MAX		0x01


enum _Enum服务功能区
{
	gdsz_枚举服务 = 0x00,
	gdsz_启动服务 = 0x01,
	gdsz_关闭服务 = 0x02
};

constexpr	PTCHAR	gszSVFunctions[] = {
	(PTCHAR)_T("枚举服务"),
	(PTCHAR)_T("启动服务"),
	(PTCHAR)_T("关闭服务")
};

constexpr	PTCHAR	gszServiceType[] = {
	(PTCHAR)_T("该服务是文件系统驱动程序。"),
	(PTCHAR)_T("该服务是设备驱动程序。"),
	(PTCHAR)_T("该服务在其自己的进程中运行。"),
	(PTCHAR)_T("该服务与其他服务共享一个过程。")
};

constexpr	PTCHAR	gszCurrentState[] = {
	(PTCHAR)_T("将继续"),
	(PTCHAR)_T("正在暂停"),
	(PTCHAR)_T("已暂停"),
	(PTCHAR)_T("正在运行"),
	(PTCHAR)_T("正在启动"),
	(PTCHAR)_T("正在停止"),
	(PTCHAR)_T("已停止")
};


typedef struct _SERVICEINFO {	//结构体[服务信息]
	//PTCHAR	lpServiceName;
	//SERVICE_STATUS_PROCESS ServiceStatusProcess;
	DWORD	ServiceStatus[9];
	DWORD	PID;
	PTCHAR	lpServiceName;
	PTCHAR	lpDisplayName;
	PTCHAR	pServiceType;
	PTCHAR	pCurrentState;
}SERVICEINFO, * LPSERVICEINFO;


class DLLexp CMyServer
{
public:
	CMyServer();
	~CMyServer();

	BOOL EnumServices(vector< SERVICEINFO>& serviceInfos, bool bEx = true);
	void GetServiceString(LPSERVICEINFO serviceInfo);
	BOOL StartServiceMy(const LPSERVICEINFO serviceInfo);
	BOOL OpenServiceMy(const LPSERVICEINFO serviceInfo);
	BOOL StopService(const LPSERVICEINFO serviceInfo);


	void CleanHeap() {
		if (this->_dwHeapAlloc) {
			HeapDestroy(this->_hHeap);
			this->_hHeap = HeapCreate(0, 0, 0);
			this->_dwHeapAlloc = 0;
		}
	};
private:
	HANDLE	_hHeap;		//申请堆控制器
	DWORD	_dwHeapAlloc;
};

