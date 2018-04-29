#pragma once
#include "afxwin.h"

// CModifyDlg �Ի���

class CModifyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModifyDlg)

public:
	CModifyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CModifyDlg();

	// �༭ģʽ
	BOOL IsNew, IsProject;
	
	CComboBox m_ComboBox;
	
	CString m_Name;
	CString m_Path;
	CString m_Version;

	// ���ݲ�ѯ���
	vector<CString> pResult;

// �Ի�������
	enum { IDD = IDD_MODIFY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
