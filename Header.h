#ifndef HEADER_H
#define HEADER_H

#pragma comment(lib, "advapi32.lib")

#define SVCNAME TEXT("tinky")

VOID SvcInstall(void);
VOID DoDeleteSvc(void);

#endif