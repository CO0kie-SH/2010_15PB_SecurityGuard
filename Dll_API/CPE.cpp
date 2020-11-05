#include "pch.h"
#include "CPE.h"

#include "CApi.h"

#pragma region ���ں�����

CPE::CPE() :FOA(0), is32o64(0)
{
	ZeroMemory(&this->NTHead_Info, sizeof(NTHead_INFO));
	ZeroMemory(&this->ZONE_Info, sizeof(ZONE_INFO));
	ZeroMemory(&this->ExportTable, sizeof(PEExportTable));
}


CPE::~CPE()
{
	if (_pFile)
		HeapFree(GetProcessHeap(), 0, _pFile);
}

BOOL CPE::Init(char* pFilePath)
{
	FOA = 0;	is32o64 = 0;
	ZeroMemory(&this->NTHead_Info, sizeof(NTHead_INFO));
	ZeroMemory(&this->ZONE_Info, sizeof(ZONE_INFO));
	ZeroMemory(&this->ExportTable, sizeof(PEExportTable));
	if (_pFile)
		HeapFree(GetProcessHeap(), 0, _pFile);
	_pFile = (char*)GetFilePtr(pFilePath, this->_FileSize);
	if (_pFile == nullptr)	return false;
	if (IsPE(_pFile))
		return true;
	HeapFree(GetProcessHeap(), 0, _pFile);
	_pFile = nullptr;
	return false;
}

BOOL CPE::Init(PTCHAR pFilePath)
{
	char buff[512];
	size_t len = 0;
	wcstombs_s(&len, buff, pFilePath, 512);
	if (len == 0)
		return false;
	return Init(buff);
}

/*
	��������ȡ�ж��ļ��Ƿ�ΪPE�ļ�
	���أ�����TRUE��FALSE
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-30_15-20
*/
BOOL CPE::IsPE(char* lpImage)
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
			_pNt = pNt;	FOA = (ULONGLONG)lpImage;
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
BOOL CPE::GetNTHeadInfo()
{
	if (FOA == 0)	return false;		//���FOAΪ0��ʧ��
	if (this->NTHead_Info.dwPEHead[0])	//��0ֵ��ʾ�Լ���ȡ����
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
	LPDWORD dwInfos = (LPDWORD)&this->NTHead_Info;
	dwInfos[Idx00��ڵ�] = pOptiHeader->AddressOfEntryPoint;
	dwInfos[Idx01�����ַ] = pOptiHeader->ImageBase;
	dwInfos[Idx02�����С] = pOptiHeader->SizeOfImage;
	dwInfos[Idx03�����ַ] = pOptiHeader->BaseOfCode;
	dwInfos[Idx04���ݻ�ַ] = pOptiHeader->BaseOfData;
	dwInfos[Idx05�����] = pOptiHeader->SectionAlignment;
	dwInfos[Idx06�ļ�����] = pOptiHeader->FileAlignment;
	dwInfos[Idx07��־��] = pOptiHeader->Magic;
	dwInfos[Idx08��ϵͳ] = pOptiHeader->Subsystem;
	dwInfos[Idx09������Ŀ] = pFileHeader->NumberOfSections;
	dwInfos[Idx10����ʱ���־] = pFileHeader->TimeDateStamp;
	dwInfos[Idx11�ײ���С] = pOptiHeader->SizeOfHeaders;
	dwInfos[Idx12����ֵ] = pFileHeader->Characteristics;
	dwInfos[Idx13У���] = pOptiHeader->CheckSum;
	dwInfos[Idx14��ѡͷ����С] = pFileHeader->SizeOfOptionalHeader;
	dwInfos[Idx15RVA������С] = pOptiHeader->NumberOfRvaAndSizes;
	if (this->is32o64 == 64) {
		PIMAGE_OPTIONAL_HEADER64 pOptiHeader64 = (PIMAGE_OPTIONAL_HEADER64)
			&_pNt->OptionalHeader;
		this->NTHead_Info.x64ImageBase = pOptiHeader64->ImageBase;
		dwInfos[Idx15RVA������С] = pOptiHeader64->NumberOfRvaAndSizes;
	}
	if (this->is32o64 == 0) {
		MessageBoxW(0, L"����ı�־�֣����顣", 0, 0);
		return false;
	}
	return true;
}
DWORD CPE::RvaToFoa(DWORD dwRva, bool isPrint /*= false*/,
	vector<ZONE_INFO>* zoneInfos /*= nullptr*/)
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
	if (dwRva > 400 && dwRva < _pNt->OptionalHeader.SizeOfHeaders)
		return dwRva;
	else if (dwRva > 0x05 && dwRva < _pNt->OptionalHeader.SizeOfHeaders)
		return dwRva;
	char buff[MAX_PATH];
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
		if (isPrint) {
			wsprintfA(buff, "%lu\tRͷ%-5lX\tV��%5lX\t��%4lX\tβ%lX\t��Foa%4lX\t",
				i + 1, dwSectionRva, dwSectionVSize,
				dwSectionLen, dwSectionEnd, dwSectionFoa);
			OutputDebugStringA(buff);
			wsprintfA(buff, "��־%lX\t%s\n",
				pHeader[i].Characteristics, pHeader[i].Name);
			OutputDebugStringA(buff);

		}
		if (zoneInfos) {
			wsprintfA(buff, "%s", pHeader[i].Name);

			this->ZONE_Info.dwNums = _pNt->FileHeader.NumberOfSections;
			this->ZONE_Info.dwSectionRva = dwSectionRva;
			this->ZONE_Info.dwSectionLen = dwSectionLen;
			this->ZONE_Info.dwSectionEnd = dwSectionEnd;
			this->ZONE_Info.dwSectionFoa = dwSectionFoa;
			this->ZONE_Info.dwSectionVSize = dwSectionVSize;
			this->ZONE_Info.dwCharacteristics = pHeader[i].Characteristics;
			mbstowcs_s(nullptr, this->ZONE_Info.chName, buff, 8);
			zoneInfos->push_back(this->ZONE_Info);
		}
		if (dwRva >= dwSectionRva && dwRva < dwSectionEnd) {
			return dwFOA;
		}
	}
	return NULL;
}

