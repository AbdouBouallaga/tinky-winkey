#ifndef TINKY_H
#define TINKY_H

#pragma warning(push)
#pragma warning(disable:4668 4577)
#include <windows.h>
#include <iostream>
#include <Tlhelp32.h>
#pragma warning(pop)


#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "User32.lib")


#define		SVCNAME TEXT("tinky")


int				Open_SCManager();
int				OpenSvc();

//LPSERVICE_MAIN_FUNCTIONA		ServiceMain();
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
VOID WINAPI		ServiceCtrlHandler(DWORD CtrlCode);
void			ReportStatus(DWORD state, DWORD Q_ERROR);

#endif