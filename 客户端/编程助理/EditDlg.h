#pragma once
#include "afxwin.h"


// CEditDlg 对话框

class CEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditDlg)

public:
	CEditDlg(CWnd* pParent = NULL);   // 标准构造方法
	virtual ~CEditDlg();

	BOOL IsFunction;

	CString Class;
	CString Type;
	
	CEdit m_Class;
	CEdit m_Type;

// 对话框数据
	enum { IDD = IDD_EDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
