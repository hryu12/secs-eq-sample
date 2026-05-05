// EqSampleDlg.cpp

#include "stdafx.h"
#include "EqSample.h"
#include "EqSampleDlg.h"
#include "afxdialogex.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define VALUE_I1        10
#define VALUE_I2        32555
#define VALUE_I4        655360
#define VALUE_I8        240000001
#define VALUE_U1        250
#define VALUE_U2        65000
#define VALUE_U4        4294967290
#define VALUE_U8        1844674400
#define VALUE_F4        1234.567f
#define VALUE_F8        123456789.87654321
#define VALUE_BOOL      1
#define VALUE_BINARY    10
#define VALUE_STRING    "STRING; With-Tech Hsms Sample"
#define VALUE_JIS8      "JIS8; With-Tech Hsms Sample Sample"

// ---------------------------------------------------------------------------
// JSON helpers  (narrow std::string ??JSON is always ASCII)
// ---------------------------------------------------------------------------

static int BridgeGetInt(const std::string& s, const char* key, int def = 0)
{
    std::string search = std::string("\"") + key + "\":";
    size_t p = s.find(search);
    if (p == std::string::npos) return def;
    p += search.size();
    while (p < s.size() && s[p] == ' ') p++;
    return atoi(s.c_str() + p);
}

static std::string BridgeGetStr(const std::string& s, const char* key,
                                const std::string& def = "")
{
    std::string search = std::string("\"") + key + "\":\"";
    size_t p = s.find(search);
    if (p == std::string::npos) return def;
    p += search.size();
    size_t e = s.find('"', p);
    if (e == std::string::npos) return def;
    return s.substr(p, e - p);
}

static bool BridgeGetBool(const std::string& s, const char* key, bool def = false)
{
    std::string search = std::string("\"") + key + "\":";
    size_t p = s.find(search);
    if (p == std::string::npos) return def;
    p += search.size();
    while (p < s.size() && s[p] == ' ') p++;
    return p + 4 <= s.size() && s.substr(p, 4) == "true";
}

static std::string IntStr(int n)
{
    char buf[32]; sprintf_s(buf, "%d", n); return buf;
}

static std::string MakeReply(const std::string& id, int stream, int func,
                             const std::string& data)
{
    std::string j;
    j += "{\"id\":\"";    j += id;
    j += "\",\"stream\":"; j += IntStr(stream);
    j += ",\"function\":"; j += IntStr(func);
    j += ",\"reply\":false,\"data\":"; j += data; j += "}";
    return j;
}

static std::string MakeRequest(int stream, int func, bool wbit,
                               const std::string& data)
{
    static long long s_cnt = 0;
    char id[32]; sprintf_s(id, "mfc-%I64d", ++s_cnt);
    std::string j;
    j += "{\"id\":\"";    j += id;
    j += "\",\"stream\":"; j += IntStr(stream);
    j += ",\"function\":"; j += IntStr(func);
    j += ",\"reply\":";   j += (wbit ? "true" : "false");
    j += ",\"data\":";    j += data; j += "}";
    return j;
}

// ---------------------------------------------------------------------------
// CAboutDlg
// ---------------------------------------------------------------------------

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();
    enum { IDD = IDD_ABOUTBOX };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// ---------------------------------------------------------------------------
// CEqSampleDlg
// ---------------------------------------------------------------------------

CEqSampleDlg::CEqSampleDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CEqSampleDlg::IDD, pParent)
{
    m_hIcon   = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_cmbDevice = _T("");
    m_cmbMsg    = _T("");
}

void CEqSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_CBString(pDX, IDC_COMBO_DEVICE, m_cmbDevice);
    DDX_CBString(pDX, IDC_COMBO_MSG,    m_cmbMsg);
    DDX_Control(pDX,  IDC_LST_MSG,      m_lstbxMsg);
    // NOTE: Remove IDC_XCOMPROCTRL1 ActiveX control from the dialog resource
    //       in Visual Studio Resource Editor before building.
}

BEGIN_MESSAGE_MAP(CEqSampleDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK,              &CEqSampleDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BTN_START,     &CEqSampleDlg::OnBnClickedBtnStart)
    ON_BN_CLICKED(IDC_BTN_STOP,      &CEqSampleDlg::OnBnClickedBtnStop)
    ON_BN_CLICKED(IDC_BTN_SEND_MSG,  &CEqSampleDlg::OnBnClickedBtnSendMsg)
    ON_LBN_DBLCLK(IDC_LST_MSG,       &CEqSampleDlg::OnLbnDblclkLstMsg)
    ON_MESSAGE(WM_BRIDGE_MSG,  &CEqSampleDlg::OnBridgeMsg)
    ON_MESSAGE(WM_BRIDGE_CONN, &CEqSampleDlg::OnBridgeConn)
END_MESSAGE_MAP()


