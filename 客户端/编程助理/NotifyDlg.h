#pragma once


// CNotifyDlg �Ի���

class CNotifyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNotifyDlg)

public:
	CNotifyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNotifyDlg();

	static UINT NotifyThread(LPVOID lpParameter);

// �Ի�������
	enum { IDD = IDD_NOTIFY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
