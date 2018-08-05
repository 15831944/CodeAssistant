#pragma once
#include "afxcmn.h"
#include "olericheditctrl.h"

// CDocumentDlg 对话框

class CDocumentDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDocumentDlg)

public:
	CDocumentDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDocumentDlg();

	// 富文本编辑框
	COleRichEditCtrl m_Edit;

	// 成员变量
	BOOL IsOutFunction;
	CString FileClass, FileType, FilePath, FileName;

// 对话框数据
	enum { IDD = IDD_DOCUMENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
