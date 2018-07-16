#pragma once
#include "afxwin.h"


// CSettingDlg 对话框

class CSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // 标准构造方法
	virtual ~CSettingDlg();

	void OnReadSetting();

// 对话框数据
	enum { IDD = IDD_SETTING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CComboBox m_Skin;
	CString SkinName;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSelchangeSkinCombo();
	afx_msg void OnDropdownSkinCombo();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnLogin();
	afx_msg void OnSynchronize();
	afx_msg void OnPassword();
};
