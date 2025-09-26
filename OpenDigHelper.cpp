#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// WHOIS query
string whois_query(const string& server, const string& domain) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        return "WSAStartup failed.\n";

    struct addrinfo hints = {0}, *result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(server.c_str(), "43", &hints, &result) != 0) {
        WSACleanup();
        return "DNS resolution failed.\n";
    }

    SOCKET sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        return "Socket creation failed.\n";
    }

    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        closesocket(sock);
        freeaddrinfo(result);
        WSACleanup();
        return "Connection failed.\n";
    }
    freeaddrinfo(result);

    string query = domain + "\r\n";
    send(sock, query.c_str(), (int)query.size(), 0);

    char buffer[1024];
    string response;
    int bytes;
    while ((bytes = recv(sock, buffer, sizeof(buffer)-1, 0)) > 0) {
        buffer[bytes] = '\0';
        response += buffer;
    }

    closesocket(sock);
    WSACleanup();
    return response;
}

// Wait for key press before exit
void WaitAndExit() {
    cout << "\nPress any key to close..." << endl;
    _getch();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: OpenDigHelper <action> <target> [extra]\n";
        WaitAndExit();
        return 0;
    }

    string action = argv[1];
    string target = argv[2];
    string extra  = (argc > 3) ? argv[3] : "";

    if (action == "ping") {
        string cmd = "ping " + target;
        system(cmd.c_str());
    }
    else if (action == "dig") {
        string cmd = "nslookup -type=" + extra + " " + target;
        system(cmd.c_str());
    }
    else if (action == "tracert") {
        string cmd = "tracert " + target;
        system(cmd.c_str());
    }
    else if (action == "whois") {
        cout << "WHOIS query for " << target << ":\n\n";
        cout << whois_query("whois.iana.org", target);
    }
    else {
        cout << "Unknown action: " << action << endl;
    }

    WaitAndExit();
    return 0;
}
