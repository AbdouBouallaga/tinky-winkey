#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>

#pragma comment(lib, "advapi32.lib")

#define SVCNAME TEXT("tinky")



VOID SvcInstall(void);
VOID DoDeleteSvc(void);

SC_HANDLE scmH;
SC_HANDLE serviceH;


void open_SCManager() {
    scmH = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == scmH)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        exit(0);
    }
}

void open_Service() {
    serviceH = OpenService(
        scmH,       // SCM database 
        SVCNAME,          // name of service 
        DELETE);            // need delete access 

    if (serviceH == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(scmH);
        return;
    }
}

int main(int argc, CHAR* argv[])
{
    open_SCManager();
    if (!strcmp("install", argv[1]))
    {
        SvcInstall();
        return 0;
    }
    if (!strcmp("delete", argv[1]))
    {
        DoDeleteSvc();
        return 0;
    }
    if (!strcmp("start", argv[1]))
    {
        DoDeleteSvc();
        return 0;
    }
    CloseServiceHandle(scmH);
}



VOID DoDeleteSvc()
{
    open_Service();
    if (!DeleteService(serviceH))
    {
        printf("DeleteService failed (%d)\n", GetLastError());
    }
    else printf("Service deleted successfully\n");

    CloseServiceHandle(serviceH);
}

VOID SvcInstall()
{
    SC_HANDLE schService;
    TCHAR szUnquotedPath[MAX_PATH];

    if (!GetModuleFileName(NULL, szUnquotedPath, MAX_PATH))
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }

    // In case the path contains a space, it must be quoted so that
    // it is correctly interpreted. For example,
    // "d:\my share\myservice.exe" should be specified as
    // ""d:\my share\myservice.exe"".
    TCHAR szPath[MAX_PATH];
    StringCbPrintf(szPath, MAX_PATH, TEXT("\"%s\""), szUnquotedPath);

    schService = CreateService(
        scmH,              // SCM database 
        SVCNAME,                   // name of service 
        SVCNAME,                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_AUTO_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        szPath,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 

    if (schService == NULL)
    {
        printf("CreateService failed (%d)\n", GetLastError());
        CloseServiceHandle(scmH);
        return;
    }
    else printf("Service installed successfully\n");

    CloseServiceHandle(schService);
}


