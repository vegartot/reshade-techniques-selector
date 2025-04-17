
#include "main.h"


int __stdcall main(void)
{
    HRESULT hr = S_OK;

    hr = CoInitialize(NULL);
    if (FAILED(hr)) goto INIT_FAIL;

    // Various COM objects to initialize
    IShellItemArray* pItemsArray = NULL;
    LPWSTR directoryPath = NULL;

    hr = getItems(&pItemsArray, &directoryPath);
    if (FAILED(hr)) goto NO_ITEMS;

    DWORD fileCount = 0;
    hr = pItemsArray->lpVtbl->GetCount(pItemsArray, &fileCount);
    if (FAILED(hr)) goto NO_ITEMS;

    // Stack to contain all the unique reshade techniques across the selected files
    NODE* pHead = NULL;
    for (int i = 0; i < (int)fileCount; i++)
    { 
        IShellItem* pItem = NULL;
        LPWSTR fileName = NULL;

        hr = pItemsArray->lpVtbl->GetItemAt(pItemsArray, i, &pItem);
        if (FAILED(hr)) continue;

        hr = pItem->lpVtbl->GetDisplayName(pItem, SIGDN_FILESYSPATH, &fileName);
        if (FAILED(hr)) goto NAME_ERROR;

        if (directoryPath == NULL)
        {
            // Set path for writing to file later
            pItem->lpVtbl->GetDisplayName(pItem, SIGDN_DESKTOPABSOLUTEPARSING, &directoryPath);
        }

        // Open file for reading
        char buffer[MAX_READ_BYTES] = { 0 };
        HANDLE fHandle = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (fHandle == NULL) goto OPEN_FAIL;

        BOOL result = ReadFile(fHandle, (void*)buffer, MAX_READ_BYTES, NULL, NULL);
        if (result != TRUE) goto READ_FAIL;

        LPCSTR tokenStart = StrStrA(buffer, TOKEN);
        if (tokenStart == NULL) goto READ_FAIL;
    
        char* startByte = (char*)tokenStart + strlen(TOKEN);
        char* endByte = (char*)startByte;
        int EOF = 0;
        do
        {
            switch (*endByte)
            {
            
                case '\0':          // Null-term
                case '\r':          // Newline
                    EOF = 1;
                    pushIfNew(&pHead, startByte, endByte);
                    break;

            
                case 0x2c:          // Comma
                
                    pushIfNew(&pHead, startByte, endByte);

                    ++endByte;      // Skip comma delimiter
                    startByte = endByte;
                default:
                    ++endByte;
            }
        } while (!EOF);

READ_FAIL:
    CloseHandle(fHandle);
OPEN_FAIL:
    CoTaskMemFree(fileName);
NAME_ERROR:
    pItem->lpVtbl->Release(pItem);
    }

    if (pHead)
    {
        // If stack contains any techniques -> write them to file
        WCHAR outfileName[MAX_PATH] = { 0 };
        StrCpyW(outfileName, directoryPath);
        StrCatW(outfileName, L"\\" OUTFILE);
        HANDLE fOutHandle = CreateFileW(outfileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
        if (fOutHandle != INVALID_HANDLE_VALUE)
        {
            WriteFile(fOutHandle, TOKEN, (DWORD)strlen(TOKEN), NULL, NULL);

            NODE* pNode = pHead;
            while (pNode != NULL)
            {
                WriteFile(fOutHandle, pNode->buffer, (DWORD)strlen(pNode->buffer), NULL, NULL);
                if (pNode->pNext != NULL) WriteFile(fOutHandle, ",", (DWORD)strlen(","), NULL, NULL);
                pNode = pNode->pNext;
            }
            CloseHandle(fOutHandle);
        }
    }
    CoTaskMemFree(directoryPath);
    pItemsArray->lpVtbl->Release(pItemsArray);
NO_ITEMS:
    CoUninitialize();
INIT_FAIL:
    return hr;
}


void WinMainCRTStartup(void)
{   
    int result = main();
    ExitProcess(result);
}

HRESULT getItems(IShellItemArray** pItemList, LPWSTR* folderPath)
{
    HRESULT hr = S_OK;
    IFileOpenDialog* pInterface = NULL;
    
    // Retrieve FileOpenDialog interface
    hr = CoCreateInstance(&CLSID_FileOpenDialog, 0, CLSCTX_INPROC_SERVER, &IID_IFileOpenDialog, (void**)&pInterface);
    if (FAILED(hr)) goto INIT_FAIL;

    FILEOPENDIALOGOPTIONS opts;
    hr = pInterface->lpVtbl->GetOptions(pInterface, &opts);
    if (FAILED(hr)) goto CONFIG_FAIL;

    // Configure options for multiple file select etc.
    opts |= FOS_ALLOWMULTISELECT;
    hr = pInterface->lpVtbl->SetOptions(pInterface, opts);
    if (FAILED(hr)) goto CONFIG_FAIL;

    hr = pInterface->lpVtbl->SetTitle(pInterface, L"Select Reshade Presets");
    if (FAILED(hr)) goto CONFIG_FAIL;
    
    // Allow only text and INI files to be selected
    COMDLG_FILTERSPEC rgSpec = {
            .pszName = L"Presets",
            .pszSpec = L"*.ini;*.txt"
    };
    hr = pInterface->lpVtbl->SetFileTypes(
        pInterface,
        1,
        &rgSpec
    );
    if (FAILED(hr)) goto CONFIG_FAIL;

    // Show FileOpenDialog to user and wait user input
    hr = pInterface->lpVtbl->Show(pInterface, NULL);
    if (FAILED(hr)) goto CONFIG_FAIL;

    // Get results and store in array type
    hr = pInterface->lpVtbl->GetResults(pInterface, pItemList);
    if (SUCCEEDED(hr))
    {
        // Store the directory path in variable
        IShellItem* pItem = NULL;
        hr = pInterface->lpVtbl->GetFolder(pInterface, &pItem);
        pItem->lpVtbl->GetDisplayName(pItem, SIGDN_FILESYSPATH, folderPath);
    }

CONFIG_FAIL:
    pInterface->lpVtbl->Release(pInterface);  
INIT_FAIL:
    return hr;
}

// Push node to stack if technique is unique 
int pushIfNew(NODE** pStackObject, char* pStringStart, char* pStringStop)
{
    char techniqueString[TECHNIQUE_MAX_BYTES] = { 0 };
    memcpy(techniqueString, pStringStart, pStringStop - pStringStart);

    NODE* pNode = *pStackObject;
    while (pNode != NULL)
    {
        if (StrCmpA(pNode->buffer, techniqueString) == 0) return 0;
        pNode = pNode->pNext;
    }
    NODE* pNewHead = (NODE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NODE));
    if (pNewHead == NULL) return -1;
    memcpy(pNewHead->buffer, pStringStart, pStringStop - pStringStart);
    pNewHead->pNext = *pStackObject;
    *pStackObject = pNewHead;
    
    return 1;
}
