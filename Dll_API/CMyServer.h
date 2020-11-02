#pragma once
#include "CInclude.h"

#define		gdefSVFunctions_MAX		0x01


enum _Enum��������
{
	gdsz_ö�ٷ��� = 0x00,
	gdsz_�������� = 0x01,
	gdsz_�رշ��� = 0x02
};

constexpr	PTCHAR	gszSVFunctions[] = {
	(PTCHAR)_T("ö�ٷ���"),
	(PTCHAR)_T("��������"),
	(PTCHAR)_T("�رշ���")
};

constexpr	PTCHAR	gszServiceType[] = {
	(PTCHAR)_T("�÷������ļ�ϵͳ��������"),
	(PTCHAR)_T("�÷������豸��������"),
	(PTCHAR)_T("�÷��������Լ��Ľ��������С�"),
	(PTCHAR)_T("�÷���������������һ�����̡�")
};

constexpr	PTCHAR	gszCurrentState[] = {
	(PTCHAR)_T("������"),
	(PTCHAR)_T("������ͣ"),
	(PTCHAR)_T("����ͣ"),
	(PTCHAR)_T("��������"),
	(PTCHAR)_T("��������"),
	(PTCHAR)_T("����ֹͣ"),
	(PTCHAR)_T("��ֹͣ")
};


typedef struct _SERVICEINFO {	//�ṹ��[������Ϣ]
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
	HANDLE	_hHeap;		//����ѿ�����
	DWORD	_dwHeapAlloc;
};

