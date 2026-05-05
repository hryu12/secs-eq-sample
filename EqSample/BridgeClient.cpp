#include "stdafx.h"
#include "BridgeClient.h"

CBridgeClient::CBridgeClient()
    : m_socket(INVALID_SOCKET), m_hWnd(NULL), m_hThread(NULL), m_running(false)
{
    WSADATA wd;
    WSAStartup(MAKEWORD(2, 2), &wd);
}

CBridgeClient::~CBridgeClient()
{
    Disconnect();
    WSACleanup();
}

bool CBridgeClient::Connect(const std::string& host, int port, HWND hWnd)
{
    if (m_socket != INVALID_SOCKET)
        return true;

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
        return false;

    sockaddr_in addr;
    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons((u_short)port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());

    if (connect(m_socket, (sockaddr*)&addr, sizeof(addr)) != 0) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        return false;
    }

    m_hWnd    = hWnd;
    m_running = true;
    m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
    if (!m_hThread) {
        closesocket(m_socket);
        m_socket  = INVALID_SOCKET;
        m_running = false;
        return false;
    }
    return true;
}

void CBridgeClient::Disconnect()
{
    m_running = false;
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    if (m_hThread) {
        WaitForSingleObject(m_hThread, 5000);
        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
    m_leftover.clear();
}

bool CBridgeClient::SendJson(const std::string& json)
{
    if (m_socket == INVALID_SOCKET)
        return false;
    std::string line = json + "\n";
    int sent = send(m_socket, line.c_str(), (int)line.size(), 0);
    return sent == (int)line.size();
}

DWORD WINAPI CBridgeClient::ThreadProc(LPVOID param)
{
    reinterpret_cast<CBridgeClient*>(param)->RecvThread();
    return 0;
}

void CBridgeClient::RecvThread()
{
    if (m_hWnd)
        PostMessage(m_hWnd, WM_BRIDGE_CONN, 1, 0);

    char buf[4096];
    while (m_running) {
        int n = recv(m_socket, buf, sizeof(buf) - 1, 0);
        if (n <= 0)
            break;
        buf[n] = '\0';
        m_leftover += buf;
        size_t pos;
        while ((pos = m_leftover.find('\n')) != std::string::npos) {
            std::string line = m_leftover.substr(0, pos);
            m_leftover.erase(0, pos + 1);
            if (!line.empty()) {
                std::string* p = new std::string(line);
                PostMessage(m_hWnd, WM_BRIDGE_MSG, 0, (LPARAM)p);
            }
        }
    }

    if (m_hWnd)
        PostMessage(m_hWnd, WM_BRIDGE_CONN, 0, 0);
}
