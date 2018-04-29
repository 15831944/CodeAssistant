#pragma once
#include "afxcmn.h"

#include ".\code_base\CustomDrawControl.h"

// CSynchronizeDlg 对话框

class CSynchronizeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSynchronizeDlg)

public:
	CSynchronizeDlg(CWnd* pParent = NULL);   // 标准构造方法
	virtual ~CSynchronizeDlg();

	CMultiSelTriCheckTreeCtrl m_Local;
	CMultiSelTriCheckTreeCtrl m_Server;

	// 代码标识&树控件选择对象是否是文件
	BOOL IsCode, IsFile;

	// post 参数类型
	int Type;

	// 方法
	void OnSuccess();
	void OnError();
	void GetServerInfo();
	void Split(CString source, CString divKey, CStringArray &dest);
	void GetTreeData(CTreeCtrl * pTreeCtrl, HTREEITEM hitem, BOOL IsCheck);

	// 工作者线程
	static UINT Operate(LPVOID pParam);
	CWinThread * m_hOperate;

	// 全局变量
	CString Parameter, UserName, UserId, Error, ServerInfo, TreeData;
	CStringArray TargetList;

// 对话框数据
	enum { IDD = IDD_SYNCHRONIZE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
