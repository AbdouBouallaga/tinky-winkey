#ifndef TINKY_H
#define TINKY_H

#pragma comment(lib, "advapi32.lib")

#define		SVCNAME TEXT("tinky")

int				Open_SCManager();
int				OpenSvc();

VOID WINAPI		ServiceMain();
VOID WINAPI		ServiceCtrlHandler(DWORD CtrlCode);
void			ReportStatus(DWORD state, DWORD Q_ERROR);

#endif