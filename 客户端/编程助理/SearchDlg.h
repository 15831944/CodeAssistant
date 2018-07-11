#pragma once
#include "afxwin.h"
#include "autocombox.h"


// CSearchDlg �Ի���

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(CWnd* pParent = NULL);   // ��׼���췽��
	virtual ~CSearchDlg();

	BOOL NeedFind, FindFile, Web_Data;
	CString Title, FilePath, Function;
	CAutoCombox m_ComboBox;

	// ������ʾ
	CString Error;
	void OnError();

	// �������߳�
	static UINT Operate(LPVOID pParam);
	CWinThread * m_hOperate;
	CString Resolve, Parameter;

// �Ի�������
	enum { IDD = IDD_SEARCH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
