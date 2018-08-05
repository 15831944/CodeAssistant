#pragma once
#include "afxwin.h"


// CParagraphDlg �Ի���

class CParagraphDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParagraphDlg)

public:
	CParagraphDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParagraphDlg();

	int Alignment,Tab;
	CString m_Index;
	CString m_Right;
	CString m_Spacing;
	CString m_Part;
	CComboBox m_Align;
    CComboBox m_Style;
	CString m_Tab;

// �Ի�������
	enum { IDD = IDD_PARAGRAPH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnUndo();
};
