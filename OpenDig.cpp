#include <windows.h>
#include <string>

HWND hEditInput;
HWND hComboQuery;

void RunCommandWithPause(const std::string& cmd) {
    std::string fullCmd = "cmd.exe /c \"" + cmd + " & pause\"";

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;

    if (CreateProcess(NULL,
        const_cast<char*>(fullCmd.c_str()),
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
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

        // Dropdown (ComboBox) for query type
        hComboQuery = CreateWindow("COMBOBOX", NULL,
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            10, 45, 120, 100, hwnd, NULL, NULL, NULL);

        // Add items
        SendMessage(hComboQuery, CB_ADDSTRING, 0, (LPARAM)"A");
        SendMessage(hComboQuery, CB_ADDSTRING, 0, (LPARAM)"MX");
        SendMessage(hComboQuery, CB_ADDSTRING, 0, (LPARAM)"TXT");
        SendMessage(hComboQuery, CB_ADDSTRING, 0, (LPARAM)"CNAME");
        SendMessage(hComboQuery, CB_ADDSTRING, 0, (LPARAM)"SOA");
        SendMessage(hComboQuery, CB_ADDSTRING, 0, (LPARAM)"NS");

        // Default selection = TXT
        SendMessage(hComboQuery, CB_SETCURSEL, 2, 0);

        // Dig button
        CreateWindow("BUTTON", "Dig", WS_CHILD | WS_VISIBLE,
            150, 45, 70, 25, hwnd, (HMENU)1, NULL, NULL);

        // Ping button
        CreateWindow("BUTTON", "Ping", WS_CHILD | WS_VISIBLE,
            230, 45, 70, 25, hwnd, (HMENU)2, NULL, NULL);
        break;

    case WM_COMMAND:
    {
        char host[256];
        GetWindowText(hEditInput, host, sizeof(host));
        std::string hostStr = host;

        if (LOWORD(wParam) == 1) {  // Dig
            char queryType[16];
            int sel = SendMessage(hComboQuery, CB_GETCURSEL, 0, 0);
            SendMessage(hComboQuery, CB_GETLBTEXT, sel, (LPARAM)queryType);

            std::string cmd = "nslookup -type=" + std::string(queryType) + " " + hostStr;
            RunCommandWithPause(cmd);

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
    wc.lpszClassName = "OpenDig";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("OpenDig", "OpenDig v0.02a - Dig / Ping",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 350, 150,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
