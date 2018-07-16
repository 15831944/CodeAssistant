#pragma once
#include "afxwin.h"


// CSettingDlg �Ի���

class CSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // ��׼���췽��
	virtual ~CSettingDlg();

	void OnReadSetting();

// �Ի�������
	enum { IDD = IDD_SETTING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
