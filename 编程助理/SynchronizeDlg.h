#pragma once
#include "afxcmn.h"

#include ".\code_base\CustomDrawControl.h"

// CSynchronizeDlg �Ի���

class CSynchronizeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSynchronizeDlg)

public:
	CSynchronizeDlg(CWnd* pParent = NULL);   // ��׼���췽��
	virtual ~CSynchronizeDlg();

	CMultiSelTriCheckTreeCtrl m_Local;
	CMultiSelTriCheckTreeCtrl m_Server;

	// �����ʶ&���ؼ�ѡ������Ƿ����ļ�
	BOOL IsCode, IsFile;

	// post ��������
	int Type;

	// ����
	void OnSuccess();
	void OnError();
	void GetServerInfo();
	void Split(CString source, CString divKey, CStringArray &dest);
	void GetTreeData(CTreeCtrl * pTreeCtrl, HTREEITEM hitem, BOOL IsCheck);

	// �������߳�
	static UINT Operate(LPVOID pParam);
	CWinThread * m_hOperate;

	// ȫ�ֱ���
	CString Parameter, UserName, UserId, Error, ServerInfo, TreeData;
	CStringArray TargetList;

// �Ի�������
	enum { IDD = IDD_SYNCHRONIZE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void ShowFile(CString str_Dir, HTREEITEM tree_Root);

	void ConsistentParentCheck(CTreeCtrl * pTreeCtrl, HTREEITEM hTreeItem);
	void ConsistentChildCheck(CTreeCtrl  * pTreeCtrl, HTREEITEM hTreeItem);

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnClickLocalTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickServerTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkLocalTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkServerTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpload();
	afx_msg void OnDownload();
	afx_msg void OnSwitch();
	afx_msg void OnVersion();
};
