#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <cstdio>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

string whois_query(const string& server, const string& domain) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) return "WSAStartup failed.\n";

    SOCKET sock = INVALID_SOCKET;
    struct addrinfo hints, *res = nullptr;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(server.c_str(), "43", &hints, &res) != 0) {
        WSACleanup();
        return "getaddrinfo failed.\n";
    }

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == INVALID_SOCKET) {
        freeaddrinfo(res);
        WSACleanup();
        return "Socket creation failed.\n";
    }

    if (connect(sock, res->ai_addr, (int)res->ai_addrlen) != 0) {
        closesocket(sock);
        freeaddrinfo(res);
        WSACleanup();
        return "Connection failed.\n";
    }

    freeaddrinfo(res);

    string query = domain + "\r\n";
    send(sock, query.c_str(), (int)query.size(), 0);

    char buf[1024];
    int bytes;
    string response;
    while ((bytes = recv(sock, buf, sizeof(buf)-1, 0)) > 0) {
        buf[bytes] = '\0';
        response += buf;
    }

    closesocket(sock);
    WSACleanup();
    return response;
}

void RunSystemCommand(const string& cmd) {
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) {
        cout << "Failed to run command.\n";
        return;
    }
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        cout << buffer;
    }
    _pclose(pipe);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: OpenDigHelper <action> <target> [extra]\n";
        return 1;
    }

    string action = argv[1];
    string target = argv[2];
    string extra = (argc > 3) ? argv[3] : "";

    if (action == "ping") {
        RunSystemCommand("ping " + target);
    } else if (action == "dig") {
        RunSystemCommand("nslookup -type=" + extra + " " + target);
    } else if (action == "tracert") {
        RunSystemCommand("tracert " + target);
    } else if (action == "whois") {
        string result = whois_query("whois.iana.org", target);
        cout << result;
    }

    cout << "\n\nPress any key to close...";
    _getch();
    return 0;
}
