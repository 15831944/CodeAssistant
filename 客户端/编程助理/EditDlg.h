#pragma once
#include "afxwin.h"


// CEditDlg �Ի���

class CEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditDlg)

public:
	CEditDlg(CWnd* pParent = NULL);   // ��׼���췽��
	virtual ~CEditDlg();

	BOOL IsFunction;

	CString Class;
	CString Type;
	
	CEdit m_Class;
	CEdit m_Type;

// �Ի�������
	enum { IDD = IDD_EDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