DWORD CPE::FoaToRva(DWORD dwFoa)
{
	if (FOA == 0)	return false;

	PIMAGE_SECTION_HEADER pHeader = IMAGE_FIRST_SECTION(_pNt);
	if (dwFoa > 0 && dwFoa < _pNt->OptionalHeader.SizeOfHeaders)
		return dwFoa;
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
		//DWORD dwFOA = dwRva - dwSectionRva + dwSectionFoa;
		DWORD dwRva = dwFoa + dwSectionRva - dwSectionFoa;
		if (dwRva >= dwSectionRva && dwRva < dwSectionEnd) {
			return dwRva;
		}

	}

	return 0;
}

/*
	��������ȡĿ¼��
	���أ��ɹ�true��ʧ��false
	���ߣ�CO0kieؼ
	ʱ�䣺2020-11-03_09-00
*/
BOOL CPE::GetTableInfo()
{
	if (FOA == 0)	return false;
	PIMAGE_OPTIONAL_HEADER32 pOption32 = (PIMAGE_OPTIONAL_HEADER32)
		&_pNt->OptionalHeader;
	PIMAGE_OPTIONAL_HEADER64 pOption64 = (PIMAGE_OPTIONAL_HEADER64)
		&_pNt->OptionalHeader;

	PIMAGE_DATA_DIRECTORY pTable = this->is32o64 == 32 ? pOption32->DataDirectory :
		this->is32o64 == 64 ? pOption64->DataDirectory : nullptr;
	if (pTable == nullptr)	return false;


	PDWORD pdwRVA = (PDWORD)&Table_Info.dwRVA;
	PDWORD pdwSize = (PDWORD)&Table_Info.dwSize;
	ZeroMemory(&Table_Info, sizeof(PETables_INFO));


	for (BYTE i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; ++i, ++pTable)
	{
		pdwRVA[i] = pTable->VirtualAddress;
		pdwSize[i] = pTable->Size;
		//Table_Info.chName[i] = gszTablesInfos[i];
		//wsprintf(buff, _T("%d\t%08lX\t"),i, pTable->VirtualAddress);
		//OutputDebugString(buff);
		//wsprintf(buff, _T("%08lX\n"), pTable->Size);
		//OutputDebugString(buff);
	}
	return true;
}

