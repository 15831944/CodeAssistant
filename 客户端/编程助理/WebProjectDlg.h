#pragma once

#include "sortlistctrl.h"
#include<vector>

// CWebProjectDlg �Ի���

class CWebProjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWebProjectDlg)

public:
	CWebProjectDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWebProjectDlg();

	// ȫ�ֱ���
	int DataSource, ProjectType, OperateType, Total;
	CString UserId, UserName, Parameter, Error, Project, Number, OwnerId, Role, Share, Remark, Version, Path, Type, ProjectId, VersionId;

	// �������߳�
	static UINT Operate(LPVOID pParam);
	CWinThread * m_hOperate;

	// ����
	void OnSuccess();
	void OnError();
	void AddTarget(CString Path, CStringArray &TargetList);
	void CopyDirectory(CString source, CString target);
	void AddProject(), AddVersion();

	// �б�
	CSortListCtrl m_Project;
	CSortListCtrl m_Version;

	// ���ݲ�ѯ���
	vector<CString> pResult;

	void Split(CString source, CString divKey, CStringArray &dest);


// �Ի�������
	enum { IDD = IDD_WEB_PROJECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	CToolTipCtrl m_toolTips;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnAssistant();
	afx_msg void OnGithub();
	afx_msg void OnGitee();
	afx_msg void OnSourceforge();
	afx_msg void OnGoogle();
	afx_msg void OnNew();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	afx_msg void OnSwitch();
	afx_msg void OnAdd();
	afx_msg void OnMerge();
	afx_msg void OnModify();
	afx_msg void OnRemove();
	afx_msg void OnClone();

	afx_msg void OnItemchangedProjectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickProjectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkProjectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickProjectList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnItemchangedVersionList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickVersionList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkVersionList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickVersionList(NMHDR *pNMHDR, LRESULT *pResult);
};
