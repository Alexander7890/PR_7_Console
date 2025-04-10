#include <windows.h>
#include <iostream>
#include <conio.h>

typedef void (*HookFunction)();

int main() {
    HINSTANCE hDll = LoadLibrary(TEXT("HookDll.dll"));
    if (!hDll) {
        std::cerr << "Не вдалося завантажити DLL!" << std::endl;
        return 1;
    }

    HookFunction SetHook = (HookFunction)GetProcAddress(hDll, "SetHook");
    HookFunction RemoveHook = (HookFunction)GetProcAddress(hDll, "RemoveHook");

    if (!SetHook || !RemoveHook) {
        std::cerr << "Не вдалося знайти функції в DLL!" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    SetHook();
    std::cout << "Хук встановлено. Натисніть ESC для виходу." << std::endl;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
            break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    RemoveHook();
    std::cout << "Хук знято." << std::endl;
    FreeLibrary(hDll);
    return 0;
}