BOOL CEqSampleDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_cmbMsg    = _T("S1F1");
    m_cmbDevice = _T("1");

    for (int i = 0; i < MAX_ARRAY; i++) {
        VALUE_I1_ARR[i]     = (char)(i + 1);
        VALUE_I2_ARR[i]     = (short)(i + 1);
        VALUE_I4_ARR[i]     = i + 1;
        VALUE_I8_ARR[i]     = i + 1;
        VALUE_U1_ARR[i]     = (BYTE)(i + 1);
        VALUE_U2_ARR[i]     = (WORD)(i + 1);
        VALUE_U4_ARR[i]     = i + 1;
        VALUE_U8_ARR[i]     = i + 1;
        VALUE_F4_ARR[i]     = (float)(i + 1);
        VALUE_F8_ARR[i]     = (double)(i + 1);
        VALUE_BOOL_ARR[i]   = 1;
        VALUE_BINARY_ARR[i] = 1;
    }
    m_SECSCommunicationOK = false;
    GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
    UpdateData(FALSE);
    return TRUE;
}

void CEqSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CEqSampleDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        dc.DrawIcon((rect.Width() - cxIcon + 1) / 2,
                    (rect.Height() - cyIcon + 1) / 2, m_hIcon);
    }
    else {
        CDialogEx::OnPaint();
    }
}

HCURSOR CEqSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CEqSampleDlg::OnBnClickedOk()
{
    m_Bridge.Disconnect();
    CDialogEx::OnOK();
}


void CEqSampleDlg::OnBnClickedBtnStart()
{
    if (m_Bridge.Connect("127.0.0.1", 19001, GetSafeHwnd())) {
        m_lstbxMsg.AddString(_T("Bridge connected to relay (127.0.0.1:19001)"));
    }
    else {
        m_lstbxMsg.AddString(_T("[Error] Failed to connect to relay (127.0.0.1:19001)"));
        return;
    }
    GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
}


void CEqSampleDlg::OnBnClickedBtnStop()
{
    m_Bridge.Disconnect();
    m_SECSCommunicationOK = false;
    GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
}


// ---------------------------------------------------------------------------
// WM_BRIDGE_CONN  (WPARAM=1 connected, 0 disconnected)
// ---------------------------------------------------------------------------

LRESULT CEqSampleDlg::OnBridgeConn(WPARAM wParam, LPARAM)
{
    if (wParam == 1) {
        m_lstbxMsg.AddString(_T("[EVENT] Bridge connected"));
    }
    else {
        m_SECSCommunicationOK = false;
        m_lstbxMsg.AddString(_T("[EVENT] Bridge disconnected"));
        GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
    }
    while (m_lstbxMsg.GetCount() > 200) m_lstbxMsg.DeleteString(0);
    return 0;
}


// ---------------------------------------------------------------------------
// WM_BRIDGE_MSG  (LPARAM = new std::string*, caller deletes)
// ---------------------------------------------------------------------------

LRESULT CEqSampleDlg::OnBridgeMsg(WPARAM, LPARAM lParam)
{
    std::string* pLine = reinterpret_cast<std::string*>(lParam);
    std::string  line  = *pLine;
    delete pLine;

    int  s   = BridgeGetInt(line, "stream");
    int  f   = BridgeGetInt(line, "function");
    bool rep = BridgeGetBool(line, "reply");
    std::string id = BridgeGetStr(line, "id");

    CString cs;
    cs.Format(_T("Received S%dF%d"), s, f);
    m_lstbxMsg.AddString(cs);

    if (!rep) {
        // Reply to a message we initiated ??display only.
        while (m_lstbxMsg.GetCount() > 200) m_lstbxMsg.DeleteString(0);
        return 0;
    }

    // Messages from MES that require a reply from us.
    std::string replyJson;
    LPCTSTR tag = nullptr;

    if (s == 1 && f == 1) {
        replyJson = MakeReply(id, 1, 2,
            "[{\"type\":\"A\",\"value\":\"XComV2\"}"
            ",{\"type\":\"A\",\"value\":\"V2.0.0\"}]");
        tag = _T("S1F2");
    }
    else if (s == 1 && f == 13) {
        m_SECSCommunicationOK = true;
        replyJson = MakeReply(id, 1, 14,
            "[{\"type\":\"Binary\",\"value\":\"AA==\"},[]]");
        tag = _T("S1F14");
    }
    else if (s == 2 && f == 13) {
        replyJson = MakeReply(id, 2, 14,
            "[{\"type\":\"Binary\",\"value\":\"AA==\"}]");
        tag = _T("S2F14");
    }
    else if (s == 2 && f == 33) {
        replyJson = MakeReply(id, 2, 34,
            "{\"type\":\"Binary\",\"value\":\"AA==\"}");
        tag = _T("S2F34");
    }
    else if (s == 5 && f == 23) {
        replyJson = MakeReply(id, 5, 24,
            "{\"type\":\"U1\",\"value\":1}");
        tag = _T("S5F24");
    }
    else {
        cs.Format(_T("Undefined S%dF%d -- no reply sent"), s, f);
        m_lstbxMsg.AddString(cs);
        while (m_lstbxMsg.GetCount() > 200) m_lstbxMsg.DeleteString(0);
        return 0;
    }

    if (m_Bridge.SendJson(replyJson))
        cs.Format(_T("Reply %s successfully"), tag);
    else
        cs.Format(_T("Fail to reply %s"), tag);
    m_lstbxMsg.AddString(cs);

    while (m_lstbxMsg.GetCount() > 200) m_lstbxMsg.DeleteString(0);
    return 0;
}


