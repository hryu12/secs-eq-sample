#pragma once
#include <string>
// WinSock2 types provided by MFC/afxwin.h via stdafx.h.

#define WM_BRIDGE_MSG   (WM_USER + 100)  // LPARAM = new std::string* (caller deletes)
#define WM_BRIDGE_CONN  (WM_USER + 101)  // WPARAM = 1 connected / 0 disconnected

class CBridgeClient
{
public:
    CBridgeClient();
    ~CBridgeClient();

    bool Connect(const std::string& host, int port, HWND hWnd);
    void Disconnect();
    bool SendJson(const std::string& json);
    bool IsConnected() const { return m_socket != INVALID_SOCKET; }

private:
    static DWORD WINAPI ThreadProc(LPVOID param);
    void RecvThread();

    SOCKET        m_socket;
    HWND          m_hWnd;
    HANDLE        m_hThread;
    volatile bool m_running;
    std::string   m_leftover;
};
