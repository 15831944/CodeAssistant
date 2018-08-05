#pragma once
#include "autocombox.h"
#include "afxwin.h"

#define ID_HOTKEY_A 6000
#define ID_HOTKEY_B 6001
#define ID_HOTKEY_C 6002
#define ID_HOTKEY_D 6003
#define ID_HOTKEY_E 6004
#define ID_HOTKEY_F 6005
#define ID_HOTKEY_G 6006
#define ID_HOTKEY_H 6007
#define ID_HOTKEY_I 6008
#define ID_HOTKEY_J 6009
#define ID_HOTKEY_K 6010
#define ID_HOTKEY_L 6011
#define ID_HOTKEY_M 6012
#define ID_HOTKEY_N 6013
#define ID_HOTKEY_O 6014
#define ID_HOTKEY_P 6015
#define ID_HOTKEY_Q 6016
#define ID_HOTKEY_R 6017
#define ID_HOTKEY_S 6018
#define ID_HOTKEY_T 6019
#define ID_HOTKEY_U 6020
#define ID_HOTKEY_V 6021
#define ID_HOTKEY_W 6022
#define ID_HOTKEY_X 6023
#define ID_HOTKEY_Y 6024
#define ID_HOTKEY_Z 6025


// CAssistantDlg 对话框

class CAssistantDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAssistantDlg)

public:
	CAssistantDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAssistantDlg();

	void OnSetCode();

// 对话框数据
	enum { IDD = IDD_ASSISTANT_DIALOG };

	// 成员变量
	HWND TargetWnd;
	CButton m_Hide;
	CButton m_Follow;
	CAutoCombox m_ComboBox;
	CString FilePath;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnDropdownCodeCombo();
	afx_msg void OnHideCheck();
	afx_msg void OnFollowCheck();
};
