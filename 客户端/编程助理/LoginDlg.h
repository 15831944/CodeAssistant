#pragma once
#include "afxwin.h"


// CLoginDlg �Ի���

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // ��׼���췽��
	virtual ~CLoginDlg();

	// ����
	void OnSuccess();
	void OnError();

	// �������߳�
	static UINT Login(LPVOID pParam);
	CWinThread * m_hOperate;

	// ȫ�ֱ���
	CString Parameter, UserId, Error;
	CToolTipCtrl m_toolTips;


// �Ի�������
	enum { IDD = IDD_LOGIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnRegister();
	afx_msg void OnCancel();
	afx_msg void OnRemember();
	afx_msg void OnAuto();
	CButton m_Remember;
	CButton m_Auto;
};
