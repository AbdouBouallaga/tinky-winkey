#include <windows.h>
#include <tchar.h>
#include <iostream>
#include "tinky.h"


SC_HANDLE scmH;
SC_HANDLE serviceH;

void usage() {
    printf("Usage: svc [install, delete, start, stop] 1 arg at the time\n");
}

int Open_SCManager() {
    scmH = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == scmH)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return(0);
    }
    return(1);
}

int OpenSvc() {
    serviceH = OpenService(
        scmH,       // SCM database 
        SVCNAME,    // name of service 
        GENERIC_ALL      // need delete access 
    );

    if (serviceH == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(scmH);
        return(0);
    }
    return(1);
}

void DeleteSvc()
{
    if (OpenSvc()) {
        if (!DeleteService(serviceH))
        {
            printf("DeleteService failed (%d)\n", GetLastError());
        }
        else printf("Service deleted successfully\n");
    }
    else {
        printf("Service not installed\n");
    }
    CloseServiceHandle(serviceH);
}

void SvcInstall()
{
    SC_HANDLE schService;
    char Path[260];


    if (!GetCurrentDirectory(260, Path))
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }
    strcat(Path, "\\tinky.exe");
    printf("%s\n", Path);

    schService = CreateService(
        scmH,              // SCM database 
        SVCNAME,                   // name of service 
        SVCNAME,                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        Path,                      // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL                       // no password 
    );

    if (schService == NULL)
    {
        printf("CreateService failed (%d)\n", GetLastError());
        CloseServiceHandle(scmH);
        return;
    }

    else printf("Service installed successfully\n");

    CloseServiceHandle(schService);
}

void StartSvc() {
    if (!StartService(serviceH, 0, NULL)) {
        printf("StartService failed (%d)\n", GetLastError());
    }
}

int main(int argc, CHAR **argv)
{
    if (!Open_SCManager()) {
        printf("Please run as administrator.\n");
        exit(0);
    }
    if (argc != 2) {
        usage();
    }
    else if (!strcmp("install", argv[1]))
    {
        SvcInstall();
        return 0;
    }
    else if (!strcmp("delete", argv[1]))
    {
        if (OpenSvc()) {
            DeleteSvc();
        }
        else {
            printf("Service not installed!\n");
        }
    }
    else if (!strcmp("start", argv[1]))
    {
        if (OpenSvc()) {
            StartSvc();
        }
        else {
            printf("Service not installed!\nTry svc install.\n");
        }
    }
    else if (!strcmp("stop", argv[1]))
    {
        if (OpenSvc()) {
            DeleteSvc();
        }
        else {
            printf("Service not installed!\nTry svc install.\n");
        }
    }
    else {
        usage();
    }
    CloseServiceHandle(scmH);
    CloseServiceHandle(serviceH);
}




