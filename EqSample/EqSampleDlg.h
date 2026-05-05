
// EqSampleDlg.h : ?§Îçî ?åÏùº
//

#pragma once
#include "BridgeClient.h"

#define MAX_ARRAY	7
// CEqSampleDlg ?Ä???ÅÏûê
class CEqSampleDlg : public CDialogEx
{
// ?ùÏÑ±?ÖÎãà??
public:
	CEqSampleDlg(CWnd* pParent = NULL);	// ?úÏ? ?ùÏÑ±?êÏûÖ?àÎã§.

// ?Ä???ÅÏûê ?∞Ïù¥?∞ÏûÖ?àÎã§.
	enum { IDD = IDD_EQSAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ÏßÄ?êÏûÖ?àÎã§.


// Íµ¨ÌòÑ?ÖÎãà??
protected:
	HICON m_hIcon;

	// ?ùÏÑ±??Î©îÏãúÏßÄ Îß??®Ïàò
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedOk();
	CString m_cmbDevice;
	CString m_cmbMsg;
	CListBox m_lstbxMsg;

// Implementation
private:
    // Variable for array
    char VALUE_I1_ARR[MAX_ARRAY];
    short VALUE_I2_ARR[MAX_ARRAY];
    long VALUE_I4_ARR[MAX_ARRAY];
    LONGLONG VALUE_I8_ARR[MAX_ARRAY];
    BYTE VALUE_U1_ARR[MAX_ARRAY];
    WORD VALUE_U2_ARR[MAX_ARRAY];
    ULONG VALUE_U4_ARR[MAX_ARRAY];
    ULONGLONG VALUE_U8_ARR[MAX_ARRAY];
    float VALUE_F4_ARR[MAX_ARRAY];
    double VALUE_F8_ARR[MAX_ARRAY];
    BYTE VALUE_BOOL_ARR[MAX_ARRAY];
    BYTE VALUE_BINARY_ARR[MAX_ARRAY];
	bool m_SECSCommunicationOK;

public:
	CBridgeClient m_Bridge;
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnSendMsg();
	afx_msg LRESULT OnBridgeMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBridgeConn(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLbnDblclkLstMsg();
};
