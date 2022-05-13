#ifndef WINKEY_H
#define WINKEY_H

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "User32.lib")
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(push)
#pragma warning(disable:4668 4577)
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <iostream>
#include <psapi.h>
#include <string>
#include <time.h>
#include <conio.h>
#pragma warning(pop)


char *HookCode(DWORD code, BOOL caps, BOOL shift);

#endif

