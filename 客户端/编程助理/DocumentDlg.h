#pragma once
#include "afxcmn.h"
#include "olericheditctrl.h"

// CDocumentDlg �Ի���

class CDocumentDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDocumentDlg)

public:
	CDocumentDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDocumentDlg();

	// ���ı��༭��
	COleRichEditCtrl m_Edit;

	// ��Ա����
	BOOL IsOutFunction;
	CString FileClass, FileType, FilePath, FileName;

// �Ի�������
	enum { IDD = IDD_DOCUMENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSetfocusCodeRichedit();
	afx_msg void OnChangeCodeRichedit();
	afx_msg void OnRichEditLink(NMHDR* in_pNotifyHeader, LRESULT*out_pResult ); 
};
