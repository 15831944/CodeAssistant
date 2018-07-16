#pragma once
#include "sortlistctrl.h"
#include<vector>

// CProjectDlg �Ի���

class CProjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProjectDlg)

public:
	CProjectDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProjectDlg();

	// ȫ�ֱ���
	CString Name, Path, FilePath, TargetPath;

	// ��������
	int Type;

	// �������߳�
	static UINT Operate(LPVOID pParam), Notify(LPVOID pParam);
	CWinThread * m_hOperate;

	// �б�
	CSortListCtrl m_Project;
	CSortListCtrl m_Version;

	// ���ݲ�ѯ���
	vector<CString> pResult;

	void Refresh();
	void CopyDirectory(CString source, CString target);
	static void DeleteDirectory(CString Directory);

	// �����ȴ���ʾ
	void OnShowNotify(CString Notify);

// �Ի�������
	enum { IDD = IDD_PROJECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	afx_msg void OnAdd();
	afx_msg void OnRestore();
	afx_msg void OnModify();
	afx_msg void OnRemove();

	afx_msg void OnItemchangedProjectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickProjectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkProjectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickProjectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkVersionList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickVersionList(NMHDR *pNMHDR, LRESULT *pResult);
};
