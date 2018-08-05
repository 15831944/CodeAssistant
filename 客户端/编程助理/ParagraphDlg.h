#pragma once
#include "afxwin.h"


// CParagraphDlg 对话框

class CParagraphDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParagraphDlg)

public:
	CParagraphDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParagraphDlg();

	int Alignment,Tab;
	CString m_Index;
	CString m_Right;
	CString m_Spacing;
	CString m_Part;
	CComboBox m_Align;
    CComboBox m_Style;
	CString m_Tab;

// 对话框数据
	enum { IDD = IDD_PARAGRAPH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnUndo();
};
