#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <commctrl.h>
#include <string>

#pragma comment(lib, "comctl32.lib")

using namespace std;

HWND hEditUrl, hComboQuery;
int lastSelection = 5; // default "A"

// Launch helper console process
void LaunchHelper(const string& action, const string& target, const string& extra = "") {
    string cmd = "OpenDigHelper.exe " + action + " \"" + target + "\" " + extra;
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
                      CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        MessageBox(NULL, "Failed to launch helper process.", "Error", MB_OK | MB_ICONERROR);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // Server / Domain label and edit box
        CreateWindow("STATIC", "Server / Domain:", WS_VISIBLE | WS_CHILD,
                     10, 10, 100, 20, hwnd, NULL, NULL, NULL);
        hEditUrl = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                120, 10, 200, 20, hwnd, NULL, NULL, NULL);

        // Query type label and combo box
        CreateWindow("STATIC", "Query Type:", WS_VISIBLE | WS_CHILD,
                     10, 40, 100, 20, hwnd, NULL, NULL, NULL);

        hComboQuery = CreateWindow("COMBOBOX", "",
                                   WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                                   120, 40, 200, 250, hwnd, NULL, NULL, NULL); // dropdown height = 250

        // Add six items: TXT, CNAME, SOA, MX, NS, A
        const char* items[] = { "TXT", "CNAME", "SOA", "MX", "NS", "A" };
        for (int i = 0; i < 6; ++i)
            SendMessage(hComboQuery, CB_ADDSTRING, 0, (LPARAM)items[i]);

        // Default selection is "A" (index 5)
        SendMessage(hComboQuery, CB_SETCURSEL, lastSelection, 0);

        // Buttons
        CreateWindow("BUTTON", "Ping", WS_VISIBLE | WS_CHILD,
                     10, 80, 80, 30, hwnd, (HMENU)1, NULL, NULL);
        CreateWindow("BUTTON", "Dig", WS_VISIBLE | WS_CHILD,
                     100, 80, 80, 30, hwnd, (HMENU)2, NULL, NULL);
        CreateWindow("BUTTON", "WHOIS", WS_VISIBLE | WS_CHILD,
                     190, 80, 80, 30, hwnd, (HMENU)3, NULL, NULL);
        CreateWindow("BUTTON", "Traceroute", WS_VISIBLE | WS_CHILD,
                     280, 80, 90, 30, hwnd, (HMENU)4, NULL, NULL);
        break;
    }
    case WM_COMMAND: {
        char buf[256];
        GetWindowText(hEditUrl, buf, sizeof(buf));
        string target(buf);

        // Preserve combo selection
        LRESULT sel = SendMessage(hComboQuery, CB_GETCURSEL, 0, 0);
        if (sel != CB_ERR) lastSelection = (int)sel;

        char qtype[32];
        if (sel == CB_ERR) {
            strcpy(qtype, "A"); // fallback
            SendMessage(hComboQuery, CB_SETCURSEL, lastSelection, 0);
        } else {
            SendMessage(hComboQuery, CB_GETLBTEXT, sel, (LPARAM)qtype);
        }
        string queryType(qtype);

        switch (LOWORD(wParam)) {
        case 1: LaunchHelper("ping", target); break;
        case 2: LaunchHelper("dig", target, queryType); break;
        case 3: LaunchHelper("whois", target); break;
        case 4: LaunchHelper("tracert", target); break;
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "OpenDigGUIClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("OpenDigGUIClass", "OpenDig v0.03a",
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             400, 160, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
