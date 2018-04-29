#pragma once
#include "afxwin.h"

// CModifyDlg 对话框

class CModifyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModifyDlg)

public:
	CModifyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModifyDlg();

	// 编辑模式
	BOOL IsNew, IsProject;
	
	CComboBox m_ComboBox;
	
	CString m_Name;
	CString m_Path;
	CString m_Version;

	// 数据查询结果
	vector<CString> pResult;

// 对话框数据
	enum { IDD = IDD_MODIFY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnChoose();
	afx_msg void OnUndo();
	afx_msg void OnCancel();
	afx_msg void OnSelchangeVersion();
};
