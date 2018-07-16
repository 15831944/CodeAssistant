#pragma once
#include "sortlistctrl.h"
#include<vector>

// CProjectDlg 对话框

class CProjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProjectDlg)

public:
	CProjectDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProjectDlg();

	// 全局变量
	CString Name, Path, FilePath, TargetPath;

	// 操作类型
	int Type;

	// 工作者线程
	static UINT Operate(LPVOID pParam), Notify(LPVOID pParam);
	CWinThread * m_hOperate;

	// 列表
	CSortListCtrl m_Project;
	CSortListCtrl m_Version;

	// 数据查询结果
	vector<CString> pResult;

	void Refresh();
	void CopyDirectory(CString source, CString target);
	static void DeleteDirectory(CString Directory);

	// 操作等待提示
	void OnShowNotify(CString Notify);

// 对话框数据
	enum { IDD = IDD_PROJECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
