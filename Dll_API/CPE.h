#pragma once
#include "CInclude.h"

#define		gdefPEFunctions_MAX		0x0B
#define		gdefNTHeadInfos_MAX		0x10

enum _EnumPE������
{
	gdsz_PE�ļ�ͷ,
	gdsz_PE������Ϣ
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


/*
#define	defszPE_00��ڵ�			(char*)"��ڵ㣺"
#define	defszPE_01�����ַ		(char*)"�����ַ��"
#define	defszPE_02�����С		(char*)"�����С��"
#define	defszPE_03�����ַ		(char*)"�����ַ��"
#define	defszPE_04���ݻ�ַ		(char*)"���ݻ�ַ��"
#define	defszPE_05�����			(char*)"����룺"
#define	defszPE_06�ļ�����		(char*)"�ļ����룺"
#define	defszPE_07��־��			(char*)"��־�֣�"
#define	defszPE_08��ϵͳ			(char*)"��ϵͳ��"
#define	defszPE_09������Ŀ		(char*)"������Ŀ��"
#define	defszPE_10����ʱ���־	(char*)"����ʱ���־��"
#define	defszPE_11�ײ���С		(char*)"�ײ���С��"
#define	defszPE_12����ֵ			(char*)"����ֵ��"
#define	defszPE_13У���			(char*)"У��ͣ�"
#define	defszPE_14��ѡͷ����С	(char*)"��ѡͷ����С��"
#define	defszPE_15RVA������С		(char*)"RVA������С��"
*/

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

class DLLexp CPE
{
public:
	CPE();
	~CPE();
	BOOL	Init(char* pFilePath);
	BOOL	Init(PTCHAR pFilePath);
	BOOL	IsPE(char* lpImage);
	BOOL	GetNTHeadInfo();
	DWORD	RvaToFoa(DWORD dwRva, bool isPrint = false);
	//PTCHAR	GetNTHeadTCHAR(BYTE i);
public:
	ULONGLONG		FOA;
	BYTE			is32o64;
	NTHead_INFO		NTHead_Info;
private:
	char*				_pFile = nullptr;
	__int64				_FileSize = 0;
	PIMAGE_NT_HEADERS	_pNt = nullptr;
};