#include "pch.h"
#include "CPE.h"
#include <stdio.h>


#pragma region ���ں�����
CPE::CPE(char* pFile, ULONGLONG fileSize)
	:_pNt(nullptr), FOA(0), is32o64(0)
{
	this->PEHead_Info.x64ImageBase = 0;
	if (isPE(pFile)) {
		this->FOA = (SIZE_T)pFile;
	}
}

CPE::~CPE()
{
}

/*
	��������ȡ�ж��ļ��Ƿ�ΪPE�ļ�
	���أ�����TRUE��FALSE
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-30_15-20
*/
BOOL CPE::isPE(char* lpImage)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpImage;
	if (pDos == nullptr) {
		OutputDebugStringA("���Dosͷʧ��\n");
		return false;
	}
	if (pDos->e_magic == IMAGE_DOS_SIGNATURE)
	{
		OutputDebugStringA("��⵽Dosͷ��ָ��PEƫ��\n");
		PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + lpImage);
		if (pNt->Signature == IMAGE_NT_SIGNATURE)
		{
			_pNt = pNt;
			return true;
		}
	}
	return false;
}
/*
	��������ȡPEͷ����Ϣ
	���أ�����TRUE��FALSE
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-30_17-21
*/
BOOL CPE::GetNTHeadInfo(bool isPrint)
{
	if (FOA == 0)	return false;		//���FOAΪ0��ʧ��
	if (this->PEHead_Info.x64ImageBase)	//��0ֵ��ʾ�Լ���ȡ����
		return true;		
	//�ṹ��[NTͷ]˵��
	/*
typedef struct _IMAGE_NT_HEADERS {
	DWORD Signature;	//ָ��NTͷ��־��0x0000 4550 [PE..]
						//==��[IMAGE_NT_SIGNATURE]
	IMAGE_FILE_HEADER FileHeader;			//ָ��[�ļ�ͷ]�Ľṹ��[IMAGE_FILE_HEADER]
	IMAGE_OPTIONAL_HEADER32 OptionalHeader;	//ָ��[��չͷ]�Ľṹ��[IMAGE_OPTIONAL_HEADER]
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;
*/
	//�ṹ��[�ļ�ͷ]˵��
	/*
	typedef struct _IMAGE_FILE_HEADER {	//[�ļ�ͷ]�ṹ��
		WORD    Machine;				//�ļ����е�ƽ̨[i386/014C]
		WORD    NumberOfSections;		//����������
		DWORD   TimeDateStamp;			//�ļ�����ʱ��
		DWORD   PointerToSymbolTable;	//���ű�ƫ��
		DWORD   NumberOfSymbols;		//���Ÿ���
		WORD    SizeOfOptionalHeader;	//��չͷ�Ĵ�С[32/00E0][64/00F0]
		WORD    Characteristics;		//PE�ļ�������
	} IMAGE_FILE_HEADER, * PIMAGE_FILE_HEADER;
	*/
	PIMAGE_FILE_HEADER pFileHeader = &_pNt->FileHeader;
	//�ṹ��[PE��Ϣͷ]˵��
	/*
	typedef struct _IMAGE_OPTIONAL_HEADER {
		//
		// Standard fields.
		//

		WORD    Magic;	//���ļ�����[32/010B][64/020B][ROM����/0170]
		BYTE    MajorLinkerVersion;		// ���� ���������汾
		BYTE    MinorLinkerVersion;		// ���� �������ΰ汾
		DWORD   SizeOfCode;				//[����]���д�������(��)���ܴ�С
		DWORD   SizeOfInitializedData;	//[����]�ѳ�ʼ�����ݵ��ܴ�С
		DWORD   SizeOfUninitializedData;	//[����]δ��ʼ�����ݵ��ܴ�С��
										//�ڴ����в�ռ�ÿռ䣬�ڼ��ؽ��ڴ�֮��
										//��Ԥ����ô��Ŀռ䣬һ��洢��.bss������
		DWORD   AddressOfEntryPoint;	//�������ʼִ�е���������(RVA),
									//Ҳ��OEP��Orginal��Entry��Point��Դ��ڵ�
		DWORD   BaseOfCode;			//[����]��ʼ�������������ַ(RVA)
		DWORD   BaseOfData;			//[����]��ʼ���ݵ���������ַ(RVA)

		//
		// NT additional fields.
		//

		DWORD   ImageBase;			//���Ĭ�ϼ��ػ�ַ(���û�м�����Ϊ�ض�λ)
		DWORD   SectionAlignment;	//���ء����������һ��Ϊ0x1000
		DWORD   FileAlignment;		//���ء�PE��������һ��Ϊ0x200
		WORD    MajorOperatingSystemVersion;	// ���� ������ϵͳ�汾��
		WORD    MinorOperatingSystemVersion;	// ���� �β���ϵͳ�汾��
		WORD    MajorImageVersion;			// ���� ��ӳ��汾
		WORD    MinorImageVersion;			// ���� ��ӳ��汾
		WORD    MajorSubsystemVersion;		// ���� ����ϵͳ�汾
		WORD    MinorSubsystemVersion;		// ���� ����ϵͳ�汾
		DWORD   Win32VersionValue;		// ���� ����ֵ һ��Ϊ0
		DWORD   SizeOfImage;			//���ء�Ҫ���ļ����ؽ��ڴ棬
								//����Ҫ���ڴ��С��ע���ǽ����˿����֮��
		DWORD   SizeOfHeaders;		//���ء�����ͷ����С
		DWORD   CheckSum;			// ���� У���
		WORD    Subsystem;			//[����]��ϵͳ
		WORD    DllCharacteristics;	//[����]ָʾDLL�����ı�־
		DWORD   SizeOfStackReserve;	//[����]��ʾ������ջ���ֵ,һ��Ϊ1MB
		DWORD   SizeOfStackCommit;	//[����]��ʾ������ջ��ʼֵ,һ��Ϊ4KB
		DWORD   SizeOfHeapReserve;	//[����]��ʾ�����ж����ֵ,һ��Ϊ1MB
		DWORD   SizeOfHeapCommit;	//[����]��ʾ�����жѳ�ʼֵ
		DWORD   LoaderFlags;
		DWORD   NumberOfRvaAndSizes;	//[����]����Ŀ¼�ĸ�����Ҳ���ǡ�����Ԫ�صĸ���
										//�������Ŀ¼�����Ϊ16
		IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
	} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;
	*/
	PIMAGE_OPTIONAL_HEADER32 pOptiHeader = (PIMAGE_OPTIONAL_HEADER32)
		&_pNt->OptionalHeader;
	if (!pFileHeader || !pOptiHeader)	return false;
	this->is32o64 = pOptiHeader->Magic == 0x10B ? 32 :
		pOptiHeader->Magic == 0x20B ? 64 : 0;
	LPDWORD dwInfos = (LPDWORD)&this->PEHead_Info;
	dwInfos[Idx00��ڵ�]		= pOptiHeader->AddressOfEntryPoint;
	dwInfos[Idx01�����ַ]	= pOptiHeader->ImageBase;
	dwInfos[Idx02�����С]	= pOptiHeader->SizeOfImage;
	dwInfos[Idx03�����ַ]	= pOptiHeader->BaseOfCode;
	dwInfos[Idx04���ݻ�ַ]	= pOptiHeader->BaseOfData;
	dwInfos[Idx05�����]		= pOptiHeader->SectionAlignment;
	dwInfos[Idx06�ļ�����]	= pOptiHeader->FileAlignment;
	dwInfos[Idx07��־��]		= pOptiHeader->Magic;
	dwInfos[Idx08��ϵͳ]		= pOptiHeader->Subsystem;
	dwInfos[Idx09������Ŀ]	= pFileHeader->NumberOfSections;
	dwInfos[Idx10����ʱ���־] = pFileHeader->TimeDateStamp;
	dwInfos[Idx11�ײ���С]	= pOptiHeader->SizeOfHeaders;
	dwInfos[Idx12����ֵ]		= pFileHeader->Characteristics;
	dwInfos[Idx13У���]		= pOptiHeader->CheckSum;
	dwInfos[Idx14��ѡͷ����С] = pFileHeader->SizeOfOptionalHeader;
	dwInfos[Idx15RVA������С] = pOptiHeader->NumberOfRvaAndSizes;
	this->PEHead_Info.x64ImageBase = dwInfos[Idx01�����ַ];
	if (this->is32o64 == 64) {
		PIMAGE_OPTIONAL_HEADER64 pOptiHeader64 = (PIMAGE_OPTIONAL_HEADER64)
			&_pNt->OptionalHeader;
		this->PEHead_Info.x64ImageBase = pOptiHeader64->ImageBase;
		dwInfos[Idx15RVA������С] = pOptiHeader64->NumberOfRvaAndSizes;
	}
	for (BYTE i = 0; i < 16; i++)
	{
		if (i == Idx01�����ַ && this->is32o64 == 64) {
			printf_s("%-16s%0llX\n", this->PEHead_Info.szPEHead[i],
				this->PEHead_Info.x64ImageBase);
		}
		else {
			printf_s("%-16s%08lX\n", this->PEHead_Info.szPEHead[i],
				this->PEHead_Info.dwPEHead[i]);
		}
	}
	if (this->is32o64 == 0) {
		MessageBoxW(0, L"����ı�־�֣����顣", 0, 0);
		return false;
	}
	return true;
}
DWORD CPE::RvaToFoa(DWORD dwRva, bool isPrint /*= false*/)
{
	if (FOA == 0)	return NULL;	//���FOAΪ0��ʧ��
	//�ṹ��[���α�]˵��
	/*
typedef struct _IMAGE_SECTION_HEADER {
	BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];	//����
	union {
			DWORD   PhysicalAddress;	//NULL
			DWORD   VirtualSize;		//2[����]����û�ж���ǰ��ʵ�ʴ�С
	} Misc;
	DWORD   VirtualAddress;			//3�����RVA
	DWORD   SizeOfRawData;			//4����δ�С	+ RVA =��β
	DWORD   PointerToRawData;		//5�����FOA	��FOA-��RVA=Ŀ��FOA-Ŀ��RVA
	DWORD   PointerToRelocations;	//6 û�� �����ض�λ��Ϣ���ļ�ƫ��
	DWORD   PointerToLinenumbers;	//7 û�� COFF�к���Ϣ��ƫ��
	WORD    NumberOfRelocations;	//8 û�� �ض�λ��Ϣ����Ŀ
	WORD    NumberOfLinenumbers;	//9 û�� �к���Ϣ����Ŀ��ֻ��COFF�к�ʱ������
	DWORD   Characteristics;		//10����������ԣ��ɶ�дִ��
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;
*/
	PIMAGE_SECTION_HEADER pHeader = IMAGE_FIRST_SECTION(_pNt);
	//if (dwRva < _pNt->OptionalHeader.SizeOfHeaders)
	//	return dwRva;
	for (DWORD i = 0; i < _pNt->FileHeader.NumberOfSections; ++i)
	{
		DWORD& dwSectionRva = pHeader[i].VirtualAddress;
		DWORD& dwSectionLen = pHeader[i].SizeOfRawData;
		DWORD  dwSectionEnd = dwSectionRva + dwSectionLen;
		DWORD& dwSectionFoa = pHeader[i].PointerToRawData;
		DWORD& dwSectionVSize = pHeader[i].Misc.VirtualSize;
		/*
		Ŀ�꣺FOA=Ŀ��RVA-��RVA+��FOA
		���Σ���FOA-��RVA=Ŀ��FOA-Ŀ��RVA
		ԭ�ͣ�ԭʼVA - Ĭ�ϻ�ַ   =  ��VA -  �»�ַ
		*/
		DWORD dwFOA = dwRva - dwSectionRva + dwSectionFoa;

		printf_s("%lu\tRͷ%-5lX\tV��%5lX\t��%4lX\tβ%lX\t��Foa%4lX\t", i + 1,
			dwSectionRva, dwSectionVSize, dwSectionLen, dwSectionEnd, dwSectionFoa);
		printf_s("��־%lX\t%s\n", pHeader[i].Characteristics, pHeader[i].Name);
	}
	return NULL;
}
#pragma endregion
