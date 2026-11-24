#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <cwchar>
#include <memory>
#include <string>

#include "triskelion_win/windows_client.hpp"

// Public key of counter.key (the private key never goes in the app).
// Server: your Triskelion service on Render.
#ifndef COUNTER_VERSION
#define COUNTER_VERSION "1.0.0"
#endif

namespace {
constexpr int kValueId = 1001;
constexpr int kDecrementId = 1002;
constexpr int kResetId = 1003;
constexpr int kIncrementId = 1004;
constexpr int kCheckId = 1005;
constexpr int kStatusId = 1006;
constexpr UINT kUpdateStatusMessage = WM_APP + 1;

constexpr char kServerUrl[] = "https://triskelion.onrender.com";
constexpr char kAppId[] = "counter";
constexpr char kPublicKey[] = "IQncJwj/zAPAdrIxZ3kQk9fc5MJmZyghNyo1i4cZwTw=";

HWND statusLabel = nullptr;
std::unique_ptr<triskelion_win::WindowsClient> updater;

std::wstring Widen(const std::string& utf8) {
    if (utf8.empty()) return L"";
    const int n = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
    std::wstring out(static_cast<size_t>(n), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), &out[0], n);
    return out;
}

// Called by Triskelion on its own thread: hand the text to the UI thread.
void PostStatus(HWND window, const std::string& text) {
    PostMessageW(window, kUpdateStatusMessage, 0, reinterpret_cast<LPARAM>(new std::wstring(Widen(text))));
}

void StartUpdater(HWND window) {
    try {
        triskelion_win::Config cfg;
        cfg.server_url = kServerUrl;
        cfg.app_id = kAppId;
        cfg.current_version = COUNTER_VERSION;
        cfg.public_key_b64 = kPublicKey;
        cfg.mode = triskelion_win::ApplyMode::ReplaceExe;  // the release is the single CounterApp.exe
        updater = std::make_unique<triskelion_win::WindowsClient>(
            cfg, [window](const triskelion::RunResult& r) {
                if (r.outcome == triskelion::Outcome::Applied)
                    PostStatus(window, "Updated to " + r.info.manifest.version + ". Restart the app to use it.");
                else if (r.outcome == triskelion::Outcome::UpToDate)
                    PostStatus(window, "Up to date");
                else
                    PostStatus(window, std::string(triskelion::to_string(r.outcome)) + " " + r.detail);
            });
        updater->start();
    } catch (const std::exception& e) {
        PostStatus(window, std::string("Updater not started: ") + e.what());
    }
}

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

            CreateWindowExW(
                0, L"BUTTON", L"Check for updates",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                125, 220, 150, 30,
                window, reinterpret_cast<HMENU>(kCheckId),
                GetModuleHandleW(nullptr), nullptr);

            statusLabel = CreateWindowExW(
                0, L"STATIC", Widen(std::string("Version ") + COUNTER_VERSION).c_str(),
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                20, 260, 360, 40,
                window, reinterpret_cast<HMENU>(kStatusId),
                GetModuleHandleW(nullptr), nullptr);

            StartUpdater(window);
            return 0;
        }

        case kUpdateStatusMessage: {
            std::unique_ptr<std::wstring> text(reinterpret_cast<std::wstring*>(lParam));
            SetWindowTextW(statusLabel, (L"Version " + Widen(COUNTER_VERSION) + L"\n" + *text).c_str());
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
                case kCheckId:
                    SetWindowTextW(statusLabel, L"Checking...");
                    if (updater) updater->check_now();
                    return 0;
                default:
                    return DefWindowProcW(window, message, wParam, lParam);
            }
            UpdateCounter(window);
            return 0;
        }

        case WM_DESTROY:
            updater.reset();  // stops the update threads before the window goes away
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
        CW_USEDEFAULT, CW_USEDEFAULT, 416, 360,
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
