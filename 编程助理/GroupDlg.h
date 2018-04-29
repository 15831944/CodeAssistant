#pragma once


// CGroupDlg 对话框

class CGroupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGroupDlg)

public:
	CGroupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGroupDlg();

	BOOL IsNew;

	CString m_UserName;
	CString m_Role;
	CString m_Version;

// 对话框数据
	enum { IDD = IDD_GROUP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnOK();
	afx_msg void OnSearch();
	afx_msg void OnSelchangeRoleCombo();
	afx_msg void OnUndo();
	afx_msg void OnCancel();
};
