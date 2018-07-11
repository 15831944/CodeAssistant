#pragma once

#include "sortlistctrl.h"

// CWebEditDlg 对话框

class CWebEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWebEditDlg)

public:
	CWebEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWebEditDlg();

	// 成员变量
	int Type;
	BOOL IsNew, IsProject;
	CString UserId, UserName, Parameter, Error, ProjectId, VersionId, OwnerId;
	
	CString m_Project;
	CString m_Version;
	CString m_Type;
	CString m_Path;
	CString m_Share;

	// 方法
	void OnSuccess();
	void OnError();
	void Split(CString source, CString divKey, CStringArray &dest);
	BOOL OnCheck(CString Target);

// 对话框数据
	enum { IDD = IDD_WEB_EDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 列表
	CSortListCtrl m_Group;

	// 工作者线程
	static UINT Operate(LPVOID pParam);
	CWinThread * m_hOperate;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnUndo();
	afx_msg void OnCancel();
	afx_msg void OnSelect();
	afx_msg void OnSelchangeShareCombo();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnEdit();
};
