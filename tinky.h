#ifndef TINKY_H
#define TINKY_H

#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "advapi32.lib")


#define		SVCNAME TEXT("tinky")


int				Open_SCManager();
int				OpenSvc();

LPSERVICE_MAIN_FUNCTIONA		ServiceMain();
VOID WINAPI		ServiceCtrlHandler(DWORD CtrlCode);
void			ReportStatus(DWORD state, DWORD Q_ERROR);

#endif