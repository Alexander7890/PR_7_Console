#include <windows.h>
#include <iostream>

HHOOK hKeyboardHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = pKeyboard->vkCode;
        std::cout << "Key pressed: " << vkCode << std::endl;
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) void SetHook() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
}

extern "C" __declspec(dllexport) void RemoveHook() {
    UnhookWindowsHookEx(hKeyboardHook);
}
