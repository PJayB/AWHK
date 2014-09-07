
// AWHKConfigAppDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CAWHKConfigAppDlg dialog
class CAWHKConfigAppDlg : public CDialogEx
{
// Construction
public:
	CAWHKConfigAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AWHKCONFIGAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	void OnHelpAbout();
	void OnHelpOnlineHelp();
	void OnApplicationUnloadAWHK();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedApply();
};
