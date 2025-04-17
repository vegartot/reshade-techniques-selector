
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define NOCOMM

#pragma warning(push, 3)
#include <windows.h>
#include <ShObjIdl.h>
#include <Shlwapi.h>
#pragma warning(pop)

#define MAX_READ_BYTES 8196
#define TECHNIQUE_MAX_BYTES 128
#define TOKEN "Techniques="
#define OUTFILE L"reshade-techniques.ini"

#include "utils.h"

typedef struct NODE
{
	struct NODE *pNext;
	char buffer[TECHNIQUE_MAX_BYTES];
}NODE;


int __stdcall main(void);
void WINAPI WinMainCRTStartup(void);
HRESULT getItems(IShellItemArray** pItemList, LPWSTR *folderPath);
int pushIfNew(NODE** stackObject, char* pStringStart, char* pStringStop);
