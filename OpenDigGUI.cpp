#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <string>

using namespace std;

HWND hEditUrl, hComboQuery;
int lastSelection = 5; // default "A"
HBITMAP hHeaderBmp;

// Launch helper console process
void LaunchHelper(const string& action, const string& target, const string& extra = "") {
    string cmd = "OpenDigHelper.exe " + action + " \"" + target + "\" " + extra;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

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
        hHeaderBmp = (HBITMAP)LoadImage(NULL, "HEADER.BMP", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hHeaderBmp) {
            HWND hBmp = CreateWindow("STATIC", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP,
                                     0, 0, 400, 80, hwnd, NULL, NULL, NULL);
            SendMessage(hBmp, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hHeaderBmp);
        }

        int controlsY = 130;

        CreateWindow("STATIC", "Server / Domain:", WS_VISIBLE | WS_CHILD,
                     10, controlsY, 100, 20, hwnd, NULL, NULL, NULL);
        hEditUrl = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                120, controlsY, 200, 20, hwnd, NULL, NULL, NULL);

        CreateWindow("STATIC", "Query Type:", WS_VISIBLE | WS_CHILD,
                     10, controlsY + 30, 100, 20, hwnd, NULL, NULL, NULL);
        hComboQuery = CreateWindow("COMBOBOX", "",
                                   WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                                   120, controlsY + 30, 200, 250, hwnd, NULL, NULL, NULL);
        const char* items[] = { "TXT", "CNAME", "SOA", "MX", "NS", "A" };
        for (int i = 0; i < 6; ++i)
            SendMessage(hComboQuery, CB_ADDSTRING, 0, (LPARAM)items[i]);
        SendMessage(hComboQuery, CB_SETCURSEL, lastSelection, 0);

        CreateWindow("BUTTON", "Ping", WS_VISIBLE | WS_CHILD,
                     10, controlsY + 70, 80, 30, hwnd, (HMENU)1, NULL, NULL);
        CreateWindow("BUTTON", "Dig", WS_VISIBLE | WS_CHILD,
                     100, controlsY + 70, 80, 30, hwnd, (HMENU)2, NULL, NULL);
        CreateWindow("BUTTON", "WHOIS", WS_VISIBLE | WS_CHILD,
                     190, controlsY + 70, 80, 30, hwnd, (HMENU)3, NULL, NULL);
        CreateWindow("BUTTON", "Traceroute", WS_VISIBLE | WS_CHILD,
                     280, controlsY + 70, 90, 30, hwnd, (HMENU)4, NULL, NULL);

        break;
    }
    case WM_COMMAND: {
        char buf[256];
        GetWindowText(hEditUrl, buf, sizeof(buf));
        string target(buf);

        LRESULT sel = SendMessage(hComboQuery, CB_GETCURSEL, 0, 0);
        if (sel != CB_ERR) lastSelection = (int)sel;

        char qtype[32];
        if (sel == CB_ERR) {
            strcpy(qtype, "A");
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
        if (hHeaderBmp) DeleteObject(hHeaderBmp);
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

    HWND hwnd = CreateWindow("OpenDigGUIClass", "OpenDig v0.04a",
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             400, 280, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