/*
	��������ȡ������
	���أ��ɹ�true��ʧ��false
	���ߣ�CO0kieؼ
	ʱ�䣺2020-11-03_14-00
*/
BOOL CPE::GetExportInfo(vector<PEExport_INFO>& exportInfos)
{
	if (FOA == 0)	return false;
	PIMAGE_OPTIONAL_HEADER32 pOption32 = (PIMAGE_OPTIONAL_HEADER32)
		&_pNt->OptionalHeader;
	PIMAGE_OPTIONAL_HEADER64 pOption64 = (PIMAGE_OPTIONAL_HEADER64)
		&_pNt->OptionalHeader;

	PIMAGE_DATA_DIRECTORY pTable = this->is32o64 == 32 ? pOption32->DataDirectory :
		this->is32o64 == 64 ? pOption64->DataDirectory : nullptr;
	if (pTable == nullptr)	return false;


	DWORD dwExportFOA = RvaToFoa(pTable->VirtualAddress);
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(FOA + dwExportFOA);
	//	������ṹ��
	/*
typedef struct _IMAGE_EXPORT_DIRECTORY {
	DWORD   Characteristics;	//1 û�� Ϊ0
	DWORD   TimeDateStamp;		//2 û�� ���ļ�ͷʱ��һ��
	WORD    MajorVersion;		//3 û�� ���汾��
	WORD    MinorVersion;		//4 û�� �ΰ汾��
	DWORD   Name;				//5[����]ģ��DLL��
	DWORD   Base;				//6[����]��������ʼ���
	DWORD   NumberOfFunctions;	//7�����������
	DWORD   NumberOfNames;		//8�������������
	DWORD   AddressOfFunctions;		//09���������ַ��RVA
	DWORD   AddressOfNames;			//10����������Ʊ�RVA
	DWORD   AddressOfNameOrdinals;	//11���������ű�RVA
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;
*/


	DWORD EatFoa = RvaToFoa(pExport->AddressOfFunctions);
	DWORD EntFoa = RvaToFoa(pExport->AddressOfNames);
	DWORD EotFoa = RvaToFoa(pExport->AddressOfNameOrdinals);
	//����������ַ��
	//�����������Ʊ�
	//����������ű�
	PDWORD	pEat = (PDWORD)(FOA + EatFoa);
	PDWORD	pEnt = (PDWORD)(FOA + EntFoa);
	PWORD	pEot = (PWORD)(FOA + EotFoa);
	DWORD	dwFoa = RvaToFoa(pExport->Name);
	if (dwFoa == 0)	return false;


	this->ExportTable.pDLLName = (char*)FOA + dwFoa;
	//TCHAR buff[MAX_PATH];
	PEExport_INFO tmp;
	for (DWORD i = 0; i < pExport->NumberOfFunctions; i++)
	{
		//ZeroMemory(&tmp, sizeof(PEExport_INFO));
		//wsprintf(buff, _T("\n%lu\t��ű�RVA=0x%lX\t"), i + 1, pEat[i]);
		//OutputDebugString(buff);
		if (pEat[i] == 0)	continue;
		tmp = PEExport_INFO{
			i + 1, pEat[i], RvaToFoa(pEat[i]),(char*)"-"
		};
		//OutputDebugString(_T("���Ʊ�\n"));
		for (DWORD j = 0; j < pExport->NumberOfFunctions; j++)
		{
			//wsprintf(buff, _T("%lu\t���Ʊ�%lu\n"), j + 1, pEot[j]);
			//OutputDebugString(buff);
			if (i == pEot[j]) {
				dwFoa = RvaToFoa(pEnt[j]);
				tmp.pApiName = (char*)FOA + dwFoa;
				break;
			}
		}
		exportInfos.push_back(tmp);
	}
	return !exportInfos.empty();
}


