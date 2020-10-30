#pragma once

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

typedef struct _PEHead_INFO
{
	DWORD	dwPEHead[16] = { 0 };
	char* szPEHead[16] = {
		defszPE_00��ڵ�,
		defszPE_01�����ַ,
		defszPE_02�����С,
		defszPE_03�����ַ,
		defszPE_04���ݻ�ַ,
		defszPE_05�����,
		defszPE_06�ļ�����,
		defszPE_07��־��,
		defszPE_08��ϵͳ,
		defszPE_09������Ŀ,
		defszPE_10����ʱ���־,
		defszPE_11�ײ���С,
		defszPE_12����ֵ,
		defszPE_13У���,
		defszPE_14��ѡͷ����С,
		defszPE_15RVA������С
	};
	ULONGLONG   x64ImageBase;
}PEHead_INFO, * LPPEHead_INFO;

class _declspec(dllexport) CPE
{
public:
	CPE(char* pFile, ULONGLONG fileSize) ;
	~CPE();

	BOOL	isPE(char* lpImage);
	BOOL	GetNTHeadInfo(bool isPrint);
	DWORD	RvaToFoa(DWORD dwRva, bool isPrint = false);
public:
	SIZE_T			FOA;
	WORD			is32o64;
	PEHead_INFO		PEHead_Info;
private:
	PIMAGE_NT_HEADERS	_pNt;
};