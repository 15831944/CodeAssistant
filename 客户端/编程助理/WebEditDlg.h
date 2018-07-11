#pragma once

#include "sortlistctrl.h"

// CWebEditDlg �Ի���

class CWebEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWebEditDlg)

public:
	CWebEditDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWebEditDlg();

	// ��Ա����
	int Type;
	BOOL IsNew, IsProject;
	CString UserId, UserName, Parameter, Error, ProjectId, VersionId, OwnerId;
	
	CString m_Project;
	CString m_Version;
	CString m_Type;
	CString m_Path;
	CString m_Share;

	// ����
	void OnSuccess();
	void OnError();
	void Split(CString source, CString divKey, CStringArray &dest);
	BOOL OnCheck(CString Target);

// �Ի�������
	enum { IDD = IDD_WEB_EDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// �б�
	CSortListCtrl m_Group;

	// �������߳�
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
