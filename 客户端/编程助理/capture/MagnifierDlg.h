#pragma once


// CMagnifierDlg �Ի���

class CMagnifierDlg : public CDialog
{
public:
	CMagnifierDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMagnifierDlg();
	BOOL OnInitDialog();
	void OnCancel();
	void MagnifyScreen();
// �Ի�������
	enum { IDD = IDD_MAGNIFIER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
