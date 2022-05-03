#include "winkey.h"
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
#include <iostream>




void GetPidByName(const char* filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                printf(">>> %lu",pEntry.th32ProcessID);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}

int main() {
    GetPidByName("winlogon.exe");
	return 0;
}