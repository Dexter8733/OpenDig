#include <windows.h>
#include <string>

HWND hEditInput;

void RunCommandWithPause(const std::string& cmd) {
    // Build command to run in a temporary console
    std::string fullCmd = "cmd.exe /c \"" + cmd + " & pause\"";

    // STARTUPINFO setup
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW; // show console window

    // Create a new console process for the command
    if (CreateProcess(NULL,
        const_cast<char*>(fullCmd.c_str()),
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {

        // Wait until command finishes
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Close handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        // Input box for host/domain
        hEditInput = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_CHILD | WS_VISIBLE | ES_LEFT,
            10, 10, 250, 25, hwnd, NULL, NULL, NULL);

        // Dig TXT button
        CreateWindow("BUTTON", "Dig TXT", WS_CHILD | WS_VISIBLE,
            270, 10, 70, 25, hwnd, (HMENU)1, NULL, NULL);

        // Ping button
        CreateWindow("BUTTON", "Ping", WS_CHILD | WS_VISIBLE,
            350, 10, 50, 25, hwnd, (HMENU)2, NULL, NULL);
        break;

    case WM_COMMAND:
    {
        char host[256];
        GetWindowText(hEditInput, host, sizeof(host));
        std::string hostStr = host;

        if (LOWORD(wParam) == 1) {  // Dig TXT
            RunCommandWithPause("nslookup -type=TXT " + hostStr);
        } else if (LOWORD(wParam) == 2) {  // Ping
            RunCommandWithPause("ping " + hostStr);
        }
        break;
    }

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MiniDigPingConsole";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("MiniDigPingConsole", "OpenDig", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 100, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
