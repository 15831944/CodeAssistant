#pragma once


// CRegisterDlg �Ի���

class CRegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegisterDlg)

public:
	CRegisterDlg(CWnd* pParent = NULL);   // ��׼���췽��
	virtual ~CRegisterDlg();

	// ����
	void OnSuccess();
	void OnError();

	// �������߳�
	static UINT Register(LPVOID pParam);
	CWinThread * m_hOperate;

	// ȫ�ֱ���
	CString Parameter, Error;
	CString UserName, Password;

// �Ի�������
	enum { IDD = IDD_REGISTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
