#pragma once
#include "sortlistctrl.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "olericheditctrl.h"
#include "SettingDlg.h"


// CMainDlg �Ի���

class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // ��׼���췽��
	virtual ~CMainDlg();

	CSortListCtrl m_List;
	CComboBox m_Class;
	CComboBox m_Type;
	COleRichEditCtrl m_Edit;

	void static DeleteDirectory(CString Directory);
	void Refresh(), OnHelp();
	void OnEditFunction();

	void Split(CString source, CString divKey, CStringArray &dest);
	void Complete();
	void OnError();

	void OnCheck();
	void OnSaveCode();
	void OnNewCode();

	void OnCopy(), OnPaste(), OnCut();
	void OnUndo(), OnRedo();
	BOOL GetPic(REOBJECT FAR* pObject);
	void SetPic(), OnImage(), OnScreenCapture();
	void OnLeft(), OnRight(), OnCenter();
	int  static CountFile(CString DirPath);

	bool Compress  (const char* scrfilename, const char* desfilename);
	bool Uncompress(const char* scrfilename, const char* desfilename);

// �Ի�������
	enum { IDD = IDD_MAIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ͼ��
	HICON m_hIcon;

	// ���öԻ���
	CSettingDlg * m_Setting;

	// ��������
	int Type;

	// �̶߳���
	CWinThread * m_hOperate, * m_hUpDate;

	CString UpDateInfo, Error, Msg, FilePath;
	BOOL IsNew, IsSave;

	IOleObject* m_pPicObj;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// �Ӵ�����Ϣ
	afx_msg LRESULT OnMessageChild(WPARAM wParam, LPARAM lParam);

	// �������߳�
	static UINT Operate(LPVOID pParam);
	static UINT UpDate (LPVOID pParam);

	// �����ؼ���Ϣӳ��
	afx_msg void OnSave();
	afx_msg void OnDelete();
	afx_msg void OnOK();
	afx_msg void OnFont();
	afx_msg void OnDirectory();
	afx_msg void OnRemove();

	afx_msg void OnClickCodeList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRClickCodeList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkCodeList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemchangedCodeList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnKillfocusClassCombo();
	afx_msg void OnKillfocusTypeCombo();

	afx_msg void OnDropdownClassCombo();
	afx_msg void OnDropdownTypeCombo();

	afx_msg void OnRichEditLink(NMHDR* in_pNotifyHeader, LRESULT*out_pResult ); 
	afx_msg void OnSetfocusCodeRichedit();
	afx_msg void OnKillfocusCodeRichedit();

	afx_msg void OnEdit();
	afx_msg void OnClearFormat();
	afx_msg void OnSynchronization();
	afx_msg void OnSetting();
	afx_msg void OnChangeCodeRichedit();
	afx_msg void OnManager();
	afx_msg void OnNew();

	virtual void OnCancel();
	
};
