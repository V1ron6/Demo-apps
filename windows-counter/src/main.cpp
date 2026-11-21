#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <cwchar>

namespace {
constexpr int kValueId = 1001;
constexpr int kDecrementId = 1002;
constexpr int kResetId = 1003;
constexpr int kIncrementId = 1004;

HWND valueLabel = nullptr;
long long counter = 0;

void UpdateCounter(HWND window) {
    wchar_t text[32];
    std::swprintf(text, 32, L"%lld", counter);
    SetWindowTextW(valueLabel, text);
    if (window != nullptr) {
        InvalidateRect(window, nullptr, TRUE);
    }
}

LRESULT CALLBACK WindowProcedure(HWND window, UINT message,
                                 WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            valueLabel = CreateWindowExW(
                0, L"STATIC", L"0",
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                50, 20, 300, 100,
                window, reinterpret_cast<HMENU>(kValueId),
                GetModuleHandleW(nullptr), nullptr);

            CreateWindowExW(
                0, L"BUTTON", L"-",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                50, 150, 90, 50,
                window, reinterpret_cast<HMENU>(kDecrementId),
                GetModuleHandleW(nullptr), nullptr);

            CreateWindowExW(
                0, L"BUTTON", L"Reset",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                155, 150, 90, 50,
                window, reinterpret_cast<HMENU>(kResetId),
                GetModuleHandleW(nullptr), nullptr);

            CreateWindowExW(
                0, L"BUTTON", L"+",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                260, 150, 90, 50,
                window, reinterpret_cast<HMENU>(kIncrementId),
                GetModuleHandleW(nullptr), nullptr);

            return 0;
        }

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case kDecrementId:
                    --counter;
                    break;
                case kResetId:
                    counter = 0;
                    break;
                case kIncrementId:
                    ++counter;
                    break;
                default:
                    return DefWindowProcW(window, message, wParam, lParam);
            }
            UpdateCounter(window);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProcW(window, message, wParam, lParam);
    }
}
}  // namespace

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int showCommand) {
    const wchar_t className[] = L"CounterAppWindow";
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszClassName = className;

    if (RegisterClassExW(&windowClass) == 0) {
        return 1;
    }

    HWND window = CreateWindowExW(
        0, className, L"Counter",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 416, 260,
        nullptr, nullptr, instance, nullptr);

    if (window == nullptr) {
        return 1;
    }

    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}