/*
	��������ȡ������
	���أ��ɹ�true��ʧ��false
	���ߣ�CO0kieؼ
	ʱ�䣺2020-11-03_14-00
*/
BOOL CPE::GetImportInfo(vector<PEImport_INFO>& importInfos)
{

	//�ṹ��˵��
	/*
typedef struct _IMAGE_IMPORT_DESCRIPTOR {	//������ṹ��
	union {
		DWORD   Characteristics;
		DWORD   OriginalFirstThunk;		//1���ָ��ṹ�������RVA���ṹ�����
										//�������Ʊ�[INT:Import Name Table]
										//�ṹ��[IMAGE_THUNK_DATA]
	} DUMMYUNIONNAME;
	DWORD   TimeDateStamp;			//����ִ���ļ����뱻�����DLL���а�ʱ,���ֶ�Ϊ0
	DWORD   ForwarderChain;			// -1 if no forwarders	ת�������õ�
									//��һ����ת���api������
	DWORD   Name;					//4[����]�����PE�ļ�������RVA
	DWORD   FirstThunk;				//5���ָ��һ���ṹ�������RVA���ṹ�����
									//�����ַ��[IAT:Import Address Table]
									//��INT���ýṹ��[IMAGE_THUNK_DATA]
									//�ڴ����ļ�����INT��ͬ,���Կ���INT��IAT�ı���
} IMAGE_IMPORT_DESCRIPTOR;
typedef IMAGE_IMPORT_DESCRIPTOR UNALIGNED *PIMAGE_IMPORT_DESCRIPTOR;

//3.����Щ�ļ����������Ʊ��ǿյģ�ȫ�㣬ʲô��û�С�
//	��˵�������ַ����ʱû�б��ݡ����Խ��������ʱ��IAT������

typedef struct _IMAGE_THUNK_DATA32 {
	union {
		DWORD ForwarderString;		//ת��ʱ�õ�
		DWORD Function;				//���뺯���ĵ�ַ���ڼ��ص��ڴ��������
		DWORD Ordinal;				//��������ŵ���ģ��õ�����
		DWORD AddressOfData;		//�����Ǻ���������ģ������
	} u1;							//ָ��һ���ṹ��[IMAGE_IMPORT_BY_NAME]
} IMAGE_THUNK_DATA32;
typedef IMAGE_THUNK_DATA32 * PIMAGE_THUNK_DATA32;

typedef struct _IMAGE_IMPORT_BY_NAME {
	WORD    Hint;
	CHAR   Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;
*/


	if (FOA == 0)	return false;
	PIMAGE_DATA_DIRECTORY dwImportDir =
		(PIMAGE_DATA_DIRECTORY)GetDataDirectory(1);
	if (dwImportDir == nullptr)	return false;

	DWORD dwFOA = RvaToFoa(dwImportDir->VirtualAddress);	//�õ����α�
	PIMAGE_IMPORT_DESCRIPTOR pImportTable =					//��ȡ�����
		(PIMAGE_IMPORT_DESCRIPTOR)(FOA + dwFOA);			//���ڴ��ַ






	dwFOA = RvaToFoa(pImportTable->Name);
	PEImport_INFO tmp;
	DWORD dwNum = 0;
	constexpr char* pNULL = (char*)"-";
	while (pImportTable->Name != 0)
	{
		dwFOA = RvaToFoa(pImportTable->Characteristics);
		tmp = { 0,pImportTable->Characteristics,dwFOA,0,0,pNULL };
		char* pDllName = (char*)FOA + dwFOA;
		tmp.pApiName = pDllName;
		importInfos.push_back(tmp);
		tmp = PEImport_INFO{
				dwNum,
				pImportTable->OriginalFirstThunk,
				RvaToFoa(pImportTable->OriginalFirstThunk),
				pImportTable->FirstThunk,
				0,pNULL
		};

		PIMAGE_THUNK_DATA64 pNameTable64 = NULL;
		PIMAGE_THUNK_DATA32 pNameTable = NULL;
		dwFOA = RvaToFoa(pImportTable->FirstThunk);		//ʹ��IAT����
		pNameTable = (PIMAGE_THUNK_DATA32)(FOA + dwFOA);
		pNameTable64 = (PIMAGE_THUNK_DATA64)(FOA + dwFOA);
		if (this->is32o64 == 32) {
			while (pNameTable->u1.Ordinal != 0)
			{
				tmp.dwIndex = ++dwNum;
				tmp.pApiName = pNULL;
				dwFOA = RvaToFoa(pNameTable->u1.Ordinal);
				if (IMAGE_SNAP_BY_ORDINAL(pNameTable->u1.Ordinal) == 1)
				{
					tmp.Thunk = pNameTable->u1.AddressOfData;
				}
				else
				{
					PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)(FOA + dwFOA);
					tmp.Thunk = pNameTable->u1.Ordinal;
					tmp.wdHint = pName->Hint;
					tmp.pApiName = pName->Name;
				}
				importInfos.push_back(tmp);
				++pNameTable;
				tmp.dwRVA += 4;	tmp.dwFOA += 4;
			}
		}
		else if (this->is32o64 == 64) {
			while (pNameTable64->u1.Ordinal != 0)
			{
				tmp.dwIndex = ++dwNum;
				tmp.pApiName = pNULL;
				dwFOA = RvaToFoa((DWORD)pNameTable64->u1.Ordinal);
				if (IMAGE_SNAP_BY_ORDINAL(pNameTable64->u1.Ordinal) == 1)
				{
					tmp.Thunk = pNameTable64->u1.AddressOfData;
				}
				else
				{
					PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)(FOA + dwFOA);
					tmp.Thunk = pNameTable64->u1.Ordinal;
					tmp.wdHint = pName->Hint;
					tmp.pApiName = pName->Name;
				}
				importInfos.push_back(tmp);
				++pNameTable64;
				tmp.dwRVA += 4;	tmp.dwFOA += 4;
			}
		}
		++pImportTable;
	}
	return !importInfos.empty();
}

