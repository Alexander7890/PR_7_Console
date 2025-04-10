#include <windows.h>
#include <iostream>

HHOOK hKeyboardHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = pKeyboard->vkCode;

        // Обробка спеціальних клавіш
        switch (vkCode) {
            case VK_SPACE: std::wcout << L"[SPACE]" << std::endl; break;
            case VK_RETURN: std::wcout << L"[ENTER]" << std::endl; break;
            case VK_BACK: std::wcout << L"[BACKSPACE]" << std::endl; break;
            case VK_TAB: std::wcout << L"[TAB]" << std::endl; break;
            case VK_ESCAPE:
                std::wcout << L"[ESCAPE] — вихід з програми" << std::endl;
                PostQuitMessage(0);
                return 0;
            case VK_INSERT: std::wcout << L"[INSERT]" << std::endl; break;
            case VK_DELETE: std::wcout << L"[DELETE]" << std::endl; break;
            case VK_HOME: std::wcout << L"[HOME]" << std::endl; break;
            case VK_END: std::wcout << L"[END]" << std::endl; break;
            case VK_PRIOR: std::wcout << L"[PAGE UP]" << std::endl; break;
            case VK_NEXT: std::wcout << L"[PAGE DOWN]" << std::endl; break;
            case VK_LEFT: std::wcout << L"[LEFT ARROW]" << std::endl; break;
            case VK_RIGHT: std::wcout << L"[RIGHT ARROW]" << std::endl; break;
            case VK_UP: std::wcout << L"[UP ARROW]" << std::endl; break;
            case VK_DOWN: std::wcout << L"[DOWN ARROW]" << std::endl; break;
            default:
                break;
        }

        BYTE keyboardState[256];
        if (!GetKeyboardState(keyboardState)) {
            return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
        }

        // Обробляємо Caps Lock та Shift
        bool capsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
        bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

        if (shift) {
            keyboardState[VK_SHIFT] |= 0x80;
        } else {
            keyboardState[VK_SHIFT] &= ~0x80;
        }

        WCHAR buffer[5] = {0};
        UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
        HKL layout = LoadKeyboardLayout(L"00000409", KLF_ACTIVATE); // Англійська розкладка

        int result = ToUnicodeEx(
            vkCode,
            scanCode,
            keyboardState,
            buffer,
            4,
            0,
            layout
        );

        if (result > 0) {
            buffer[result] = '\0';

            // Обробляємо Caps Lock + Shift для літер
            if (iswalpha(buffer[0])) {
                if (capsLock ^ shift) {
                    buffer[0] = towupper(buffer[0]);
                } else {
                    buffer[0] = towlower(buffer[0]);
                }
            }

            std::wcout << L"Key pressed: " << buffer << std::endl;
        } else if (
            vkCode != VK_SPACE && vkCode != VK_RETURN &&
            vkCode != VK_BACK && vkCode != VK_TAB && vkCode != VK_ESCAPE &&
            vkCode != VK_INSERT && vkCode != VK_DELETE &&
            vkCode != VK_HOME && vkCode != VK_END &&
            vkCode != VK_PRIOR && vkCode != VK_NEXT &&
            vkCode != VK_LEFT && vkCode != VK_RIGHT &&
            vkCode != VK_UP && vkCode != VK_DOWN
        ) {
            // Інші спеціальні клавіші
            char keyName[128];
            GetKeyNameTextA(pKeyboard->scanCode << 16, keyName, sizeof(keyName));
            std::cout << "Special key: " << keyName << std::endl;
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport) void SetHook() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
}

extern "C" __declspec(dllexport) void RemoveHook() {
    UnhookWindowsHookEx(hKeyboardHook);
}