// ---------------------------------------------------------------------------
// Send button
// ---------------------------------------------------------------------------

void CEqSampleDlg::OnBnClickedBtnSendMsg()
{
    UpdateData(TRUE);

    if (!m_SECSCommunicationOK) {
        m_lstbxMsg.AddString(_T("[Warning] Cannot send: HSMS not established yet"));
        return;
    }

    std::string json;

    if (m_cmbMsg == _T("S1F1")) {
        json = MakeRequest(1, 1, true, "{}");
    }
    else if (m_cmbMsg == _T("S1F13_1")) {
        json = MakeRequest(1, 13, true, "[]");
    }
    else if (m_cmbMsg == _T("S1F13_2")) {
        json = MakeRequest(1, 13, true,
            "[{\"type\":\"A\",\"value\":\"XComV2\"}"
            ",{\"type\":\"A\",\"value\":\"V2.0.0\"}]");
    }
    else if (m_cmbMsg == _T("S2F13")) {
        json = MakeRequest(2, 13, true,
            "[{\"type\":\"U2\",\"value\":1}"
            ",{\"type\":\"U2\",\"value\":2}]");
    }
    else if (m_cmbMsg == _T("S2F33")) {
        char data[1024];
        sprintf_s(data, sizeof(data),
            "[[{\"type\":\"I1\",\"value\":%d}"
            ",{\"type\":\"I2\",\"value\":%d}"
            ",{\"type\":\"I4\",\"value\":%ld}"
            ",{\"type\":\"I8\",\"value\":%lld}"
            ",{\"type\":\"U1\",\"value\":%u}"
            ",{\"type\":\"U2\",\"value\":%u}"
            ",{\"type\":\"U4\",\"value\":%lu}"
            ",{\"type\":\"U8\",\"value\":%llu}"
            ",{\"type\":\"F4\",\"value\":%.3f}"
            ",{\"type\":\"F8\",\"value\":%.8f}]"
            ",[{\"type\":\"A\",\"value\":\"%s\"}"
            ",{\"type\":\"JIS8\",\"value\":\"%s\"}"
            ",{\"type\":\"Binary\",\"value\":\"Cg==\"}"
            ",{\"type\":\"Boolean\",\"value\":true}]]",
            (int)VALUE_I1,   (int)VALUE_I2,
            (long)VALUE_I4,  (long long)VALUE_I8,
            (unsigned)VALUE_U1, (unsigned)VALUE_U2,
            (unsigned long)VALUE_U4, (unsigned long long)VALUE_U8,
            (double)VALUE_F4, (double)VALUE_F8,
            VALUE_STRING, VALUE_JIS8);
        json = MakeRequest(2, 33, true, data);
    }
    else if (m_cmbMsg == _T("S5F23")) {
        json = MakeRequest(5, 23, true,
            "[[1,1,1,1,1,1,1]"
            ",[1,1,1,1,1,1,1]"
            ",[1,2,3,4,5,6,7]"
            ",[1,2,3,4,5,6,7]"
            ",[1,2,3,4,5,6,7]"
            ",[1,2,3,4,5,6,7]"
            ",[1,2,3,4,5,6,7]"
            ",[1,2,3,4,5,6,7]"
            ",[1,2,3,4,5,6,7]"
            ",[1,2,3,4,5,6,7]"
            ",[1.0,2.0,3.0,4.0,5.0,6.0,7.0]"
            ",[1.0,2.0,3.0,4.0,5.0,6.0,7.0]]");
    }
    else {
        m_lstbxMsg.AddString(_T("[Warning] Undefined message"));
        return;
    }

    CString cs;
    if (m_Bridge.SendJson(json))
        cs = _T("Send ") + m_cmbMsg + _T(" successfully");
    else
        cs = _T("Fail to send ") + m_cmbMsg;
    m_lstbxMsg.AddString(cs);

    while (m_lstbxMsg.GetCount() > 200) m_lstbxMsg.DeleteString(0);
}


void CEqSampleDlg::OnLbnDblclkLstMsg()
{
    m_lstbxMsg.ResetContent();
}
