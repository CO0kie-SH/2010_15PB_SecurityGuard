#include "pch.h"
#include "CHeap.h"


CHeap::CHeap()
{
	mHeap = HeapCreate(0, 0, 0);
}

CHeap::~CHeap()
{
	HeapDestroy(mHeap);
}


LPVOID CHeap::HeapAlloc(SIZE_T size)
{
	return ::HeapAlloc(mHeap, 0, size);
}

bool CHeap::HeapAlloc(LPVOID& save, SIZE_T size, bool isZERO)
{
	LPVOID p = ::HeapAlloc(mHeap, isZERO ? HEAP_ZERO_MEMORY : 0, size);
	if (p)	save = p;
	return p;
}


bool CHeap::HeapFree(LPVOID save)
{
	return ::HeapFree(mHeap, 0, save);
}
