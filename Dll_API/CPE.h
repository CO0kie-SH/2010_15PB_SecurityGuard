#pragma once
#include "CInclude.h"

#define		gdefPEFunctions_MAX		0x0B
#define		gdefNTHeadInfos_MAX		0x10
#define		gdefZONEInfos_MAX		0x07
#define		gdefTableInfos_MAX		0x10

enum _EnumPE������
{
	gdsz_PE�ļ�ͷ,
	gdsz_PE������Ϣ,
	gdsz_PEĿ¼��Ϣ,
	gdsz_PE�������Ϣ,
	gdsz_PE��������Ϣ
};



constexpr	PTCHAR	gszPEFunctions[] = {
	(PTCHAR)_T("PE�ļ�ͷ"),
	(PTCHAR)_T("������Ϣ"),
	(PTCHAR)_T("Ŀ¼��Ϣ"),
	(PTCHAR)_T("�� ��  ��"),
	(PTCHAR)_T("�� ��  ��"),
	(PTCHAR)_T("�� Դ  ��"),
	(PTCHAR)_T("�ض�λ��"),
	(PTCHAR)_T("�� ��  ��"),
	(PTCHAR)_T("T L S ��"),
	(PTCHAR)_T("��ַת��"),
	(PTCHAR)_T("ʱ��ת��")
};

constexpr	PTCHAR gszNTHeadInfos[]	= {
	(PTCHAR)_T("��ڵ㣺"),
	(PTCHAR)_T("�����ַ��"),
	(PTCHAR)_T("�����С��"),
	(PTCHAR)_T("�����ַ��"),
	(PTCHAR)_T("���ݻ�ַ��"),
	(PTCHAR)_T("����룺"),
	(PTCHAR)_T("�ļ����룺"),
	(PTCHAR)_T("��־�֣�"),
	(PTCHAR)_T("��ϵͳ��"),
	(PTCHAR)_T("������Ŀ��"),
	(PTCHAR)_T("����ʱ���־��"),
	(PTCHAR)_T("�ײ���С��"),
	(PTCHAR)_T("����ֵ��"),
	(PTCHAR)_T("У��ͣ�"),
	(PTCHAR)_T("��ѡͷ����С��"),
	(PTCHAR)_T("RVA������С��")
};

constexpr	PTCHAR	gszZONEInfos[] = {
	(PTCHAR)_T("������"),
	(PTCHAR)_T("��ʼRVA"),
	(PTCHAR)_T("���δ�С"),
	(PTCHAR)_T("RVAβ��"),
	(PTCHAR)_T("����FOA"),
	(PTCHAR)_T("VOffset"),
	(PTCHAR)_T("��־")
};


constexpr	PTCHAR	gszTablesInfos[] = {
	(PTCHAR)_T("�����"),
	(PTCHAR)_T("�����"),
	(PTCHAR)_T("��Դ"),
	(PTCHAR)_T("����"),
	(PTCHAR)_T("��ȫ"),
	(PTCHAR)_T("�ض�λ"),
	(PTCHAR)_T("����"),
	(PTCHAR)_T("��Ȩ"),
	(PTCHAR)_T("ȫ��ָ��"),
	(PTCHAR)_T("TLS��"),
	(PTCHAR)_T("��������"),
	(PTCHAR)_T("���뷶Χ"),
	(PTCHAR)_T("IAT"),
	(PTCHAR)_T("�ӳ�����"),
	(PTCHAR)_T("COM"),
	(PTCHAR)_T("����")
};

enum IDXPEINFO
{
	Idx00��ڵ� = 0,
	Idx01�����ַ = 1,
	Idx02�����С = 2,
	Idx03�����ַ = 3,
	Idx04���ݻ�ַ = 4,
	Idx05����� = 5,
	Idx06�ļ����� = 6,
	Idx07��־�� = 7,
	Idx08��ϵͳ = 8,
	Idx09������Ŀ = 9,
	Idx10����ʱ���־ = 10,
	Idx11�ײ���С = 11,
	Idx12����ֵ = 12,
	Idx13У��� = 13,
	Idx14��ѡͷ����С = 14,
	Idx15RVA������С = 15,
};


typedef struct _NTHead_INFO
{
	DWORD	dwPEHead[16];
	//char* szPEHead[16] = {}
	ULONGLONG   x64ImageBase;
}NTHead_INFO, * LPNTHead_INFO;

typedef struct _ZONE_INFO
{
	DWORD	dwNums;
	DWORD	dwSectionRva;
	DWORD	dwSectionLen;
	DWORD	dwSectionEnd;
	DWORD	dwSectionFoa;
	DWORD	dwSectionVSize;
	DWORD	dwCharacteristics;
	WCHAR	chName[9];
}ZONE_INFO, * LPZONE_INFO;


typedef struct _PETables_INFO
{
	DWORD	dwRVA	[gdefTableInfos_MAX];
	DWORD	dwSize	[gdefTableInfos_MAX];
}PETables_INFO, * LPPETables_INFO;



typedef struct _PEExport_INFO
{
	DWORD	dwIndex;
	DWORD	dwRVA;
	DWORD	dwFOA;
	char*	pApiName;
}PEExport_INFO, * LPPEExport_INFO;


typedef struct _PEExportTable
{
	DWORD	dwRVA;
	DWORD	dwNums;
	DWORD	dwNumsName;
	char*	pDLLName;
}PEExportTable, * LPPEExportTable;



class DLLexp CPE
{
public:
	CPE();
	~CPE();
	BOOL	Init(char* pFilePath);
	BOOL	Init(PTCHAR pFilePath);
	BOOL	IsPE(char* lpImage);
	BOOL	GetNTHeadInfo();
	DWORD	RvaToFoa(DWORD dwRva, bool isPrint = false,
		vector<ZONE_INFO>* zoneInfos = nullptr);


	BOOL	GetTableInfo();
	BOOL	GetExportInfo(vector<PEExport_INFO>& exportInfos);
	//PTCHAR	GetNTHeadTCHAR(BYTE i);
public:
	ULONGLONG		FOA;
	BYTE			is32o64;
	NTHead_INFO		NTHead_Info;
	ZONE_INFO		ZONE_Info;
	PETables_INFO	Table_Info;
	PEExportTable	ExportTable;
private:
	char*				_pFile = nullptr;
	__int64				_FileSize = 0;
	PIMAGE_NT_HEADERS	_pNt = nullptr;
};