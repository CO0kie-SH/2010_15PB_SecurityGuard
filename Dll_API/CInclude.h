#pragma once

#define DLLexp	_declspec(dllexport)
#define DLLexpC	 "C" _declspec(dllexport)

#include <Shlwapi.h>
#include <TlHelp32.h>
#include <tchar.h>


#include <vector>
using std::vector;