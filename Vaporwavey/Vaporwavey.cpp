// Vaporwavey.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>

bool status = true;

int GetFullwidthChar(int vkCode, bool caps, bool shift)
{
	switch (vkCode)
	{
	//Space
	case 0X20:
		return 0xFFA0;

	//Numbers and symbols
	case 0x30:
	case 0X60:
		return shift ? 0xFF09 : 0xFF10;
	case 0x31:
	case 0X61:
		return shift ? 0xFF01 : 0xFF11;
	case 0x32:
	case 0X62:
		return shift ? 0xFF20 : 0xFF12;
	case 0x33:
	case 0X63:
		return shift ? 0xFF03 : 0xFF13;
	case 0x34:
	case 0X64:
		return shift ? 0xFF04 : 0xFF14;
	case 0x35:
	case 0X65:
		return shift ? 0xFF05 : 0xFF15;
	case 0x36:
	case 0X66:
		return shift ? 0xFF3E : 0xFF16;
	case 0x37:
	case 0X67:
		return shift ? 0xFF06 : 0xFF17;
	case 0x38:
	case 0X68:
		return shift ? 0xFF01 : 0xFF18;
	case 0x39:
	case 0X69:
		return shift ? 0xFF08 : 0xFF19;

	//Other symbols
	case 0xBA:
		return shift ? 0xFF1A : 0xFF1B;
	case 0xBB:
		return shift ? 0xFF0B : 0xFF1D;
	case 0xBC:
		return shift ? 0xFF1C : 0xFF0C;
	case 0xBD:
		return shift ? 0xFF3F : 0xFF0D;
	case 0xBE:
		return shift ? 0xFF1E : 0xFF0E;
	case 0xBF:
		return shift ? 0xFF1F : 0xFF0F;
	case 0xC0:
		return shift ? 0xFF5E : 0xFF07;
	case 0xDB:
		return shift ? 0xFF5B : 0xFF3B;
	case 0xDC:
		return shift ? 0xFF5C : 0xFF3C;
	case 0xDD:
		return shift ? 0xFF5D : 0xFF3D;
	case 0xDE:
		return shift ? 0xFF02 : 0xFF07;

	//Alphabet
	case 0x41:
		return caps ? 0xFF21 : 0xFF41;
	case 0x42:
		return caps ? 0xFF22 : 0xFF42;
	case 0x43:
		return caps ? 0xFF23 : 0xFF43;
	case 0x44:
		return caps ? 0xFF24 : 0xFF44;
	case 0x45:
		return caps ? 0xFF25 : 0xFF45;
	case 0x46:
		return caps ? 0xFF26 : 0xFF46;
	case 0x47:
		return caps ? 0xFF27 : 0xFF47;
	case 0x48:
		return caps ? 0xFF28 : 0xFF48;
	case 0x49:
		return caps ? 0xFF29 : 0xFF49;
	case 0x4A:
		return caps ? 0xFF2A : 0xFF4A;
	case 0x4B:
		return caps ? 0xFF2B : 0xFF4B;
	case 0x4C:
		return caps ? 0xFF2C : 0xFF4C;
	case 0x4D:
		return caps ? 0xFF2D : 0xFF4D;
	case 0x4E:
		return caps ? 0xFF2E : 0xFF4E;
	case 0x4F:
		return caps ? 0xFF2F : 0xFF4F;
	case 0x50:
		return caps ? 0xFF30 : 0xFF50;
	case 0x51:
		return caps ? 0xFF31 : 0xFF51;
	case 0x52:
		return caps ? 0xFF32 : 0xFF52;
	case 0x53:
		return caps ? 0xFF33 : 0XFF53;
	case 0x54:
		return caps ? 0xFF34 : 0xFF54;
	case 0x55:
		return caps ? 0xFF35 : 0xFF55;
	case 0x56:
		return caps ? 0xFF36 : 0xFF56;
	case 0x57:
		return caps ? 0xFF37 : 0xFF57;
	case 0x58:
		return caps ? 0xFF38 : 0xFF58;
	case 0x59:
		return caps ? 0xFF39 : 0xFF59;
	case 0x5A:
		return caps ? 0xFF5A : 0xFF60;
	}
	return 0;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (wParam & 0x0100)
	{
		PKBDLLHOOKSTRUCT info = (PKBDLLHOOKSTRUCT)lParam;

		if (nCode != 0 || (info->flags & 0x10))
		{
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}

		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}

		int fullwidthChar = GetFullwidthChar(
			info->vkCode,
			GetKeyState(VK_CAPITAL) & 0x0001, //could store status from previous calls to the hook
			GetKeyState(VK_SHIFT) & 0x8000 //could store status from previous calls to the hook
		);

		if (fullwidthChar && status)
		{
			if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
			{
				KEYBDINPUT ki = { 0 };
				INPUT input = { 0 };

				ki.wScan = fullwidthChar;
				ki.dwFlags = KEYEVENTF_UNICODE;
				input.type = INPUT_KEYBOARD;
				input.ki = ki;
				SendInput(1, &input, sizeof(input));
			}
			else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
			{
				KEYBDINPUT ki = { 0 };
				INPUT input = { 0 };

				ki.wScan = fullwidthChar;
				ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
				input.type = INPUT_KEYBOARD;
				input.ki = ki;
				SendInput(1, &input, sizeof(input));
			}

			return 1;
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	while (true)
	{
		if (GetAsyncKeyState(VK_F8) & 0x1)
		{
			printf("Toggling to %s\n", (status = !status) ? "ON" : "OFF");
			Sleep(100);
		}
	}

	return 0;
}

int main()
{
	SetConsoleTitle(L"Vaporwavey");
	HWND console = GetConsoleWindow();
	RECT rect;
	GetWindowRect(console, &rect);
	MoveWindow(console, rect.left, rect.top, 400, 200, TRUE);

	std::cout << "Vaporwavey" << std::endl;
	std::cout << "Made by @Jamezrin" << std::endl;
	std::cout << "Press the F8 key to toggle status" << std::endl;

	CreateThread(NULL, 0, &ThreadProc, NULL, 0, 0);

	HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, 0, 0);

	MSG msg;
	while (!GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hhkLowLevelKybd);

	return 0;
}