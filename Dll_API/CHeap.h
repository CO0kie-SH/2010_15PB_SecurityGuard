#pragma once
#include "CInclude.h"


class DLLexp CHeap
{
public:
	CHeap();
	~CHeap();

	LPVOID HeapAlloc(SIZE_T size);
	bool HeapAlloc(LPVOID& save, SIZE_T size, bool isZERO = true);
	bool HeapFree(LPVOID save);
private:
	HANDLE	mHeap;
};