BOOL CPE::GetTLSInfo(ULONGLONG Info[6])
{

	if (FOA == 0)	return false;
	PIMAGE_DATA_DIRECTORY dwImportDir =
		(PIMAGE_DATA_DIRECTORY)GetDataDirectory(9);
	if (dwImportDir == nullptr)	return false;

	PIMAGE_TLS_DIRECTORY32 pTls = (PIMAGE_TLS_DIRECTORY32)(
		RvaToFoa(dwImportDir->VirtualAddress)
		+ FOA);
	PIMAGE_TLS_DIRECTORY64 pTls64 = (PIMAGE_TLS_DIRECTORY64)(
		RvaToFoa(dwImportDir->VirtualAddress)
		+ FOA);
	//����TLS����Ϣ
	TCHAR buff[MAX_PATH];
	if (this->is32o64 == 32) {
		wsprintfW(buff, L"%lX\n", pTls->StartAddressOfRawData);
		Info[0] = pTls->StartAddressOfRawData;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%lX\n", pTls->EndAddressOfRawData);
		Info[1] = pTls->EndAddressOfRawData;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%lX\n", pTls->AddressOfIndex);
		Info[2] = pTls->AddressOfIndex;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%lX\n", pTls->AddressOfCallBacks);
		Info[3] = pTls->AddressOfCallBacks;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%lX\n", pTls->Characteristics);
		Info[4] = pTls->SizeOfZeroFill;
		Info[5] = pTls->Characteristics;
		OutputDebugStringW(buff);
	}
	else if (this->is32o64 == 64) {
		wsprintfW(buff, L"%llX\n", pTls->StartAddressOfRawData);
		Info[0] = pTls64->StartAddressOfRawData;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%llX\n", pTls->EndAddressOfRawData);
		Info[1] = pTls64->EndAddressOfRawData;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%llX\n", pTls->AddressOfIndex);
		Info[2] = pTls64->AddressOfIndex;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%llX\n", pTls->AddressOfCallBacks);
		Info[3] = pTls64->AddressOfCallBacks;
		OutputDebugStringW(buff);
		wsprintfW(buff, L"%llX\n", pTls->Characteristics);
		Info[4] = pTls64->SizeOfZeroFill;
		Info[5] = pTls64->Characteristics;
		OutputDebugStringW(buff);
	}

	return 0;
}

/*
	��������ȡPEĿ¼��
	ʱ�䣺11-03_16-05
*/
void* CPE::GetDataDirectory(WORD i)
{
	if (FOA == 0)	return nullptr;
	PIMAGE_OPTIONAL_HEADER32 pOption32 = (PIMAGE_OPTIONAL_HEADER32)
		&_pNt->OptionalHeader;
	PIMAGE_OPTIONAL_HEADER64 pOption64 = (PIMAGE_OPTIONAL_HEADER64)
		&_pNt->OptionalHeader;

	PIMAGE_DATA_DIRECTORY pTable = this->is32o64 == 32 ? pOption32->DataDirectory :
		this->is32o64 == 64 ? pOption64->DataDirectory : nullptr;
	if (pTable == nullptr)	return nullptr;

	pTable += i;
	return (char*)pTable;
}
#pragma endregion
